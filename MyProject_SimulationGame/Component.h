/**************************************************//*
	@file	| Component.h
	@brief	| コンポーネント基底クラス
*//**************************************************/
#pragma once
#include <string>

// 前方宣言
class CGameObject;

// @brief コンポーネント基底クラス
class CComponent
{
public:
	// @brief デフォルトコンストラクタ禁止
	CComponent() = delete;

	// @brief 引数付きコンストラクタ
	// @param inPtr：紐付けるゲームオブジェクトのポインタ
	CComponent(CGameObject* inPtr)
		: m_pGameObject(nullptr) 
	{
		// 紐付けるゲームオブジェクトのポインタを保存
		m_pGameObject = inPtr;
	}

	// @brief デストラクタ
	virtual ~CComponent() {}

	// @brief 初期化処理
	virtual void Init();

	// @brief 終了処理
	virtual void Uninit();

	// @brief 更新処理
	virtual void Update();

	// @brief 描画処理
	virtual void Draw();

	// @brief アクティブ設定
	// @param inActive：アクティブ設定
	void SetActive(bool inActive) { m_bActive = inActive; }

	// @brief アクティブ取得
	// @return アクティブ設定
	bool GetActive() { return m_bActive; }

	// @brief 紐付けているゲームオブジェクトの取得
	// @return 紐付けているゲームオブジェクトのポインタ
	CGameObject* GetGameObject() { return m_pGameObject; }

	// @brief 識別用タグの指定
	// @param inTag：識別用タグ
	void SetTag(const std::string& inTag) { m_sTag = inTag; }

	// @brief 識別用タグの取得
	// @return 識別用タグ
	const std::string GetTag() { return m_sTag; }

protected:
	// @brief 紐付けているゲームオブジェクトのポインタ
	CGameObject* m_pGameObject = nullptr;

	// @brief アクティブフラグ
	bool m_bActive = true; 

	// @brief 識別用タグ
	std::string m_sTag = "";
};
