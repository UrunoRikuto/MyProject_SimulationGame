/**************************************************//*
	@file	| BuildManager.h
	@brief	| 建築物関係の管理システムのhファイル
	@note	| 建築物関係の管理システムを定義
			| シングルトンパターンで作成
*//**************************************************/
#pragma once
#include <list>
#include "Item.h"
#include "BuildObject.h"

// @brief 建築物関係の管理システムクラス
class CBuildManager
{
public:
	// @brief 依頼状態の列挙型
	enum class RequestState
	{
		// 未処理
		Unprocessed,
		// 処理中
		InProcess,
	};

	// @brief 依頼タイプの列挙型
	enum class RequestType
	{
		// 建築
		Build,
		// 強化
		Upgrade,
		// 解体
		Demolition,
	};

	// @brief 建築物タイプの列挙型
	enum class BuildType
	{
		// 休憩所
		RefreshFacility,
		// 人間の家
		HumanHouse,

		MAX
	};

	// @brief 建築依頼構造体
	struct BuildRequest
	{
		// @brief コンストラクタ
		BuildRequest()
			: eRequestType(RequestType::Build)
			, eBuildType(BuildType::RefreshFacility)
			, eRequestState(RequestState::Unprocessed)
			, n2BuildIndex(DirectX::XMINT2(-1, -1))
		{
		}

		RequestType eRequestType;		// 依頼タイプ
		BuildType eBuildType;			// 建築物タイプ
		RequestState eRequestState;		// 依頼状態
		DirectX::XMINT2 n2BuildIndex;	// 建築位置
	};

	// @brief 建築素材構造体
	struct BuildMaterial
	{
		CItem::ITEM_TYPE eItemType;		// アイテムタイプ
		int nRequiredAmount;			// 必要数
	};

private:
	// @brief コンストラクタ
	CBuildManager();

public:
	// @brief デストラクタ
	~CBuildManager();

	// @brief インスタンスを取得
	static CBuildManager* GetInstance();

	// @brief インスタンスを解放
	static void ReleaseInstance();

	// @brief 建築依頼を追加
	// @param In_eRequestType：依頼タイプ
	void AddBuildRequest(const BuildType In_eRequestType);

	// @brief 建築依頼を受ける
	// @return 建築依頼構造体のポインタ、無ければnullptr
	BuildRequest* TakeBuildRequest();

	// @brief 建築依頼を未処理状態に設定
	// @param pRequest：建築依頼構造体のポインタ
	void ResetBuildRequest(BuildRequest* pRequest);

	// @brief 建築依頼を完了状態に設定
	void CompleteBuildRequest(BuildRequest* pRequest);

	// @brief 依頼が存在するかどうかを取得
	// @return true:存在する false:存在しない
	bool HasBuildRequest() const { return !m_BuildRequestList.empty(); }

	// @brief 建築依頼リストの取得
	const std::list<BuildRequest>& GetBuildRequestList() const { return m_BuildRequestList; }

private:

	// @brief ランダムに建築位置を決定
	DirectX::XMINT2 DecideRandomBuildPosition();

private:

	// @brief インスタンス
	static CBuildManager* m_pInstance;

	// 建築依頼リスト
	std::list<BuildRequest> m_BuildRequestList;
};

// @brief 建築素材データ名前空間
namespace BuildMaterials
{
	// @brief 休憩所に必要な素材
	const std::vector<CBuildManager::BuildMaterial> RefreshFacility[CBuildObject::MAX_BUILD_LEVEL] =
	{
		// Level 1
		{{ CItem::ITEM_TYPE::Wood,  10 },{ CItem::ITEM_TYPE::Stone,   10 }},
		// Level 2
		{{ CItem::ITEM_TYPE::Wood,  20 },{ CItem::ITEM_TYPE::Stone,   20 }},
		// Level 3
		{{ CItem::ITEM_TYPE::Wood,  30 },{ CItem::ITEM_TYPE::Stone,   30 }},
		// Level 4
		{{ CItem::ITEM_TYPE::Wood,  40 },{ CItem::ITEM_TYPE::Stone,   40 }},
		// Level 5
		{{ CItem::ITEM_TYPE::Wood,  50 },{ CItem::ITEM_TYPE::Stone,   50 }},
	};

	// @brief 人間の家に必要な素材
	const std::vector<CBuildManager::BuildMaterial> HumanHouse[CBuildObject::MAX_BUILD_LEVEL] =
	{
		// Level 1
		{{ CItem::ITEM_TYPE::Wood,  15 },{ CItem::ITEM_TYPE::Stone,   5 }},
		// Level 2
		{{ CItem::ITEM_TYPE::Wood,  30 },{ CItem::ITEM_TYPE::Stone,  10 }},
		// Level 3
		{{ CItem::ITEM_TYPE::Wood,  45 },{ CItem::ITEM_TYPE::Stone,  15 }},
		// Level 4
		{{ CItem::ITEM_TYPE::Wood,  60 },{ CItem::ITEM_TYPE::Stone,  20 }},
		// Level 5
		{{ CItem::ITEM_TYPE::Wood,  75 },{ CItem::ITEM_TYPE::Stone,  25 }},
	};

	// @brief 指定された建築タイプの建築素材リストを取得
	std::vector<CBuildManager::BuildMaterial> GetBuildMaterials(CBuildManager::BuildType eType, int nLevel);
}