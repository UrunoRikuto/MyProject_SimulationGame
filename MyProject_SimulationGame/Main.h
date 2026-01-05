/**********************************************************************************//*
    @file		|Main.h
    @brief		|メイン処理
*//***********************************************************************************/
#pragma once
#include <Windows.h>
#include <string>
#include "Scene.h"

// @brief 初期化
// @param　hWnd：ウィンドウハンドル
// @param　width：画面幅
// @param　height：画面高さ
// @return 初期化に成功したかどうか
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
// @param inScene：変更先のシーンポインタ
void ChangeScene(CScene* inScene);

// @brief フェードイン
// @param onFadeComplete：フェードイン完了時に呼ばれる関数
void FadeIn(std::function<void()> onFadeComplete);

// @brief フェードアウト
// @param onFadeComplete：フェードアウト完了時に呼ばれる関数
void FadeOut(std::function<void()> onFadeComplete);

// @brief セットImgui有効フラグ
// @param bOnImgui：Imgui有効フラグ
void SetOnImgui(bool bOnImgui);

// @brief ウインドウハンドル取得
// @return ウインドウハンドル
HWND GetMyWindow();

// @brief フレームレート取得
// @return フレームレート
int GetFPS();

// @brief マウスホイールの回転量取得
// @return 回転量
int GetMouseWheelDelta();

// @brief リセットマウスホイールの回転量
void ResetMouseWheelDelta();
