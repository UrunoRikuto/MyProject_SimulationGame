/**************************************************//*
	@file	| HumanHouse.h
	@brief	| 人間の家クラスのhファイル
	@note	| 人間の家の処理を定義
			| CBuildObjectを継承
*//**************************************************/
#pragma once
#include "BuildObject.h"
#include "Human.h"
#include <list>

// @brief 人間の家クラス
class CHumanHouse : public CBuildObject
{
private:

	// @brief 1軒あたりの最大居住人数配列
	static constexpr int MAX_RESIDENTS[CBuildObject::MAX_BUILD_LEVEL] = {
		1, 2, 3, 4, 5
	};

public:
	// @brief コンストラクタ
	CHumanHouse();

	// @brief デストラクタ
	virtual ~CHumanHouse();

	// @brief 初期化処理
	virtual void Init() override;

	// @brief 最大居住者数の取得
	// @return 最大居住者数
	int GetMaxResidents() const;

	// @brief 居住者の数
	int GetResidentCount() const { return static_cast<int>(m_Residents.size()); }

	// @brief 居住者の追加
	void AddResident(CHuman* pHuman);

	// @brief 居住者の削除
	void RemoveResident(CHuman* pHuman);

private:

	// @brief 居住者リスト
	std::list<CHuman*> m_Residents;
};

