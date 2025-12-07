/**************************************************//*
	@file	| BuildManager.h
	@brief	| 建築物関係の管理システムのhファイル
	@note	| 建築物関係の管理システムを定義
			| シングルトンパターンで作成
*//**************************************************/
#pragma once
#include "Singleton.h"
#include <list>
#include "Item.h"
#include "BuildObject.h"
#include "Enums.h"
#include "Main.h"

// @brief 建築物関係の管理システムクラス
class CBuildManager : public ISingleton<CBuildManager>
{
public:

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
	// @brief 依頼タイプを文字列に変換する関数
	static constexpr const char* REQUEST_TYPE_TO_STRING(RequestType eType)
	{
		switch (eType)
		{
		case RequestType::Build:		return "Build";
		case RequestType::Upgrade:		return "Upgrade";
		case RequestType::Demolition:	return "Demolition";
		default:						return "Unknown";
		}
	}

	// @brief 建築物タイプの列挙型
	enum class BuildType
	{
		// 休憩所
		RefreshFacility,
		// 人間の家
		HumanHouse,
		// 鍛冶屋
		BlackSmith,
		// 食品加工施設
		FoodFactory,
		// 農作施設
		FarmFacility,

		MAX
	};
	// @brief 建築物タイプを文字列に変換する関数
	static constexpr const char* BUILD_TYPE_TO_STRING(BuildType eType)
	{
		switch (eType)
		{
		case BuildType::RefreshFacility:	return "RefreshFacility";
		case BuildType::HumanHouse:		return "HumanHouse";
		case BuildType::BlackSmith:		return "BlackSmith";
		case BuildType::FoodFactory:	return "FoodFactory";
		case BuildType::FarmFacility:	return "FarmFacility";
		default:						return "Unknown";
		}
	}
	// @brief 建築タイプに応じた建築物クラスのポインタを生成する関数
	static CBuildObject* CreateBuildObjectByType(BuildType eType);


	// @brief 建築依頼構造体
	struct BuildRequest
	{
		// @brief コンストラクタ
		BuildRequest()
			: eRequestType(RequestType::Build)
			, eBuildType(BuildType::RefreshFacility)
			, eRequestState(REQUEST_STATE::Unprocessed)
			, n2BuildIndex(DirectX::XMINT2(-1, -1))
		{
		}

		RequestType eRequestType;		// 依頼タイプ
		BuildType eBuildType;			// 建築物タイプ
		REQUEST_STATE eRequestState;		// 依頼状態
		DirectX::XMINT2 n2BuildIndex;	// 建築位置
	};

	// @brief クールタイム時間
	static constexpr float COOL_TIME_DURATION = 10.0f;

private:
	// @brief コンストラクタ
	CBuildManager();

	friend class ISingleton<CBuildManager>;

public:
	// @brief デストラクタ
	~CBuildManager();

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

	// @brief クールタイム処理
	void CoolTimeUpdate();

private:

	// @brief 建築依頼がクールタイム中かどうかを取得
	// @return true:クールタイム中 false:クールタイム終了
	bool IsCoolTime() const { return m_fCoolTime > 0.0f; }

	// @brief ランダムに建築位置を決定
	DirectX::XMINT2 DecideRandomBuildPosition();

private:

	// 建築依頼リスト
	std::list<BuildRequest> m_BuildRequestList;

	// @brief クールタイム
	// @note 次の依頼を受けるまでの待機時間
	float m_fCoolTime = 0.0f;

};
