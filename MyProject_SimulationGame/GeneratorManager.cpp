/**************************************************//*
	@file	| GeneratorManager.cpp
	@brief	| 生成管理システム実装ファイル
	@note	| オブザーバーパターンで作成
			| シングルトンパターンで作成
*//**************************************************/
#include "GeneratorManager.h"
#include "Main.h"

// 静的メンバ変数の初期化
CGeneratorManager* CGeneratorManager::m_pInstance = nullptr;

/*****************************************
	@brief　	| オブザーバーへの通知処理
*//*****************************************/
void CGeneratorManager::NotifyObservers()
{
	for (IObserver* observer : m_Observers)
	{
		observer->Notify();
	}
}

/*****************************************
	@brief　	| シングルトンインスタンスの取得
*//*****************************************/
CGeneratorManager* CGeneratorManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new CGeneratorManager();
	}
	return m_pInstance;
}

/*****************************************
	@brief　	| シングルトンインスタンスの解放
*//*****************************************/
void CGeneratorManager::ReleaseInstance()
{
	delete m_pInstance;
	m_pInstance = nullptr;
}

/*****************************************
	@brief　	| オブザーバーの通知処理
*//*****************************************/
template<typename T>
void CGeneratorManager::NotifyObserver()
{
	// 登録されている全オブザーバーに対して通知を行う
	for (IObserver* observer : m_Observers)
	{
		// ダウンキャストを試みる
		T* specificObserver = dynamic_cast<T*>(observer);

		// ダウンキャストに成功した場合のみ通知を行う
		if (specificObserver)
		{
			// 通知処理を呼び出す
			specificObserver->Notify();
		}
	}
}