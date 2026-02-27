/**************************************************//*
	@file	| FieldManager.cpp
	@brief	| フィールド管理クラスのcppファイル
	@note	| フィールド全体の管理を行う
			| シングルトンパターンで作成
*//**************************************************/
#include "FieldManager.h"
#include "FbmNoise.h"
#include "GeneratorManager.h"
#include <random>
#include "Main.h"
#include "StorageHouse.h"
#include "RefreshFacility.h"
#include "HumanHouse.h"
#include "ImguiSystem.h"
#include "Camera.h"
#include "Oparation.h"
#include <thread>
#include <vector>
#include <atomic>
#include <algorithm>
#include <cmath>

// 初期村のサイズ
const int INITIAL_VILLAGE_SIZE_X =5;	// 初期村のXサイズ
const int INITIAL_VILLAGE_SIZE_Y =5;	// 初期村のYサイズ

// 縄張り
const int TERRITORY_COUNT =100;

const int TERRITORY_MINSIZE_X =2;
const int TERRITORY_MINSIZE_Y =1;
const int TERRITORY_MAXSIZE_X =3;
const int TERRITORY_MAXSIZE_Y =3;


// フィールドデバック表示サイズ
constexpr int DEBUG_DRAW_SIZE =50; // DEBUG_DRAW_SIZE x DEBUG_DRAW_SIZE の範囲で表示

/*****************************************//*
	@brief 	 | コンストラクタ
 *//*****************************************/
CFieldManager::CFieldManager()
{
 	// フィールドグリッドの生成
 	m_pFieldGrid = new(std::nothrow) CFieldGrid({0.0f,0.0f,0.0f });
}

/*****************************************//*
	@brief 	 | デストラクタ
 *//*****************************************/
CFieldManager::~CFieldManager()
{
 	// フィールドグリッドの解放
 	delete m_pFieldGrid;
 	m_pFieldGrid = nullptr;
}

/*****************************************//*
	@brief 	 | フィールドセルのタイプ選出
 *//*****************************************/
void CFieldManager::AssignFieldCellType()
{
 	// フィールドタイプの作成
 	CreateFieldType();

 	// 縄張りの作成
 	CreateTerritory();

 	// 初期村の配置
 	CreateInitialVillage();

 	//生成通知
 	CGeneratorManager::GetInstance()->NotifyObservers();
}

/*****************************************//*
	@brief 	 | フィールドグリッドの表示
 *//*****************************************/
void CFieldManager::DebugDraw()
{
 	// フィールドセルの2次元配列を取得（コピーを避ける）
 	auto& fieldCells = m_pFieldGrid->GetFieldCells();

 	int halfSizeX = (CFieldGrid::GridSizeX - DEBUG_DRAW_SIZE) /2;
 	int halfSizeY = (CFieldGrid::GridSizeY - DEBUG_DRAW_SIZE) /2;

 	DirectX::XMINT2 CenterPos = {0,0 };

 	DirectX::XMFLOAT3 cameraPos = CCamera::GetInstance()->GetLook();

 	// カメラ位置に最も近いフィールドセルを中心座標に設定
 	float minDistanceSq = FLT_MAX; // 平方距離で比較して sqrt を避ける
 	for(const auto& col : fieldCells)
 	{
 		for(const auto& cell : col)
 		{
 			// フィールドセルの中心座標を取得
 			DirectX::XMFLOAT3 cellPos = cell->GetPos();

 			// カメラ位置とフィールドセルの距離の平方を計算
 			float dx = cameraPos.x - cellPos.x;
 			float dz = cameraPos.z - cellPos.z;
 			float distSq = dx * dx + dz * dz;
 			if (distSq < minDistanceSq)
 			{
 				CenterPos.x = cell->GetIndex().x - CFieldGrid::GridSizeX /2;
 				CenterPos.y = cell->GetIndex().y - CFieldGrid::GridSizeY /2;
 				minDistanceSq = distSq;
 			}
 		}
 	}

 	//ループ範囲を安全にクリップして不要な境界チェックを減らす
	int startX = (0 > (halfSizeX + CenterPos.x)) ?0 : (halfSizeX + CenterPos.x);
	int startY = (0 > (halfSizeY + CenterPos.y)) ?0 : (halfSizeY + CenterPos.y);
	int maxX = static_cast<int>(fieldCells.size());
	int maxY = (maxX >0) ? static_cast<int>(fieldCells[0].size()) :0;
	int endX = (maxX < startX + DEBUG_DRAW_SIZE) ? maxX : startX + DEBUG_DRAW_SIZE;
	int endY = (maxY < startY + DEBUG_DRAW_SIZE) ? maxY : startY + DEBUG_DRAW_SIZE;

	// 各フィールドセルのデバック描画を実行
	for(int x = startX; x < endX; ++x)
	{
		for(int y = startY; y < endY; ++y)
		{
			fieldCells[x][y]->DebugDraw(CImguiSystem::GetInstance()->GetFieldCellDisplayMode());
		}
	}
}

