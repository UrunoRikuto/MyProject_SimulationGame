/**************************************************//*
	@file	| GeneratorManager.h
	@brief	| 生成管理システムのhファイル
	@note	| オブザーバーパターンで作成
			| シングルトンパターンで作成
*//**************************************************/
#pragma once
#include "Singleton.h"
#include "SubJect.h"
#include "Generator.h"

// @brief 人間の再生成待ち時間（秒）
const float Human_Regenerate_Wait_Time = 5.0f;

// @brief 生成管理システムクラス
class CGeneratorManager final : public ISubJect, public ISingleton<CGeneratorManager>
{
public:
	// @brief 生成タイプ列挙型
	enum class GenerateType
	{
		Wood,	// 木材生成
		Stone,	// 石材生成
		Human,	// 人間生成
	};

	// @brief 生成リクエスト構造体
	struct GenerateRequest
	{
		// @brief 生成タイプ 
		GenerateType m_GenerateType;

		// @brief 生成するまでの時間（秒）
		float m_fGenerateTime;
	};

private:
	// @brief コンストラクタ
	CGeneratorManager() = default;

	friend class ISingleton<CGeneratorManager>;
public:
	// @brief デストラクタ
	~CGeneratorManager() override = default;

	// @brief 更新処理
	void Update();

	// @brief 全オブザーバーへの通知処理
	void NotifyObservers() override;

	// @brief 指定オブザーバーの通知処理
	// @tparam T 通知対象のオブザーバークラス型
	template<typename T = IGenerator>
	void NotifyObserver();

	// @brief 生成リクエストの追加処理
	// @param request：追加する生成リクエスト
	void AddGenerateRequest(GenerateRequest request);

	// @brief 生成リクエストリストの取得
	// @return 生成リクエストリストの参照
	std::list<GenerateRequest>& GetGenerateRequestList() { return m_GenerateRequestList; }

private:

	// @brief 生成リクエストリスト
	std::list<GenerateRequest> m_GenerateRequestList;
};

// Generator処理のインクルード
#include "WoodGenerator.h"
#include "StoneGenerator.h"
#include "GrassGenerator.h"
#include "HumanGenerator.h"
#include "AnimalGenerator.h"
