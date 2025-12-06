/**************************************************//*
	@file	| FoodFactory.h
	@brief	| 食料加工施設クラスヘッダ
	@note	| 調理済みの食事を作成する建築物
			| CBuildObjectを継承
*//**************************************************/
#pragma once
#include "BuildObject.h"
#include "Item.h"
#include <vector>
#include <list>

// @brief キッチン建築物クラス
class CFoodFactory final: public CBuildObject
{
private:
	// @brief 一度に調理できる最大依頼数
	static constexpr int MAX_COOK_REQUESTS[MAX_BUILD_LEVEL] = {
		1, 2, 3, 4, 5
	};

public:
	// @brief 調理依頼の構造体
	struct CookRequest
	{
		CItem::ITEM_TYPE eMealType;			// 食事のアイテムタイプ
		float fCookingProgress;				// 調理進行度
	};

public:

	// @brief コンストラクタ
	CFoodFactory();

	// @brief デストラクタ
	virtual ~CFoodFactory();

	// @brief 初期化処理
	virtual void Init() override;

	// @brief 更新処理
	virtual void Update() override;

	// @brief インスペクター表示処理
	virtual int Inspecter() override;

	// @brief 調理依頼を進める
	// @param fAmount：進める量
	void ProgressCookRequests(float fAmount);

private:

	// @brief 調理依頼の追加
	// @param eMealType：食事のアイテムタイプ
	void AddCookRequest(CItem::ITEM_TYPE eMealType);

	// @brief 製作可能な料理を探索
	// @return 見つかった場合は料理のアイテムタイプ、見つからなかった場合はMAXを返す
	CItem::ITEM_TYPE FindProducibleMealType();

	// @brief 調理可能な料理リストの取得
	// @return 調理可能な料理のアイテムタイプリスト
	std::list<CItem::ITEM_TYPE> GetProducibleMealTypes();

private:

	// @brief 調理依頼リスト
	std::vector<CookRequest> m_vCookRequestList;

};

