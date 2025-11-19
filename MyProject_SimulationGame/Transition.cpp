/**************************************************//*
	@file	| Transition.cpp
	@brief	| トランジションクラスのcppファイル
	@note	| トランジション処理を行う
			| CGameObjectを継承
*//**************************************************/
#include "Transition.h"

/*****************************************//*
    @brief　	| デストラクタ
*//****************************************/
CTransition::~CTransition()
{
}

/*****************************************//*
    @brief　	| 初期化処理
*//****************************************/
void CTransition::Init()
{
	// スプライトレンダラー追加
    m_pSprite = AddComponent<CSpriteRenderer>();
	// スプライトのキーを設定
    m_pSprite->SetKey("Fade");

	// スプライトパラメータ設定
    m_tParam.m_f3Size = DirectX::XMFLOAT3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);

	// 初期色設定(白、透明)
    m_tParam.m_f4Color = DirectX::XMFLOAT4(1.0f,1.0f,1.0f,0.0f);

    m_nCurrentCount = 0;
    m_nEndCount = 0;
    m_fFadeComplete = nullptr;
    m_eCurrentMode = eMode::NONE;
}

/*****************************************//*
    @brief　	| 更新処理
*//****************************************/
void CTransition::Update()
{
    if (m_nCurrentCount >= m_nEndCount)
    {
        m_eCurrentMode = eMode::NONE;

        if (m_fFadeComplete)
        {
            m_fFadeComplete();
            m_fFadeComplete = nullptr;
        }
        return;
    }

    m_nCurrentCount++;
    float alpha = 0.0f;
    switch (m_eCurrentMode)
    {
    case CTransition::NONE:
        break;
    case CTransition::FADE_IN:
        alpha = 1.0f - (float)m_nCurrentCount / m_nEndCount;
        break;
    case CTransition::FADE_OUT:
        alpha = (float)m_nCurrentCount / m_nEndCount;
        break;
    default:
        break;
    }

    m_tParam.m_f4Color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, alpha);
}

/*****************************************//*
    @brief　	| フェードイン開始
    @param      | inFlameCount	フェードにかけるフレーム数
    @param      | onFadeInComplete	フェードイン完了時に呼ばれる関数
*//****************************************/
void CTransition::FadeIn(int inFlameCount, std::function<void()> onFadeInComplete)
{
    m_eCurrentMode = eMode::FADE_IN;
    m_nCurrentCount = 0;
    m_nEndCount = inFlameCount;
    m_fFadeComplete = onFadeInComplete;
}

/*****************************************//*
    @brief　	| フェードアウト開始
    @param      | inFlameCount	フェードにかけるフレーム数
    @param      | onFadeOutComplete	フェードアウト完了時に呼ばれる関数
*//****************************************/
void CTransition::FadeOut(int inFlameCount, std::function<void()> onFadeOutComplete)
{
    m_eCurrentMode = eMode::FADE_OUT;
    m_nCurrentCount = 0;
    m_nEndCount = inFlameCount;
    m_fFadeComplete = onFadeOutComplete;
}