/*****************************************//*
	@brief 	 | フィールドタイプの作成
 *//*****************************************/
void CFieldManager::CreateFieldType()
{
	float scale =0.1f; // ノイズのスケール

	// シード値の初期化
	unsigned int seed =0;

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

	// FBMノイズパラメータ
	FbmNoise::Params fbmParams;
	fbmParams.octaves =10;
	fbmParams.lacunarity =2.0f;
	fbmParams.gain =0.1f;
	fbmParams.frequency =1.0f;
	fbmParams.amplitude =1.0f;
	fbmParams.normalize = true;

	// フィールドセルの2次元配列を参照で取得（コピーを避ける）
	auto& fieldCells = m_pFieldGrid->GetFieldCells();

	const int sizeX = static_cast<int>(fieldCells.size());
	const int sizeY = (sizeX >0) ? static_cast<int>(fieldCells[0].size()) :0;
	if (sizeX ==0 || sizeY ==0) return;

	// 並列でノイズ計算とセルタイプ割り当てを一度に行う
	unsigned int hwThreads = std::thread::hardware_concurrency();
	if (hwThreads ==0) hwThreads =2;
	const unsigned int numThreads = std::min<unsigned int>(hwThreads, static_cast<unsigned int>(sizeX));

	std::vector<std::thread> threads;
	threads.reserve(numThreads);

	std::atomic<int> objCount{0 };

	for (unsigned int t =0; t < numThreads; ++t)
	{
		int startX = static_cast<int>((static_cast<float>(t) / numThreads) * sizeX);
		int endX = static_cast<int>((static_cast<float>(t +1) / numThreads) * sizeX);

		threads.emplace_back([startX, endX, sizeY, scale, seed, fbmParams, &fieldCells, &objCount]() {
			// 各スレッドは同じシードで独立したFbmNoiseインスタンスを持つ（出力の一貫性を保つ）
			FbmNoise fbm_local(seed);
			for (int x = startX; x < endX; ++x)
			{
				auto& col = fieldCells[x];
				for (int y =0; y < sizeY; ++y)
				{
					float noiseValue = fbm_local.noise(static_cast<float>(x) * scale, static_cast<float>(y) * scale, fbmParams);
					noiseValue = (noiseValue +1.0f) /2.0f;

					if (noiseValue >=0.0f && noiseValue <=0.4f)
					{
						col[y]->SetCellType(CFieldCell::CellType::TREE);
						objCount.fetch_add(1, std::memory_order_relaxed);
					}
					else if (noiseValue >=0.45f && noiseValue <=0.5f)
					{
						col[y]->SetCellType(CFieldCell::CellType::GRASS);
					}
					else if (noiseValue >=0.7f && noiseValue <=0.9f)
					{
						col[y]->SetCellType(CFieldCell::CellType::ROCK);
					}
					else
					{
						col[y]->SetCellType(CFieldCell::CellType::EMPTY);
						objCount.fetch_add(1, std::memory_order_relaxed);
					}
				}
			}
		});
	}

	for (auto& th : threads) if (th.joinable()) th.join();

	// objCount は必要ならログ出力や統計に利用可能
}

