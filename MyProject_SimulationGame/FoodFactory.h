/**************************************************//*
	@file	| FoodFactory.h
	@brief	| 食料加工施設クラスヘッダ
	@note	| 調理済みの食事を作成する建築物
			| CBuildObjectを継承
*//**************************************************/
#pragma once
#include "BuildObject.h"
#include "Item.h"
#include <list>
#include "Enums.h"

// @brief キッチン建築物クラス
class CFoodFactory final: public CBuildObject
{
private:
	// @brief 一度に調理できる最大依頼数
	static constexpr int MAX_COOK_REQUESTS[MAX_BUILD_LEVEL] = {
		1, 2, 3, 4, 5
	};

	// @brief 生産進行度(マイフレーム)
	static constexpr float COOK_PRODUCTION_PROGRESS_AMOUNT[CBuildObject::MAX_BUILD_LEVEL] = {
		1.0f , 1.5f , 2.0f , 2.5f , 3.0f
	};

	// @brief 生産進行度の取得
	float GetCookProductionProgressAmount()
	{
		return COOK_PRODUCTION_PROGRESS_AMOUNT[m_nBuildLevel - 1];
	}

	// @brief 建築経験値獲得量
	static constexpr float BUILD_XP_AMOUNT = 30.0f;
public:
	// @brief 調理依頼の構造体
	struct CookRequest
	{
		REQUEST_STATE eRequestState;		// 依頼状態
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

	// @brief 調理依頼を受ける
	// @return 調理依頼構造体のポインタ、なければnullptr
	CookRequest* TakeRequest();

	// @brief 指定の料理の調理依頼があるかどうかを取得
	// @param eMealType：料理のアイテムタイプ
	// @return true:存在する false:存在しない
	bool HasRequest(CItem::ITEM_TYPE eMealType) const;

	// @brief 調理依頼を未処理状態に設定
	// @param pRequest：ツール生産依頼構造体のポインタ
	void ResetRequest(CookRequest* pRequest);

	// @brief 調理依頼の完了報告
	// @param pRequest：ツール生産依頼構造体のポインタ
	// @return true:完了報告成功 false:完了報告失敗
	bool CompleteRequest(CookRequest* pRequest);

	// @brief 調理依頼を進める
	// @param In_Request：ツール生産依頼構造体のポインタ
	// @return 生産が完了した場合は生成したCItemポインタ、未完了の場合はnullptr
	std::vector<CItem*> ProgressRequest(CookRequest* pRequest);

	// @brief 調理依頼リストの取得
	// @return 調理依頼リスト
	std::list<CookRequest> GetCookRequestList() { return m_vCookRequestList; }

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
	std::list<CookRequest> m_vCookRequestList;

	// @brief 建築経験値
	float m_fBuildXP = 0.0f;

};

