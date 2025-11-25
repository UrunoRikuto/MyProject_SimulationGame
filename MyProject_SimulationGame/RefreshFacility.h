/**************************************************//*
	@file	| RefreshFacility.h
	@brief	| 休憩施設クラスのhファイル
	@note	| 休憩施設の処理を定義
			| CBuildObjectを継承
*//**************************************************/
#pragma once
#include "BuildObject.h"
#include "Defines.h"
#include <vector>

class CHuman;

// @brief 休憩施設クラス
class CRefreshFacility : public CBuildObject
{
private:
	// @brief 休憩施設を使用できる最大人数配列
	static constexpr int MAX_USING_HUMANS[CBuildObject::MAX_BUILD_LEVEL] = {
		1, 2, 3, 4, 5
	};

public:
	// @brief スタミナ回復量(マイフレーム)
	static constexpr float REFRESH_STAMINA_AMOUNT[CBuildObject::MAX_BUILD_LEVEL] = { 
		1.0f , 1.5f , 2.0f , 2.5f , 3.0f
	};

	// 回復量の取得
	float GetRefreshStaminaAmount()
	{
		return REFRESH_STAMINA_AMOUNT[m_nBuildLevel - 1];
	}

public:
	// @brief コンストラクタ
	CRefreshFacility();

	// @brief デストラクタ
	virtual ~CRefreshFacility();

	// @brief 初期化処理
	virtual void Init() override;

	// @brief インスペクター表示処理
	virtual int Inspecter(bool isEnd = true) override;

	// @brief 休憩施設を使用可能かどうか
	// @return true:使用可能 false:使用不可
	bool CanUseRefreshFacility() const;

	// @brief 休憩所の使用
	void UseRefreshFacility(CHuman& pHuman);

	// @brief 自身が休憩所を使用しているかどうか
	bool IsUsingRefreshFacility(CHuman& pHuman) const;

	// @brief 休憩所の使用解除
	void ReleaseRefreshFacility(CHuman& pHuman);

public:
	// @brief 休憩施設を使用している人間の追加
	std::vector<CHuman*> m_pUsingHumans;

};

