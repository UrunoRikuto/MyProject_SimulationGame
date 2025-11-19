/**************************************************//*
	@file	| ObjectLoad.h
	@brief	| オブジェクトロードクラスのhファイル
	@note	| オブジェクトのロード、アンロードを行う
*//**************************************************/
#pragma once

// @brief オブジェクトロードクラス
class CObjectLoad
{
public:
	// @brief 全てのオブジェクトをロード
    static void LoadAll();

	// @brief 全てのオブジェクトをアンロード
    static void UnLoadAll();

};
