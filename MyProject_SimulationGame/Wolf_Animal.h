/**************************************************//*
	@file	| Wolf_Animal.h
	@brief	| 狼クラス定義
	@note	| 狼の振る舞いを定義
			| CCarnivorousAnimalを継承
*//**************************************************/
#pragma once
#include "CarnivorousAnimal.h"
#include "SpatialGrid.h"

// 前方宣言
class CHerbivorousAnimal;


// 狼クラス
class CWolf_Animal final: public CCarnivorousAnimal
{
public:
	// @brief コンストラクタ
	CWolf_Animal();

	// @brief デストラクタ
	~CWolf_Animal()override;

	// @brief 初期化処理
	void Init() override;

	// @brief 更新処理
	void Update() override;

	// @brief 標的の設定
	// @return 標的に設定した草食動物(標的を設定しなかった場合はnullptr)
	// @note 戻り値のポインタは、この関数を呼んだ同じフレーム内でのみ有効
	CHerbivorousAnimal* SetTarget();

	// @brief 群れの登録
	// @param In_Wolfs：オオカミリスト
	void RegisterToFlock(std::vector<CWolf_Animal*> In_Wolfs);

	// @brief 群れの情報(近隣の同種動物の位置・速度・標的)を最新の状態に更新
	void RefreshFlock();

private:
	// @brief 近隣の同種動物の情報を作り直す
	// @param In_Wolfs：オオカミリスト
	// @param fRadius：近隣として扱う距離
	void RefreshNeighbors(const std::vector<CWolf_Animal*>& In_Wolfs, float fRadius);

	// @brief 近隣の同種動物の情報を1頭分追加する
	// @param pWolf：近隣として追加する狼
	void AppendNeighbor(CWolf_Animal* pWolf);

	// @brief 生成されている全てのオオカミ(群れ情報の更新用)
	inline static std::vector<CWolf_Animal*> s_AllWolves;

	// @brief 近隣検索用の空間分割(更新の1ステップに1回作り直す)
	inline static CSpatialGrid<CWolf_Animal> s_Grid;

	// @brief 空間分割を作った時の、更新のステップ数
	inline static unsigned int s_nGridStep = 0;

	// @brief 攻撃クールダウン（秒）
	float m_fAttackCooldown =0.0f;

	// @brief 満腹の残り時間（秒）
	// @note 獲物を仕留めてから、この時間が過ぎるまでは狩りをしない
	float m_fFedTimer =0.0f;
};

