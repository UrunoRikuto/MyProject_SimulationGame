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
			// 岩の出現値(最小値：0.5f|最大値：0.7f)
			else if (noiseValue >= 0.7f && noiseValue <= 1.0f)
			{
				fieldCells[x][y]->SetCellType(CFieldCell::CellType::ROCK);
				objCount++;
			}
			else
			{
				fieldCells[x][y]->SetCellType(CFieldCell::CellType::EMPTY);
			}

			// オブザーバーへ通知
			//if (objCount < 500)
			{
				CGeneratorManager::GetInstance()->NotifyObservers();
			}
		}
	}
}
