/**************************************************//*
	@file	| Deer_Animal.h
	@brief	| 鹿クラス定義
	@note	| 鹿の振る舞いを定義
			| CHerbivorousAnimalを継承
*//**************************************************/
#pragma once
#include "HerbivorousAnimal.h"
#include "SpatialGrid.h"

// @brief 鹿クラス
class CDeer_Animal final : public CHerbivorousAnimal
{
public:
	// @brief コンストラクタ
	CDeer_Animal();

	// @brief デストラクタ
	~CDeer_Animal() override;

	// @brief 初期化処理
	void Init() override;

	// @brief 更新処理
	void Update() override;

	// @brief 脅威の設定
	void SetThreat();

	// @brief 群れの登録
	// @param In_Deers：鹿リスト
	void RegisterToFlock(std::vector<CDeer_Animal*> In_Deers);

	// @brief 群れの情報(近隣の同種動物の位置・速度・脅威)を最新の状態に更新
	void RefreshFlock();

private:

	// @brief 近隣の同種動物の情報を作り直す
	// @param In_Deers：鹿リスト
	// @param fRadius：近隣として扱う距離
	void RefreshNeighbors(const std::vector<CDeer_Animal*>& In_Deers, float fRadius);

	// @brief 近隣の同種動物の情報を1頭分追加する
	// @param pDeer：近隣として追加する鹿
	void AppendNeighbor(CDeer_Animal* pDeer);

	// @brief 生成されている全ての鹿(群れ情報の更新用)
	inline static std::vector<CDeer_Animal*> s_AllDeer;

	// @brief 近隣検索用の空間分割(更新の1ステップに1回作り直す)
	inline static CSpatialGrid<CDeer_Animal> s_Grid;

	// @brief 空間分割を作った時の、更新のステップ数
	inline static unsigned int s_nGridStep = 0;

	// @brief 0..1の乱数取得
	float Rand01();

private:

	// @brief 警戒範囲
	float m_AlertRange = 15.0f;

	// @brief クリア範囲
	float m_ClearRange = 20.0f;

	// @brief 徘徊方向
	DirectX::XMFLOAT3 m_WanderDir{0.0f,0.0f,1.0f };

	// @brief 徘徊と待機のタイマー
	float m_fWanderUpdateTimer =0.0f;
	float m_fIdleTimer =0.0f;

	// @brief 次回更新時間
	float m_fNextWanderUpdate =1.0f;
	float m_fNextIdleChange =2.0f;

	// @brief 待機中かどうかのフラグ
	bool m_bIdle = false;

	// @brief 乱数シード
	unsigned int m_uRandSeed =0;

	// @brief 威嚇チェックのためのタイマー（SetThreatの呼び出しを間引く）
	float m_fThreatCheckTimer =0.0f;
	// @brief 威嚇チェック間隔（秒）。脅威がないときはこの間隔でチェックする。
	float m_fThreatCheckInterval =0.25f;

};