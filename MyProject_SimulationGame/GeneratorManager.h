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
#include <vector>

// @brief 人間の再生成待ち時間（秒）
const float Human_Regenerate_Wait_Time = 5.0f;

// @brief 環境オブジェクトの再生成待ち時間の範囲（秒）
// @note ゲーム内の1日は120秒。範囲内でランダムに決まるため、再生成のタイミングがばらける
// 木
const float Wood_Regenerate_Min_Time = 120.0f;
const float Wood_Regenerate_Max_Time = 240.0f;
// 石
const float Stone_Regenerate_Min_Time = 240.0f;
const float Stone_Regenerate_Max_Time = 480.0f;
// 草
const float Grass_Regenerate_Min_Time = 60.0f;
const float Grass_Regenerate_Max_Time = 180.0f;

// @brief 生成管理システムクラス
class CGeneratorManager final : public ISubJect, public ISingleton<CGeneratorManager>
{
public:
	// @brief 生成タイプ列挙型
	enum class GenerateType
	{
		Wood,	// 木材生成
		Stone,	// 石材生成
		Grass,	// 草生成
		Human,	// 人間生成
	};

	// @brief 生成リクエスト構造体
	struct GenerateRequest
	{
		// @brief 生成タイプ
		GenerateType m_GenerateType;

		// @brief 生成するまでの時間（秒）
		float m_fGenerateTime;

		// @brief 生成先のセルのインデックス
		// @note 木・石・草の再生成で使用する。指定しない場合は(-1, -1)
		DirectX::XMINT2 m_n2CellIndex = DirectX::XMINT2(-1, -1);
	};

private:
	// @brief コンストラクタ
	CGeneratorManager() = default;

	friend class ISingleton<CGeneratorManager>;
public:
	// @brief デストラクタ
	// @note 登録されているオブザーバー(ジェネレーター)を破棄する
	~CGeneratorManager() override;

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

	// @brief 環境オブジェクト(木・石・草)の再生成リクエストの追加処理
	// @param eType：再生成するタイプ(Wood/Stone/Grass)
	// @param n2Cell：再生成するセルのインデックス
	// @note 待ち時間はタイプごとの範囲内でランダムに決まる
	void RequestRegenerate(GenerateType eType, const DirectX::XMINT2& n2Cell);

	// @brief 初期生成する環境オブジェクト(木・石・草)の生成の予約処理
	// @param eType：生成するタイプ(Wood/Stone/Grass)
	// @param n2Cell：生成先のセルのインデックス
	// @note 一度に全て生成すると処理が止まってしまうため、予約しておいて毎フレーム少しずつ生成する
	//       フィールドの中心(初期の村)に近いものから順に生成する
	void ReserveInitialSpawn(GenerateType eType, const DirectX::XMINT2& n2Cell);

	// @brief 生成が予約されたまま、まだ生成されていないオブジェクトの数の取得
	int GetReservedSpawnCount() const { return static_cast<int>(m_ReservedSpawns.size()); }

	// @brief 待機中の生成リクエスト数の取得
	// @param eType：数えるタイプ
	// @return 待機中のリクエスト数
	int CountRequests(GenerateType eType) const;

	// @brief 生成リクエストリストの取得
	// @return 生成リクエストリストの参照
	std::list<GenerateRequest>& GetGenerateRequestList() { return m_GenerateRequestList; }

private:
	// @brief 生成の予約
	struct ReservedSpawn
	{
		GenerateType m_GenerateType;
		DirectX::XMINT2 m_n2CellIndex;
		// @brief フィールドの中心からの距離の二乗
		float m_fDistanceSq;
	};

	// @brief 予約されている生成の処理(制限時間内で、生成できる分だけ行う)
	void ProcessReservedSpawns();

	// @brief 指定ジェネレーターに、指定セルへの生成を行わせる処理
	// @tparam T 対象のジェネレータークラス型
	// @param n2Cell：生成先のセルのインデックス((-1, -1)の場合は全体の生成を行わせる)
	template<typename T>
	void GenerateAtCell(const DirectX::XMINT2& n2Cell);

	// @brief 生成リクエストリスト
	std::list<GenerateRequest> m_GenerateRequestList;

	// @brief 予約されている生成(フィールドの中心から遠い順。末尾が最も近い)
	std::vector<ReservedSpawn> m_ReservedSpawns;

	// @brief 予約を中心からの距離順に並べ終えているかどうか
	bool m_bReservedSorted = true;
};

// Generator処理のインクルード
#include "WoodGenerator.h"
#include "StoneGenerator.h"
#include "GrassGenerator.h"
#include "HumanGenerator.h"
#include "AnimalGenerator.h"
