/**************************************************//*
	@file	| Pioneer_Job.cpp
	@brief	| 開拓職業クラスのcppファイル
	@note	| 建築可能な土地を広げる職業の処理を実装
			| CCrafter_Strategyを継承
*//**************************************************/
#include "Pioneer_Job.h"
#include "Main.h"
#include "Human.h"
#include "FieldManager.h"
#include "CollectTarget.h"
#include "CivLevelManager.h"
#include "StructMath.h"
#include "Oparation.h"
#include "ImguiSystem.h"
#include <set>
#include <map>
#include <unordered_map>
#include <cfloat>

namespace
{
	// @brief 開拓の完了に必要な進行度
	constexpr float Pioneer_Required_Progress = 100.0f;
	// @brief 労働力1あたりの毎フレームの進行量
	constexpr float Pioneer_Progress_Rate = 0.15f;
	// @brief 木・岩・草を伐り拓く場合の進行量の倍率(空き地より時間がかかる)
	constexpr float Pioneer_Occupied_Progress_Scale = 0.5f;
	// @brief 維持したい建築可能な空きセル数の基準値
	constexpr int Pioneer_Free_Cells_Base = 20;
	// @brief 文明レベル1あたりに増える空きセル数
	constexpr int Pioneer_Free_Cells_Per_Level = 5;
	// @brief 土地探索の間隔(秒)
	constexpr float Pioneer_Search_Interval = 0.5f;

	// @brief 開拓予定のセルのキー(複数の開拓者が同じセルを選ばないようにするための予約)
	std::set<int> s_ClaimedCells;

	// @brief セルインデックスを予約用のキーに変換する
	int MakeCellKey(const DirectX::XMINT2& n2Index)
	{
		return n2Index.x * CFieldGrid::GridSizeY + n2Index.y;
	}

