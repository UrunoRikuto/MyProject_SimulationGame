/**************************************************//*
	@file	| FarmFacility.h
	@brief	| 農場施設クラスヘッダ
	@note	| 農場施設クラスの宣言
			| CBuildObjectを継承
*//**************************************************/
#pragma once
#include "BuildObject.h"
#include "Item.h"
#include <list>

// @brief 農場施設クラス
class CFarmFacility : public CBuildObject
{
private:
	// @brief 同時に農作できる作物の最大数配列
	static constexpr int MAX_CROP_FARM[CBuildObject::MAX_BUILD_LEVEL] = {
		2, 4, 6, 8, 10
	};

	// @brief マイフレームの成長進行度配列
	static constexpr float CROP_GROWTH_PROGRESS_AMOUNT[CBuildObject::MAX_BUILD_LEVEL] = {
		0.5f , 1.0f , 1.5f , 2.0f , 2.5f
	};

	// @brief 成長進行度の取得
	float GetCropGrowthProgressAmount()
	{
		return CROP_GROWTH_PROGRESS_AMOUNT[m_nBuildLevel - 1];
	}

	// @brief 農作物が完成したときに得られる建築経験値
	static constexpr float BUILD_XP_AMOUNT = 10.0f;


public:

	// @brief 農作物構造体
	struct Crop
	{
		CItem::ITEM_TYPE eCropType;		// 農作物のアイテムタイプ
		float fGrowthProgress;			// 成長進行度
	};

public:

	// @brief コンストラクタ
	CFarmFacility();

	// @brief デストラクタ
	virtual ~CFarmFacility();

	// @brief 初期化処理
	virtual void Init() override;

	// @brief 更新処理
	virtual void Update() override;

	// @brief インスペクター表示処理
	virtual int Inspecter() override;

	// @brief 農作物を追加可能かどうかを取得
	// @return true:追加可能 false:追加不可
	bool CanAddCrop() const;

	// @brief 農作物を追加
	// @param eCropType：農作物のアイテムタイプ
	void AddCrop(CItem::ITEM_TYPE eCropType);

	// @brief 完成した農作物があるかどうかを取得
	// @return true:完成した農作物がある false:完成した農作物がない
	bool HasCompletedCrops() const;

	// @brief 完成した農作物を取得
	// @return 完成した農作物のCItemポインタ、無ければnullptr
	CItem* TakeCompletedCrop();

private:

	// @brief 完成した農作物を格納
	void StoreCompletedCrops();

	// @brief 農作物の成長を進める
	void ProgressCrops();

private:

	// @brief 農作物リスト
	std::list<Crop> m_pCropList;

	// @brief 完成した農作物リスト
	std::list<CItem*> m_pCompletedItemList;

	// @brief 建築経験値
	float m_fBuildXP = 0.0f;
};

