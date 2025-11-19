/**********************************************************************************//*
	@file		| Defines.h
	@brief		| 定義ファイル
*//***********************************************************************************/
#pragma once
#include <assert.h>
#include <Windows.h>
#include <stdarg.h>
#include <stdio.h>
#include <DirectXMath.h>

// @brief フレームレート
static const int FPS = 60;
static const float fFPS = static_cast<float>(FPS);

// @brief アセットファイルパス
#define ASSET_PATH(path) ("Assets/" path)
// @brief モデルファイルパス
#define MODEL_PATH(path) (ASSET_PATH("Model/" path))
// @brief テクスチャファイルパス
#define TEXTURE_PATH(path) (ASSET_PATH("Texture/" path))
// @brief シェーダーファイルパス
#define SHADER_PATH(path) (ASSET_PATH("Shader/" path))

// 3D空間定義
#define CMETER(value) (value * 0.01f)
#define METER(value) (value * 1.0f)
#define MSEC(value) (value / fFPS)
#define CMSEC(value) MSEC(CMETER(value))

// @brief 円周率
#define PI (3.1415f)

// @brief 角度とラジアンの変換
#define TORAD(deg) ((deg / 180) * PI)
#define TODEG(rad) ((rad / PI) * 180)

// @brief 重力
static const float GRAVITY = 0.28f;

// @brief アプリケーションのタイトル
static const char* APP_TITLE = "3DGame";

// 画面サイズ
static const int SCREEN_WIDTH	= 1280;
static const int SCREEN_HEIGHT	= 720;

//メモリ開放
#define SAFE_DELETE(p)			do{if(p){delete p; p = nullptr;}}while(0)
#define SAFE_DELETE_ARRAY(p)	do{if(p){delete[] p; p = nullptr;}}while(0)
#define SAFE_RELEASE(p)			do{if(p){p->Release(); p = nullptr;}}while(0)
#define SAFE_NULLPTR(p)			do{if(p){p = nullptr;}}while(0)
