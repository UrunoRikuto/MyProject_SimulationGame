/**********************************************************************************//*
    @file		|Main.h
    @brief		|メイン処理
*//***********************************************************************************/
#pragma once

#include <Windows.h>
#include <string>
#include "Scene.h"

// @brief 初期化
// @param[in]	hWnd	ウィンドウハンドル
// @param[in] 	width 画面幅
// @param[in] 	height 画面高さ
HRESULT Init(HWND hWnd, UINT width, UINT height);

// @brief 終了
void Uninit();

// @brief 更新
void Update();

// @brief 描画
void Draw();

// @brief アプリケーション終了処理
void AppEnd();

// @brief 現在のシーン取得
// @return 現在のシーンポインタ
CScene* GetScene();

// @brief シーン変更
// @param[in] inScene	変更先のシーンポインタ
void ChangeScene(CScene* inScene);

// @brief フェードイン
// @param[in] onFadeComplete	フェードイン完了時に呼ばれる関数
void FadeIn(std::function<void()> onFadeComplete);

// @brief フェードアウト
// @param[in] onFadeComplete	フェードアウト完了時に呼ばれる関数
void FadeOut(std::function<void()> onFadeComplete);


// @brief ウインドウハンドル取得
HWND GetMyWindow();

// @brief フレームレート取得
int GetFPS();

// @brief デバッグモードかどうか取得
bool IsDebugMode();