/*****************************************//*
	@brief 	 | 初期村の作成
	@note 	 | フィールドの中央付近に初期村を作成
*//*****************************************/
void CFieldManager::CreateInitialVillage()
{
	// ハーフサイズを取得
	int halfSizeX = CFieldGrid::GridSizeX /2;
	int halfSizeY = CFieldGrid::GridSizeY /2;

	// フィールドセルの2次元配列を取得（コピーを避ける）
	auto& fieldCells = m_pFieldGrid->GetFieldCells();

	// フィールドの中央付近に初期村の建築可能地を設定
	for(int i = -INITIAL_VILLAGE_SIZE_X /2; i <= INITIAL_VILLAGE_SIZE_X /2; ++i)
	{
		for(int j = -INITIAL_VILLAGE_SIZE_Y /2; j <= INITIAL_VILLAGE_SIZE_Y /2; ++j)
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

	// ヘルパー関数を使用して建造物を生成して配置
	CreateAndPlaceBuilding(cells, [&](CScene* pScene) -> CBuildObject*
		{
			return pScene->AddGameObject<CStorageHouse>(Tag::GameObject, u8"貯蔵庫");
		});

	CreateAndPlaceBuilding(cells, [&](CScene* pScene) -> CBuildObject*
		{
			return pScene->AddGameObject<CRefreshFacility>(Tag::GameObject, u8"休憩施設");
		});

	CreateAndPlaceBuilding(cells, [&](CScene* pScene) -> CBuildObject*
		{
			return pScene->AddGameObject<CHumanHouse>(Tag::GameObject, u8"人間の家");
		});
}

/*****************************************//*
	@brief 	 | 縄張りの作成
*//*****************************************/
void CFieldManager::CreateTerritory()
{
	//生成する縄張りの数
	std::vector<int> territoryNum;

	// 狼、鹿
	enum TerritoryType
	{
		Deer,
		Wolf,
		Max
	};

	// 残りの縄張り数
	int remainingTerritory = TERRITORY_COUNT /2;
	// ランダムに分割
	for (int i =0; i < TerritoryType::Max; ++i)
	{
		// 最後の要素の場合は残りをすべて割り当てる
		if (i == TerritoryType::Max -1)
		{
			territoryNum.push_back(remainingTerritory);
			break;
		}

		// ランダムに縄張り数を決定
		int randNum = rand() % (remainingTerritory +1);

		// 縄張り数を格納
		territoryNum.push_back(randNum);

		// 残りの縄張り数を更新
		remainingTerritory -= randNum;
	}

	// 半分は要素数で等分して割り当てる
	int halfTerritory = TERRITORY_COUNT /2;
	int equalNum = halfTerritory / TerritoryType::Max;
	for (int i =0; i < TerritoryType::Max; ++i)
	{
		territoryNum[i] += equalNum;
	}

	// フィールドセルの2次元配列を取得（コピーを避ける）
	auto& fieldCells = m_pFieldGrid->GetFieldCells();
	// 各縄張りタイプごとに縄張りを作成
	for (int t =0; t < territoryNum.size(); ++t)
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
		for (int n =0; n < territoryNum[t]; ++n)
		{
			// ランダムにフィールドセルを選択
			int randX = rand() % CFieldGrid::GridSizeX;
			int randY = rand() % CFieldGrid::GridSizeY;

			// 中心10x10の範囲はスキップ
			if (randX >= (CFieldGrid::GridSizeX /2 -5) && randX <= (CFieldGrid::GridSizeX /2 +5) &&
				randY >= (CFieldGrid::GridSizeY /2 -5) && randY <= (CFieldGrid::GridSizeY /2 +5))
			{
				--n;
				continue;
			}


			if(fieldCells[randX][randY]->GetTerritoryType() != CFieldCell::TerritoryType::NONE)
			{
				//すでに縄張りが設定されている場合はスキップ
				--n;
				continue;
			}

			// 縄張りのサイズを0～1の範囲でランダムに決定
			int territorySizeX = GetRandOfRange(TERRITORY_MINSIZE_X, TERRITORY_MAXSIZE_X);
			int territorySizeY = GetRandOfRange(TERRITORY_MINSIZE_Y, TERRITORY_MAXSIZE_Y);

			// createflag の一時配列を使わずに直接割り当てを行う（メモリアロケーション削減）
			for (int x =0; x < territorySizeX; ++x)
			{
				for (int y =0; y < territorySizeY; ++y)
				{
					int cellX = randX + (x - territorySizeX /2);
					int cellY = randY + (y - territorySizeY /2);
					// フィールドセルの範囲外の場合はスキップ
					if (cellX <0 || cellX >= CFieldGrid::GridSizeX || cellY <0 || cellY >= CFieldGrid::GridSizeY)
					{
						continue;
					}
					int randFlag = rand() %10; //0～9の範囲でランダムに決定
					if(randFlag >=5) continue;
					// フィールドセルの縄張りタイプを設定
					fieldCells[cellX][cellY]->SetTerritoryType(territoryType);
				}
			}
		}
	}
}

/*****************************************//*
	@brief 		| 建造物を生成して配置するヘルパー
	@param		| cells 建築可能セルのリスト（配置後は該当セルを削除）
	@param		| factory シーンを渡して生成するオブジェクトのファクトリ（CBuildObject*を返す）
	@return		|生成して配置した建造物のポインタ（失敗した場合はnullptr）
*//*****************************************/
CBuildObject* CFieldManager::CreateAndPlaceBuilding(std::vector<CFieldCell*>& cells, std::function<CBuildObject* (CScene*)> factory)
{
	if (cells.empty()) return nullptr;

	CScene* pScene = GetScene();

	// ランダムにセルを選択
	int randomIndex = rand() % cells.size();

	// オブジェクト生成
	CBuildObject* pObj = factory(pScene);
	if (!pObj) return nullptr;

	// 配置
	pObj->SetPos(cells[randomIndex]->GetPos());
	cells[randomIndex]->SetUse(true);
	cells[randomIndex]->SetObject(pObj);
	pObj->SetFieldCellIndex(cells[randomIndex]->GetIndex());

	// 削除
	cells.erase(cells.begin() + randomIndex);

	return pObj;
}
