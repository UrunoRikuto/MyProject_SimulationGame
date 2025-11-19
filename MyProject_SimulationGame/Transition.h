/**************************************************//*
	@file	| Transition.h
	@brief	| トランジションクラスのhファイル
	@note	| トランジション処理を行う
			| CGameObjectを継承
*//**************************************************/
#pragma once
#include "GameObject.h"
#include "SpriteRenderer.h"

// @brief トランジションクラス
class CTransition : public CGameObject
{
public:
	// @brief デストラクタ
    ~CTransition();

	// @brief 初期化処理
    void Init() override;

	// @brief 更新処理
    void Update() override;

	// @brief フェードイン開始
	// @param inFlameCount	フェードにかけるフレーム数
	// @param onFadeInComplete	フェードイン完了時に呼ばれる関数
    void FadeIn(int inFlameCount, std::function<void()> onFadeInComplete);

	// @brief フェードアウト開始
	// @param inFlameCount	フェードにかけるフレーム数
	// @param onFadeOutComplete	フェードアウト完了時に呼ばれる関数
    void FadeOut(int inFlameCount, std::function<void()> onFadeOutComplete);

public:
	// @brief フェードモード
    enum eMode
    {
        NONE,
        FADE_IN,
        FADE_OUT
    };

private:
	// @brief スプライトレンダラー
    CSpriteRenderer* m_pSprite;

	// @brief スプライトパラメータ
    int m_nCurrentCount;

	// @brief スプライトパラメータ
    int m_nEndCount;

	// @brief フェード完了時に呼ばれる関数
    std::function<void()> m_fFadeComplete;

	// @brief 現在のフェードモード
    eMode m_eCurrentMode;

};
