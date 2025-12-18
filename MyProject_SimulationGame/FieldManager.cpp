/**************************************************//*
	@file	| FieldManager.cpp
	@brief	| フィールド管理クラスのcppファイル
	@note	| フィールド全体の管理を行う
			| シングルトンパターンで作成
*//**************************************************/
#include "FieldManager.h"
#include "PerlinNoice.h"
#include "GeneratorManager.h"
#include <random>
#include "Main.h"
#include "StorageHouse.h"
#include "RefreshFacility.h"
#include "HumanHouse.h"
#include "ImguiSystem.h"

// 初期村のサイズ
const int INITIAL_VILLAGE_SIZE_X = 5;	// 初期村のXサイズ
const int INITIAL_VILLAGE_SIZE_Y = 5;	// 初期村のYサイズ

// フィールドデバック表示サイズ
constexpr int DEBUG_DRAW_SIZE = 50; // DEBUG_DRAW_SIZE x DEBUG_DRAW_SIZE の範囲で表示

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CFieldManager::CFieldManager()
{
	// フィールドグリッドの生成
	m_pFieldGrid = new(std::nothrow) CFieldGrid({ 0.0f, 0.0f, 0.0f });
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CFieldManager::~CFieldManager()
{
	// フィールドグリッドの解放
	delete m_pFieldGrid;
	m_pFieldGrid = nullptr;
}

/*****************************************//*
	@brief　	| フィールドセルのタイプ選出
*//*****************************************/
void CFieldManager::AssignFieldCellType()
{
	float scale = 0.1f; // ノイズのスケール

	// ランダムデバイスでシード値を生成
	std::random_device rd;
	unsigned int seed = rd();

	// パーリンノイズ生成クラスのインスタンス
	PerlinNoise perlinNoise(seed);

	// フィールドセルの2次元配列を取得
	auto fieldCells = m_pFieldGrid->GetFieldCells();

	int objCount = 0;
	// 各フィールドセルに対してタイプをランダムに割り当てる
	for (int x = 0; x < fieldCells.size(); ++x)
	{
		for (int y = 0; y < fieldCells[x].size(); ++y)
		{
			// パーリンノイズの値を取得
			float noiseValue = perlinNoise.noise(x * scale, y * scale);

			// 正規化 0.0f ～ 1.0f に変換
			noiseValue = (noiseValue + 1.0f) / 2.0f;

			// ノイズ値に基づいてセルタイプを決定
			// 最大値：1.0f、最小値：0.0f

			// 木の出現値(最小値：0.0f|最大値：0.4f)
			if (noiseValue >= 0.0f && noiseValue <= 0.4)
			{
				fieldCells[x][y]->SetCellType(CFieldCell::CellType::TREE);
				objCount++;
			}
			// 草地の出現値(最小値：0.45f|最大値：0.5f)
			else if (noiseValue >= 0.45f && noiseValue <= 0.5f)
			{
				fieldCells[x][y]->SetCellType(CFieldCell::CellType::GRASS);
			}
			// 岩の出現値(最小値：0.7f|最大値：0.9f)
			else if (noiseValue >= 0.7f && noiseValue <= 0.9f)
			{
				fieldCells[x][y]->SetCellType(CFieldCell::CellType::ROCK);
			}
			// 空地の出現値
			else
			{
				fieldCells[x][y]->SetCellType(CFieldCell::CellType::EMPTY);
				objCount++;
			}
		}
	}

	// 縄張りの作成
	CreateTerritory();

	// 初期村の配置
	CreateInitialVillage();

	// 生成通知
	CGeneratorManager::GetInstance()->NotifyObservers();
}

/*****************************************//*
	@brief　	| フィールドグリッドの表示
*//*****************************************/
void CFieldManager::DebugDraw()
{
	// フィールドセルの2次元配列を取得
	auto fieldCells = m_pFieldGrid->GetFieldCells();

	int halfSizeX = (CFieldGrid::GridSizeX - DEBUG_DRAW_SIZE) / 2;
	int halfSizeY = (CFieldGrid::GridSizeY - DEBUG_DRAW_SIZE) / 2;

	DirectX::XMINT2 CenterPos = CImguiSystem::GetInstance()->GetFieldCellDebugCenterPos();

	// 各フィールドセルのデバック描画を実行
	for(int x = halfSizeX + CenterPos.x; x < DEBUG_DRAW_SIZE + halfSizeX + CenterPos.x; ++x)
	{
		if (x >= fieldCells.size())continue;
		for(int y = halfSizeY + CenterPos.y; y < DEBUG_DRAW_SIZE + halfSizeY + CenterPos.y; ++y)
		{
			if (y >= fieldCells[0].size())continue;
			fieldCells[x][y]->DebugDraw(CImguiSystem::GetInstance()->GetFieldCellDisplayMode());
		}
	}
}

/*****************************************//*
	@brief　	| 初期村の作成
	@note		| フィールドの中央付近に初期村を作成
*//*****************************************/
void CFieldManager::CreateInitialVillage()
{
	// ハーフサイズを取得
	int halfSizeX = CFieldGrid::GridSizeX / 2;
	int halfSizeY = CFieldGrid::GridSizeY / 2;

	// フィールドセルの2次元配列を取得
	auto fieldCells = m_pFieldGrid->GetFieldCells();

	for(int i = -INITIAL_VILLAGE_SIZE_X / 2; i <= INITIAL_VILLAGE_SIZE_X / 2; ++i)
	{
		for(int j = -INITIAL_VILLAGE_SIZE_Y / 2; j <= INITIAL_VILLAGE_SIZE_Y / 2; ++j)
		{
			// フィールドセルのインデックスを計算
			int cellX = halfSizeX + i;
			int cellY = halfSizeY + j;

			// フィールドセルのタイプを建築可能地に設定
			fieldCells[cellX][cellY]->SetCellType(CFieldCell::CellType::Build);
			// フィールドセルの縄張りタイプを人間に設定
			fieldCells[cellX][cellY]->SetTerritoryType(CFieldCell::TerritoryType::Human);
		}
	}

	// シーンの取得
	CScene* pScene = GetScene();

	// 建築可能なフィールドセルを取得
	auto cells = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells(CFieldCell::CellType::Build, false);

	// ランダムにセルを選択
	int randomIndex = rand() % cells.size();

	// 貯蔵庫の生成と配置
	CBuildObject* pStorageHouse = pScene->AddGameObject<CStorageHouse>(Tag::GameObject, u8"貯蔵庫");
	pStorageHouse->SetPos(cells[randomIndex]->GetPos());
	cells[randomIndex]->SetUse(true);
	cells[randomIndex]->SetObject(pStorageHouse);
	pStorageHouse->SetFieldCellIndex(cells[randomIndex]->GetIndex());

	// 貯蔵庫を配置したセルを建築可能地リストから削除
	cells.erase(cells.begin() + randomIndex);

	// ランダムにセルを選択
	randomIndex = rand() % cells.size();

	// 休憩所の生成と配置
	CBuildObject* pRefreshFacility = pScene->AddGameObject<CRefreshFacility>(Tag::GameObject, u8"休憩施設");
	pRefreshFacility->SetPos(cells[randomIndex]->GetPos());
	cells[randomIndex]->SetUse(true);
	cells[randomIndex]->SetObject(pRefreshFacility);
	pRefreshFacility->SetFieldCellIndex(cells[randomIndex]->GetIndex());

	// 休憩所を配置したセルを建築可能地リストから削除
	cells.erase(cells.begin() + randomIndex);

	// ランダムにセルを選択
	randomIndex = rand() % cells.size();

	// 人間の家の生成と配置
	CHumanHouse* pHumanHouse = pScene->AddGameObject<CHumanHouse>(Tag::GameObject, u8"人間の家");
	pHumanHouse->SetPos(cells[randomIndex]->GetPos());
	cells[randomIndex]->SetUse(true);
	cells[randomIndex]->SetObject(pHumanHouse);
	pHumanHouse->SetFieldCellIndex(cells[randomIndex]->GetIndex());

	// 人間の家を配置したセルを建築可能地リストから削除
	cells.erase(cells.begin() + randomIndex);
	
}

/*****************************************//*
	@brief　	| 縄張りの作成
*//*****************************************/
void CFieldManager::CreateTerritory()
{
	// 生成する縄張りの数
	std::vector<int> territoryNum;
	territoryNum.push_back(10); // オオカミ
	territoryNum.push_back(10); // シカ

	// フィールドセルの2次元配列を取得
	auto fieldCells = m_pFieldGrid->GetFieldCells();
	// 各縄張りタイプごとに縄張りを作成
	for (int t = 0; t < territoryNum.size(); ++t)
	{
		// 縄張りタイプの決定
		CFieldCell::TerritoryType territoryType = CFieldCell::TerritoryType::NONE;
		if (t == 0)
		{
			territoryType = CFieldCell::TerritoryType::Wolf;
		}
		else if (t == 1)
		{
			territoryType = CFieldCell::TerritoryType::Deer;
		}
		// 縄張りの数だけ繰り返す
		for (int n = 0; n < territoryNum[t]; ++n)
		{
			// ランダムにフィールドセルを選択
			int randX = rand() % CFieldGrid::GridSizeX;
			int randY = rand() % CFieldGrid::GridSizeY;

			if(fieldCells[randX][randY]->GetTerritoryType() != CFieldCell::TerritoryType::NONE)
			{
				// すでに縄張りが設定されている場合はスキップ
				--n;
				continue;
			}

			// 縄張りのサイズを0～1の範囲でランダムに決定
			int territorySizeX = rand() % 3 + 1; // 1～3の範囲でランダムに決定
			int territorySizeY = rand() % 3 * 1; // 1～3の範囲でランダムに決定


			// 縄張りの範囲内のフィールドセルを縄張りタイプに設定
			for (int x = -territorySizeX / 2; x <= territorySizeX / 2; ++x)
			{
				for (int y = -territorySizeY / 2; y <= territorySizeY / 2; ++y)
				{
					int cellX = randX + x;
					int cellY = randY + y;
					// フィールドセルの範囲内かチェック
					if (cellX >= 0 && cellX < CFieldGrid::GridSizeX &&
						cellY >= 0 && cellY < CFieldGrid::GridSizeY)
					{

						fieldCells[cellX][cellY]->SetTerritoryType(territoryType);
						fieldCells[cellX][cellY]->SetUse(false);
					}
				}
			}

		}
	}
}
