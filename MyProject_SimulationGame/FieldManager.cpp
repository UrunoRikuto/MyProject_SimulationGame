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

#include "FarmFacility.h"

// 初期村のサイズ
const int INITIAL_VILLAGE_SIZE_X = 5;	// 初期村のXサイズ
const int INITIAL_VILLAGE_SIZE_Y = 5;	// 初期村のYサイズ

//-- 静的メンバ変数の初期化 --//
CFieldManager* CFieldManager::m_pInstance = nullptr;

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
	@brief　	| インスタンスの取得
	@return		| CFieldManager型のインスタンスのポインタ
	@note		| インスタンスが生成されていなければ生成し、インスタンスを返す
*//*****************************************/
CFieldManager* CFieldManager::GetInstance()
{
	// インスタンスが生成されていなければ生成する
	if (m_pInstance == nullptr)
	{
		m_pInstance = new(std::nothrow) CFieldManager();
	}
	// インスタンスを返す
	return m_pInstance;
}

/*****************************************//*
	@brief　	| インスタンスの解放
*//*****************************************/
void CFieldManager::ReleaseInstance()
{
	// インスタンスが生成されていれば解放する
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
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
	for(int x = 0; x < fieldCells.size(); ++x)
	{
		for(int y = 0; y < fieldCells[x].size(); ++y)
		{
			// パーリンノイズの値を取得
			float noiseValue = perlinNoise.noise(x * scale, y * scale);

			// 正規化 0.0f ～ 1.0f に変換
			noiseValue = (noiseValue + 1.0f) / 2.0f;

			// ノイズ値に基づいてセルタイプを決定
			// 最大値：1.0f、最小値：0.0f

			// 木の出現値(最小値：0.0f|最大値：0.4f)
			if (noiseValue >= 0.0f && noiseValue <= 0.4f)
			{
				fieldCells[x][y]->SetCellType(CFieldCell::CellType::TREE);
				objCount++;
			}
			// 草地の出現値(最小値：0.45f|最大値：0.5f)
			else if (noiseValue >= 0.45f && noiseValue <= 0.5f)
			{
				fieldCells[x][y]->SetCellType(CFieldCell::CellType::GRASS);
			}
			// 岩の出現値(最小値：0.7f|最大値：1.0f)
			else if (noiseValue >= 0.7f && noiseValue <= 1.0f)
			{
				fieldCells[x][y]->SetCellType(CFieldCell::CellType::ROCK);
			}
			// 空地の出現値(0.3f ～ 0.45f | 0.5f ～ 0.7f)
			else
			{
				fieldCells[x][y]->SetCellType(CFieldCell::CellType::EMPTY);
				objCount++;
			}
		}
	}

	// 初期村の配置
	CreateInitialVillage();

	// オブジェクトを生成
	for(int x = 0; x < fieldCells.size(); ++x)
	{
		for(int y = 0; y < fieldCells[x].size(); ++y)
		{
			CGeneratorManager::GetInstance()->NotifyObservers();
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
		}
	}

	// シーンの取得
	CScene* pScene = GetScene();

	// 建築可能なフィールドセルを取得
	auto cells = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells(CFieldCell::CellType::Build, false);

	// ランダムにセルを選択
	int randomIndex = rand() % cells.size();

	// 貯蔵庫の生成と配置
	CBuildObject* pStorageHouse = pScene->AddGameObject<CStorageHouse>(Tag::GameObject, "StorageHouse");
	pStorageHouse->SetPos(cells[randomIndex]->GetPos());
	cells[randomIndex]->SetUse(true);
	cells[randomIndex]->SetObject(pStorageHouse);
	pStorageHouse->SetFieldCellIndex(cells[randomIndex]->GetIndex());

	// 貯蔵庫を配置したセルを建築可能地リストから削除
	cells.erase(cells.begin() + randomIndex);

	// ランダムにセルを選択
	randomIndex = rand() % cells.size();

	// 休憩所の生成と配置
	CBuildObject* pRefreshFacility = pScene->AddGameObject<CRefreshFacility>(Tag::GameObject, "RefreshFacility");
	pRefreshFacility->SetPos(cells[randomIndex]->GetPos());
	cells[randomIndex]->SetUse(true);
	cells[randomIndex]->SetObject(pRefreshFacility);
	pRefreshFacility->SetFieldCellIndex(cells[randomIndex]->GetIndex());

	// 休憩所を配置したセルを建築可能地リストから削除
	cells.erase(cells.begin() + randomIndex);

	// ランダムにセルを選択
	randomIndex = rand() % cells.size();

	// 人間の家の生成と配置
	CHumanHouse* pHumanHouse = pScene->AddGameObject<CHumanHouse>(Tag::GameObject, "HumanHouse");
	pHumanHouse->SetPos(cells[randomIndex]->GetPos());
	cells[randomIndex]->SetUse(true);
	cells[randomIndex]->SetObject(pHumanHouse);
	pHumanHouse->SetFieldCellIndex(cells[randomIndex]->GetIndex());

	// 人間の家を配置したセルを建築可能地リストから削除
	cells.erase(cells.begin() + randomIndex);

	// ランダムにセルを選択
	randomIndex = rand() % cells.size();

	// 農場施設の生成と配置
	CBuildObject* pFarmFacility = pScene->AddGameObject<CFarmFacility>(Tag::GameObject, "FarmFacility");
	pFarmFacility->SetPos(cells[randomIndex]->GetPos());
	cells[randomIndex]->SetUse(true);
	cells[randomIndex]->SetObject(pFarmFacility);
	pFarmFacility->SetFieldCellIndex(cells[randomIndex]->GetIndex());
	
}
