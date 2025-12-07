/**************************************************//*
	@file	| Singleton.h
	@brief	| シングルトンパターンのインターフェースクラス
	@note	| シングルトンパターンの機能を提供するインターフェースクラス
*//**************************************************/
#pragma once
#include <new>

// @brief シングルトンパターンのインターフェースクラス
// @tparam T シングルトンとして扱うクラス
template <typename T>
class ISingleton 
{
public:
	// @brief シングルトンインスタンスの取得
	// @return シングルトンインスタンスの参照
	static T* GetInstance() 
	{
		// インスタンスが未生成なら生成する
		if (m_pInstance == nullptr) 
		{
			m_pInstance = new(std::nothrow) T();
		}

		// インスタンスを返す
		return m_pInstance;
	}

	// @brief シングルトンインスタンスの破棄
	static void ReleaseInstance()
	{
		// インスタンスが生成されていれば破棄する
		if (m_pInstance != nullptr) 
		{
			// インスタンスの破棄
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
protected:
	// シングルトンインスタンスのポインタ
	static T* m_pInstance;
};

// 静的メンバ変数の定義
template <typename T>
T* ISingleton<T>::m_pInstance = nullptr;