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

// @brief 自動計測モードの設定(起動引数 --profile=秒数 で指定される)
// @param fSeconds：計測する秒数(0以下の場合は通常の起動)
// @param nGameSpeed：計測中のゲーム速度(倍速)
// @param nExtraHumans：計測用に追加する人間の数
// @param pJobKey：全員に割り当てる職業の指定(nullptrの場合は全ての職業を順番に割り当てる)
// @param nStartDay：ゲーム内の開始日数(0未満の場合は最初から)。季節の見た目の確認などに使用する
void SetAutoProfile(float fSeconds, int nGameSpeed, int nExtraHumans, const char* pJobKey, int nStartDay);

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
