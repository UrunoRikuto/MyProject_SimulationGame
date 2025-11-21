/**************************************************//*
	@file	| FieldManager.cpp
	@brief	| フィールド管理クラスのcppファイル
	@note	| フィールド全体の管理を行う
			| シングルトンパターンで作成
*//**************************************************/
#include "FieldManager.h"

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
	// フィールドセルの2次元配列を取得
	auto fieldCells = m_pFieldGrid->GetFieldCells();

	// 各フィールドセルに対してタイプをランダムに割り当てる
	for (auto& row : fieldCells)
	{
		// 各行のセルに対して処理
		for (auto& cell : row)
		{
			// ランダムにセルタイプを選出
			int randType = rand() % static_cast<int>(CFieldCell::CellType::MAX);
			cell->SetCellType(static_cast<CFieldCell::CellType>(randType));
		}
	}
}