	// @brief 開拓できるセルかどうかを判定する
	// @param pCell：判定するセル
	// @param pOccupant：セルに残っている採取対象(無ければnullptr)
	// @note 動物の縄張りは開拓できない
	//       空き地は何も使用されていなければ開拓できる
	//       木・岩・草のセルは、採取職が標的にしていなければ伐り拓いて開拓できる
	bool IsClearableCell(CFieldCell* pCell, CCollectTarget* pOccupant)
	{
		if (pCell == nullptr) return false;

		// 動物の縄張りは開拓しない
		const CFieldCell::TerritoryType eTerritory = pCell->GetTerritoryType();
		if (eTerritory != CFieldCell::TerritoryType::NONE && eTerritory != CFieldCell::TerritoryType::Human) return false;

		switch (pCell->GetCellType())
		{
		case CFieldCell::CellType::EMPTY:
			return !pCell->IsUse() && pOccupant == nullptr;

		case CFieldCell::CellType::TREE:
		case CFieldCell::CellType::ROCK:
		case CFieldCell::CellType::GRASS:
			// 採取対象が残っていない(採取済み)なら、使用フラグが残っていても開拓できる
			if (pOccupant == nullptr) return true;
			// 採取職が標的にしている採取対象は避ける
			return pOccupant->GetTargetingID().m_nSameCount == -1;

		default:
			return false;
		}
	}
}

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CPioneer_Job::CPioneer_Job()
	: CCrafter_Strategy()
{
	// 開拓職業の労働力を設定
	m_fWorkPower = 10.0f;
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CPioneer_Job::~CPioneer_Job()
{
	// 開拓予定の土地の予約を解除
	// (シーン終了時の破棄順序の影響を受けないよう、シーンのオブジェクトには触れない)
	ReleaseClaim();
}

/*****************************************//*
	@brief	| 仕事処理
*//*****************************************/
void CPioneer_Job::DoWork()
{
	switch (m_eCurrentState)
	{
		// 開拓する土地を探す
	case WorkState::Searching:
		SearchingAction();
		break;

		// 開拓する土地へ移動
	case WorkState::Moving:
		MovingAction();
		break;

		// 開拓中
	case WorkState::Clearing:
		ClearingAction();
		break;

		// 休憩中
	case WorkState::Resting:
		RestingAction();
		break;
	}
}

/*****************************************//*
	@brief	| 職業切り替え時の処理
*//*****************************************/
void CPioneer_Job::OnChangeJob()
{
	// 開拓予定の土地の予約を解除
	ReleaseClaim();

	// 仕事状態を初期化
	m_eCurrentState = WorkState::Searching;
	m_ePrevState = WorkState::Searching;
	m_fProgress = 0.0f;
	m_fSearchTimer = 0.0f;
}

/*****************************************//*
	@brief	| インスペクター表示処理
	@param	| isEnd：インスペクター終了フラグ
	@return	| 表示した項目数
*//*****************************************/
int CPioneer_Job::Inspecter(bool isEnd)
{
	int nItemCount = 0;

	ImGui::BeginChild("Pioneer Job Inspector", ImVec2(0, 150), true);

	// 職業名の表示
	ImGui::Text(u8"職業名:%s", GetJobName().c_str());

	// ステータスの表示
	ImGui::Text(u8"労働力:%.2f", m_fWorkPower);

	// 現在の仕事状態の表示
	ImGui::Text(u8"現在の状態:%s", GetStateName());

	// 開拓の進行度の表示
	ImGui::ProgressBar(m_fProgress / Pioneer_Required_Progress, ImVec2(0.0f, 0.0f), u8"開拓の進行度");

	ImGui::EndChild();
	nItemCount++;

	if (isEnd)
	{
		ImGui::EndChild();
		ImGui::End();
		nItemCount++;
	}

	return nItemCount;
}

/*****************************************//*
	@brief	| 職業ステータスのImGui描画処理
*//*****************************************/
void CPioneer_Job::DrawJobStatusImGui()
{
	// 職業ステータスの基本表示
	IJob_Strategy::DrawJobStatusImGui();

	// 現在の仕事状態の表示
	ImGui::Text(u8"状態: %s", GetStateName());

	// 建築可能な空きセル数の表示
	ImGui::Text(u8"建築可能な空き地: %d / 目標 %d", GetFreeBuildCellCount(), GetTargetFreeCellCount());

	// 開拓する土地の表示
	if (m_bHasTarget)
	{
		ImGui::Text(u8"開拓予定: (%d, %d)%s", m_n2TargetIndex.x, m_n2TargetIndex.y, m_bHasOccupant ? u8" ※伐り拓く" : "");
	}
	else
	{
		ImGui::Text(u8"開拓予定なし");
	}
}

/*****************************************//*
	@brief	| 開拓する土地を探す処理
*//*****************************************/
void CPioneer_Job::SearchingAction()
{
	// 土地の探索は負荷軽減のため一定間隔で行う
	m_fSearchTimer -= fDeltaTime;
	if (m_fSearchTimer > 0.0f) return;
	m_fSearchTimer = Pioneer_Search_Interval;

	// 建築可能な空き地が十分にある間は開拓しない
	if (GetFreeBuildCellCount() >= GetTargetFreeCellCount()) return;

	// 開拓する土地を決める
	if (!FindClearTarget()) return;

	m_fProgress = 0.0f;
	ChangeState(WorkState::Moving);
}

/*****************************************//*
	@brief	| 開拓する土地へ移動する処理
*//*****************************************/
void CPioneer_Job::MovingAction()
{
	CFieldCell* pCell = GetTargetCell();

	// 既に建築可能な土地になっていた場合は探し直す
	if (pCell == nullptr || pCell->GetCellType() == CFieldCell::CellType::Build)
	{
		ReleaseClaim();
		ChangeState(WorkState::Searching);
		return;
	}

	// 土地に到達したら開拓を始める
	if (MoveToCell(pCell))
	{
		ChangeState(WorkState::Clearing);
	}
}

/*****************************************//*
	@brief	| 開拓中の処理
*//*****************************************/
void CPioneer_Job::ClearingAction()
{
	CFieldCell* pCell = GetTargetCell();

	// 既に建築可能な土地になっていた場合は探し直す
	if (pCell == nullptr || pCell->GetCellType() == CFieldCell::CellType::Build)
	{
		ReleaseClaim();
		ChangeState(WorkState::Searching);
		return;
	}

	// 開拓を進める(木・岩・草を伐り拓く場合は時間がかかる)
	const float fScale = m_bHasOccupant ? Pioneer_Occupied_Progress_Scale : 1.0f;
	m_fProgress += m_fWorkPower * Pioneer_Progress_Rate * fScale;

	// 空腹度とスタミナを減少させる
	m_pOwner->DecreaseHunger(Work_Hunger_Decrease);
	m_pOwner->DecreaseStamina(Job_Work_Stamina_Decrease);

	// 開拓が完了した場合
	if (m_fProgress >= Pioneer_Required_Progress)
	{
		// 伐り拓く採取対象が残っている場合は撤去する
		CCollectTarget* pOccupant = ResolveOccupant();
		if (pOccupant != nullptr)
		{
			// 採取職が標的にしている間は撤去できないので、採取が終わるまで待つ
			if (pOccupant->GetTargetingID().m_nSameCount != -1) return;

			// 同じフレーム内で採取職に標的にされないよう、先に自分を標的に設定してから撤去する
			pOccupant->SetTargetingID(m_pOwner->GetID());
			pOccupant->Destroy();
		}

		// セルを空け、建築可能な土地にして、人間の縄張りにする
		pCell->SetUse(false);
		pCell->SetObject(nullptr);
		pCell->SetCellType(CFieldCell::CellType::Build);
		pCell->SetTerritoryType(CFieldCell::TerritoryType::Human);

		// 文明経験値を加算
		CCivLevelManager::GetInstance()->AddExp(CCivLevelManager::ExpType::Production);

		// 次の土地を探す
		ReleaseClaim();
		m_fProgress = 0.0f;
		ChangeState(WorkState::Searching);
		return;
	}

	// スタミナが0になったら休憩する
	if (m_pOwner->IsZeroStamina())
	{
		ChangeState(WorkState::Resting);
	}
}

/*****************************************//*
	@brief	| 休憩中の処理
*//*****************************************/
void CPioneer_Job::RestingAction()
{
	// 休憩が完了していない場合は処理を抜ける
	if (!RestAction()) return;

	// 休憩前の作業に戻る(開拓予定の土地が無い場合は探索から)
	ChangeState(m_bHasTarget ? m_ePrevState : WorkState::Searching);
}

/*****************************************//*
	@brief	| 開拓する土地を決定する処理
	@return	| true:決定した false:開拓できる土地が無い
	@note	| 建築可能な土地に隣接する開拓可能な土地のうち、最も近い土地を選ぶ
*//*****************************************/
bool CPioneer_Job::FindClearTarget()
{
	auto& fieldCells = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells();
	const DirectX::XMFLOAT3 f3MyPos = m_pOwner->GetPos();

	// 採取対象(木・岩・草)を、連携しているセルのキーから引けるようにする
	// 上下左右の隣接セルの方向
	const int nDirX[4] = { 1, -1, 0, 0 };
	const int nDirY[4] = { 0, 0, 1, -1 };

	float fBestDistance = FLT_MAX;
	DirectX::XMINT2 n2Best = DirectX::XMINT2(-1, -1);
	CCollectTarget* pBestOccupant = nullptr;

	const int nSizeX = static_cast<int>(fieldCells.size());
	for (int x = 0; x < nSizeX; ++x)
	{
		const int nSizeY = static_cast<int>(fieldCells[x].size());
		for (int y = 0; y < nSizeY; ++y)
		{
			// 建築可能な土地から広げていく
			if (fieldCells[x][y]->GetCellType() != CFieldCell::CellType::Build) continue;

			for (int i = 0; i < 4; ++i)
			{
				const int nx = x + nDirX[i];
				const int ny = y + nDirY[i];
				if (nx < 0 || nx >= nSizeX || ny < 0 || ny >= static_cast<int>(fieldCells[nx].size())) continue;

				CFieldCell* pNeighbor = fieldCells[nx][ny];
				const int nKey = MakeCellKey(pNeighbor->GetIndex());

				// 他の開拓者が開拓予定の土地は選ばない
				if (s_ClaimedCells.count(nKey) > 0) continue;

				// セルに残っている採取対象を取得
				CCollectTarget* pOccupant = dynamic_cast<CCollectTarget*>(pNeighbor->GetObject());
				if (pOccupant != nullptr && pOccupant->IsDestroy()) pOccupant = nullptr;

				if (!IsClearableCell(pNeighbor, pOccupant)) continue;

				const float fDistance = StructMath::Distance(f3MyPos, pNeighbor->GetPos());
				if (fDistance < fBestDistance)
				{
					fBestDistance = fDistance;
					n2Best = pNeighbor->GetIndex();
					pBestOccupant = pOccupant;
				}
			}
		}
	}

	// 開拓できる土地が無い場合
	if (n2Best.x < 0) return false;

	// 開拓する土地を予約する
	m_n2TargetIndex = n2Best;
	m_bHasTarget = true;
	m_bHasOccupant = (pBestOccupant != nullptr);
	m_OccupantID = (pBestOccupant != nullptr) ? pBestOccupant->GetID() : ObjectID{ "", -1 };
	s_ClaimedCells.insert(MakeCellKey(n2Best));
	return true;
}

/*****************************************//*
	@brief	| 開拓する土地のセルを取得する処理
	@return	| セルのポインタ、開拓する土地が決まっていない場合はnullptr
*//*****************************************/
CFieldCell* CPioneer_Job::GetTargetCell() const
{
	if (!m_bHasTarget) return nullptr;

	return CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells()[m_n2TargetIndex.x][m_n2TargetIndex.y];
}

/*****************************************//*
	@brief	| 開拓する土地に残っている採取対象を取得する処理
	@return	| 採取対象のポインタ、無い/既に無くなっている場合はnullptr
*//*****************************************/
CCollectTarget* CPioneer_Job::ResolveOccupant() const
{
	if (!m_bHasOccupant) return nullptr;

	// 採取職に先に採取されて破棄されることがあるため、IDから毎回探し直す
	CCollectTarget* pOccupant = GetScene()->FindGameObjectByID<CCollectTarget>(m_OccupantID);
	if (pOccupant == nullptr || pOccupant->IsDestroy()) return nullptr;

	return pOccupant;
}

/*****************************************//*
	@brief	| 開拓する土地の予約を解除する処理
*//*****************************************/
void CPioneer_Job::ReleaseClaim()
{
	if (m_bHasTarget)
	{
		s_ClaimedCells.erase(MakeCellKey(m_n2TargetIndex));
	}

	m_bHasTarget = false;
	m_n2TargetIndex = DirectX::XMINT2(-1, -1);
	m_bHasOccupant = false;
	m_OccupantID = ObjectID{ "", -1 };
}

/*****************************************//*
	@brief	| 指定したセルの位置まで移動する処理
	@param	| pCell：移動先のセルのポインタ
	@return	| true:到達 false:到達していない
*//*****************************************/
bool CPioneer_Job::MoveToCell(CFieldCell* pCell)
{
	DirectX::XMFLOAT3 f3OwnerPos = m_pOwner->GetPos();
	const DirectX::XMFLOAT3 f3TargetPos = pCell->GetPos();

	// 一定距離以内に到達したら移動完了
	if (StructMath::Distance(f3OwnerPos, f3TargetPos) < 1.0f) return true;

	// 目的地に向かって少しずつ移動する
	const DirectX::XMFLOAT3 f3Direction = StructMath::Normalize(f3TargetPos - f3OwnerPos);
	f3OwnerPos += f3Direction * Human_Move_Speed;
	m_pOwner->SetPos(f3OwnerPos);

	return false;
}

/*****************************************//*
	@brief	| 現在の建築可能な空きセル数を取得する処理
	@return	| 建築可能で、まだ使われていないセルの数
*//*****************************************/
int CPioneer_Job::GetFreeBuildCellCount() const
{
	return static_cast<int>(
		CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells(CFieldCell::CellType::Build, false).size());
}

/*****************************************//*
	@brief	| 文明レベルに応じて維持したい建築可能な空きセル数を取得する処理
	@return	| 目標とする空きセル数
*//*****************************************/
int CPioneer_Job::GetTargetFreeCellCount() const
{
	return Pioneer_Free_Cells_Base + CCivLevelManager::GetInstance()->GetCivLevel() * Pioneer_Free_Cells_Per_Level;
}

/*****************************************//*
	@brief	| 現在の仕事状態名を取得する処理
	@return	| 仕事状態名の文字列
*//*****************************************/
const char* CPioneer_Job::GetStateName() const
{
	switch (m_eCurrentState)
	{
	case WorkState::Searching:	return u8"開拓地を探索中";
	case WorkState::Moving:		return u8"開拓地へ移動中";
	case WorkState::Clearing:	return u8"開拓中";
	case WorkState::Resting:	return u8"休憩中";
	default:					return "Unknown";
	}
}

/*****************************************//*
	@brief	| 開拓予定の土地の予約を全て解除する
*//*****************************************/
void CPioneer_Job::ResetClaims()
{
	s_ClaimedCells.clear();
}
