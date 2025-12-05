/**************************************************//*
	@file	| BlackSmith.h
	@brief	| 鍛冶屋クラスのhファイル
	@note	| 鍛冶屋オブジェクトの処理を定義
			| CBuildObjectを継承
*//**************************************************/
#pragma once
#include "BuildObject.h"
#include "Item.h"

// @brief 鍛冶屋クラス
class CBlackSmith : public CBuildObject
{
private:
	// @brief 同時に保持できるツール生産依頼の最大数配列
	static constexpr int MAX_REQUEST_TOOL[CBuildObject::MAX_BUILD_LEVEL] = {
		1, 2, 3, 4, 5
	};

public:

	// @brief 生産進行度(マイフレーム)
	static constexpr float TOOL_PRODUCTION_PROGRESS_AMOUNT[CBuildObject::MAX_BUILD_LEVEL] = {
		1.0f , 1.5f , 2.0f , 2.5f , 3.0f
	};

	// @brief 生産進行度の取得
	float GetToolProductionProgressAmount()
	{
		return TOOL_PRODUCTION_PROGRESS_AMOUNT[m_nBuildLevel - 1];
	}

	enum class TOOL_REQUEST_STATE
	{
		// 未処理
		Unprocessed,
		// 処理中
		InProcess,
	};

	// @brief ツール生産依頼構造体
	struct ToolRequest
	{
		TOOL_REQUEST_STATE eRequestState;	// 依頼状態
		CItem::ITEM_TYPE eToolType;		// ツールのアイテムタイプ
		float fProductionProgress;		// 生産進行度
	};

public:
	// @brief コンストラクタ
	CBlackSmith();

	// @brief デストラクタ
	virtual ~CBlackSmith();

	// @brief 初期化処理
	virtual void Init() override;

	// @brief インスペクター表示処理
	// @return 表示した項目数
	virtual int Inspecter() override;

	// @brief 生産依頼を追加可能かどうかを取得
	// @return true:追加可能 false:追加不可
	bool CanAddRequestTool() const;

	// @brief 指定の道具の生産依頼があるかどうかを取得
	// @param eToolType：道具のアイテムタイプ
	// @return true:依頼がある false:依頼がない
	bool HasRequestTool(CItem::ITEM_TYPE eToolType) const;

	// @brief 生産依頼の追加
	// @param eToolType：依頼する道具のアイテムタイプ
	void AddRequestTool(CItem::ITEM_TYPE eToolType);

	// @brief 生産依頼を受ける
	// @return ツール生産依頼構造体のポインタ、無ければnullptr
	ToolRequest* TakeRequestTool();

	// @brief 生産依頼を未処理状態に設定
	// @param pRequest：ツール生産依頼構造体のポインタ
	void ResetRequestTool(ToolRequest* pRequest);

	// @brief 生産依頼を進める
	// @param In_Request：ツール生産依頼構造体のポインタ
	// @return 生産が完了した場合は生成したCItemポインタ、未完了の場合はnullptr
	CItem* ProgressRequestTool(ToolRequest* In_Request);

private:
	// @brief 生産依頼アイテムリスト
	std::list<ToolRequest> m_vRequestToolList;
};

