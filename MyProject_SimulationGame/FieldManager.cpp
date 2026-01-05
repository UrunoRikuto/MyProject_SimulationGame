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
#include "Camera.h"
#include "Oparation.h"

// 初期村のサイズ
const int INITIAL_VILLAGE_SIZE_X = 5;	// 初期村のXサイズ
const int INITIAL_VILLAGE_SIZE_Y = 5;	// 初期村のYサイズ

// 縄張り
const int TERRITORY_COUNT = 40;			// 縄張りの数

const int TERRITORY_MINSIZE_X = 2;		// 縄張りの最大Xサイズ
const int TERRITORY_MAXSIZE_X = 3;		// 縄張りの最大Xサイズ
const int TERRITORY_MINSIZE_Y = 1;		// 縄張りの最大Yサイズ
const int TERRITORY_MAXSIZE_Y = 3;		// 縄張りの最大Yサイズ


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
	// フィールドタイプの作成
	CreateFieldType();

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

	DirectX::XMINT2 CenterPos = { 0,0 };

	DirectX::XMFLOAT3 cameraPos = CCamera::GetInstance()->GetLook();

	// カメラ位置に最も近いフィールドセルを中心座標に設定
	float minDistance = FLT_MAX;
	for(auto cells : fieldCells)
	{
		for(auto cell : cells)
		{
			// フィールドセルの中心座標を取得
			DirectX::XMFLOAT3 cellPos = cell->GetPos();

			// カメラ位置とフィールドセルの距離を計算
			float distance = sqrtf(powf(cameraPos.x - cellPos.x, 2) + powf(cameraPos.z - cellPos.z, 2));
			if (distance < minDistance)
			{
				CenterPos.x = cell->GetIndex().x - CFieldGrid::GridSizeX / 2;
				CenterPos.y = cell->GetIndex().y - CFieldGrid::GridSizeY / 2;
				minDistance = distance;
			}
		}
	}


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
	@brief　	| フィールドタイプの作成
*//*****************************************/
void CFieldManager::CreateFieldType()
{
	float scale = 0.1f; // ノイズのスケール

	// シード値の初期化
	unsigned int seed = 0;

	CImguiSystem* pImGui = CImguiSystem::GetInstance();
	if (pImGui->IsSettingSeed())
	{
		// シード値を取得
		seed = pImGui->GetSeed();
	}
	else
	{
		// ランダムデバイスでシード値を生成
		std::random_device rd;
		// シード値を生成
		seed = rd();
		// シード値をImguiシステムに設定
		pImGui->SetSeed(seed);
	}
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

	// フィールドの中央付近に初期村の建築可能地を設定
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

	// 狼、鹿
	enum TerritoryType
	{
		Deer,
		Wolf,
		Max
	};

	// 残りの縄張り数
	int remainingTerritory = TERRITORY_COUNT / 2;
	// ランダムに分割
	for (int i = 0; i < TerritoryType::Max; ++i)
	{
		// 最後の要素の場合は残りをすべて割り当てる
		if (i == TerritoryType::Max - 1)
		{
			territoryNum.push_back(remainingTerritory);
			break;
		}

		// ランダムに縄張り数を決定
		int randNum = rand() % (remainingTerritory + 1);

		// 縄張り数を格納
		territoryNum.push_back(randNum);

		// 残りの縄張り数を更新
		remainingTerritory -= randNum;
	}

	// 半分は要素数で等分して割り当てる
	int halfTerritory = TERRITORY_COUNT / 2;
	int equalNum = halfTerritory / TerritoryType::Max;
	for (int i = 0; i < TerritoryType::Max; ++i)
	{
		territoryNum[i] += equalNum;
	}

	

	// フィールドセルの2次元配列を取得
	auto fieldCells = m_pFieldGrid->GetFieldCells();
	// 各縄張りタイプごとに縄張りを作成
	for (int t = 0; t < territoryNum.size(); ++t)
	{
		// 縄張りタイプの決定
		CFieldCell::TerritoryType territoryType = CFieldCell::TerritoryType::NONE;
		if (t == TerritoryType::Wolf)
		{
			territoryType = CFieldCell::TerritoryType::Wolf;
		}
		else if (t == TerritoryType::Deer)
		{
			territoryType = CFieldCell::TerritoryType::Deer;
		}
		// 縄張りの数だけ繰り返す
		for (int n = 0; n < territoryNum[t]; ++n)
		{
			// ランダムにフィールドセルを選択
			int randX = rand() % CFieldGrid::GridSizeX;
			int randY = rand() % CFieldGrid::GridSizeY;

			// 中心10x10の範囲はスキップ
			if (randX >= (CFieldGrid::GridSizeX / 2 - 5) && randX <= (CFieldGrid::GridSizeX / 2 + 5) &&
				randY >= (CFieldGrid::GridSizeY / 2 - 5) && randY <= (CFieldGrid::GridSizeY / 2 + 5))
			{
				--n;
				continue;
			}


			if(fieldCells[randX][randY]->GetTerritoryType() != CFieldCell::TerritoryType::NONE)
			{
				// すでに縄張りが設定されている場合はスキップ
				--n;
				continue;
			}

			// 縄張りのサイズを0～1の範囲でランダムに決定
			int territorySizeX = GetRandOfRange(TERRITORY_MINSIZE_X, TERRITORY_MAXSIZE_X);
			int territorySizeY = GetRandOfRange(TERRITORY_MINSIZE_Y, TERRITORY_MAXSIZE_Y);
			std::vector<std::vector<bool>> createflag;

			createflag.resize(territorySizeX);
			for (int i = 0; i < territorySizeX; ++i)
			{
				createflag[i].resize(territorySizeY);
				for (int j = 0; j < territorySizeY; ++j)
				{
					
					int randFlag = rand() % 10; // 0～9の範囲でランダムに決定
					if(5 > randFlag)
					{
						createflag[i][j] = true;
					}
					else
					{
						createflag[i][j] = false;
					}
				}
			}

			// 縄張りのサイズ分だけフィールドセルの縄張りタイプを設定
			for (int x = 0; x < territorySizeX; ++x)
			{
				for (int y = 0; y < territorySizeY; ++y)
				{
					// フィールドセルのインデックスを計算
					int cellX = randX + (x - territorySizeX / 2);
					int cellY = randY + (y - territorySizeY / 2);
					// フィールドセルの範囲外の場合はスキップ
					if (cellX < 0 || cellX >= CFieldGrid::GridSizeX || cellY < 0 || cellY >= CFieldGrid::GridSizeY)
					{
						continue;
					}
					if (createflag[x][y] == false)
					{
						continue;
					}
					// フィールドセルの縄張りタイプを設定
					fieldCells[cellX][cellY]->SetTerritoryType(territoryType);
				}
			}
		}
	}
}
