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
	@brief　	| 更新処理
*//*****************************************/
void CGeneratorManager::Update()
{
	// 生成リクエストリストがない場合は処理を抜ける
	if (m_GenerateRequestList.empty())return;

	// 生成リクエストリストの先頭から処理を行う
	for (int i = 0; i < static_cast<int>(m_GenerateRequestList.size()); ++i)
	{
		// イテレータを取得
		auto it = std::next(m_GenerateRequestList.begin(), i);
		// 経過時間を加算
		it->m_fGenerateTime -= 1.0f / fFPS;

		// 生成時間に達した場合は生成処理を行う
		if (it->m_fGenerateTime <= 0.0f)
		{
			// 生成タイプに応じたオブザーバーへの通知処理
			switch (it->m_GenerateType)
			{
			case GenerateType::Wood:
				NotifyObserver<CWoodGenerator>();
				break;
			case GenerateType::Stone:
				NotifyObserver<CStoneGenerator>();
				break;
			case GenerateType::Human:
				NotifyObserver<CHumanGenerator>();
				break;
			}

			// 生成リクエストリストから削除
			it = m_GenerateRequestList.erase(it);
			// イテレータを1つ前に戻す
			--i;
		}
	}
}

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
	@brief　	| 生成リクエストの追加
	@param　	| request 生成リクエスト情報
*//*****************************************/
void CGeneratorManager::AddGenerateRequest(GenerateRequest request)
{
	// 生成リクエストリストに追加
	m_GenerateRequestList.push_back(request);
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