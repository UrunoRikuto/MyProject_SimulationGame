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
#include "Human.h"

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
			// 岩の出現値(最小値：0.7f|最大値：1.0f)
			else if (noiseValue >= 0.7f && noiseValue <= 1.0f)
			{
				fieldCells[x][y]->SetCellType(CFieldCell::CellType::ROCK);
				objCount++;
			}
			else
			{
				fieldCells[x][y]->SetCellType(CFieldCell::CellType::EMPTY);
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

	// 貯蔵庫の生成と配置
	CGameObject* pStorageHouse = pScene->AddGameObject<CStorageHouse>(Tag::GameObject, "StorageHouse");
	DirectX::XMFLOAT3 f3StorageHousePos = fieldCells[halfSizeX][halfSizeY - 2]->GetPos();
	f3StorageHousePos.y += pStorageHouse->GetSize().y / 2.0f;
	pStorageHouse->SetPos(f3StorageHousePos);
	fieldCells[halfSizeX][halfSizeY]->SetUse(true);

	// 休憩所の生成と配置
	CGameObject* pRefreshFacility = pScene->AddGameObject<CRefreshFacility>(Tag::GameObject, "RefreshFacility");
	DirectX::XMFLOAT3 f3RefreshFacilityPos = fieldCells[halfSizeX + 2][halfSizeY]->GetPos();
	f3RefreshFacilityPos.y += pRefreshFacility->GetSize().y / 2.0f;
	pRefreshFacility->SetPos(f3RefreshFacilityPos);
	fieldCells[halfSizeX + 2][halfSizeY]->SetUse(true);

	// 初期村人の生成
	CGameObject* pHuman = pScene->AddGameObject<CHuman>(Tag::GameObject, "Human");
	DirectX::XMFLOAT3 f3HumanCreatePos = fieldCells[halfSizeX][halfSizeY]->GetPos();
	f3HumanCreatePos.y += pHuman->GetSize().y / 2.0f;
	pHuman->SetPos(f3HumanCreatePos);
}
