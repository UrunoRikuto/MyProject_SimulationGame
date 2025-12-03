/**************************************************//*
	@file	| BuildObject.h
	@brief	| 建築オブジェクトクラスのhファイル
	@note	| 建築オブジェクトの処理を定義
			| CGameObjectを継承
*//**************************************************/
#pragma once
#include "GameObject.h"

// @brief 建築オブジェクトクラス
class CBuildObject : public CGameObject
{
public:
	// 建築物レベルの最大値
	static constexpr int MAX_BUILD_LEVEL = 5;

public:
	// @brief コンストラクタ
	CBuildObject();

	// @brief デストラクタ
	virtual ~CBuildObject();

	// @brief 初期化処理
	virtual void Init() override;

	// @brief インスペクター表示処理
	virtual int Inspecter(bool isEnd = true) override;

	// @brief 建築物レベルが最大かどうかを取得
	bool IsMaxBuildLevel() const { return m_nBuildLevel >= MAX_BUILD_LEVEL; }

	// @brief 建築物レベルを進める
	void UpgradeBuildLevel();

	// @brief 建築物レベルを取得
	int GetBuildLevel() const { return m_nBuildLevel; }

	// @brief 自分を配置しているフィールドセルのインデックスを取得
	// @return フィールドセルのインデックス
	DirectX::XMINT2 GetFieldCellIndex() const { return m_n2FieldCellIndex; }

	// @brief 自分を配置しているフィールドセルのインデックスを設定
	// @param inIndex：フィールドセルのインデックス
	void SetFieldCellIndex(DirectX::XMINT2 inIndex) { m_n2FieldCellIndex = inIndex; }

	// @brief 完成しているかどうかを取得
	// @return true:完成している false:未完成
	bool IsCompleted() const { return m_fBuildProgress >= 100.0f; }

	// @brief 建築完成度を進める
	// @param fAmount：進める量
	void ProgressBuild(float fAmount);

	// @brief 建築完成度を初期化
	void InitializeBuildProgress() { m_fBuildProgress = 0.0f; }

protected:
	// @brief 建築完成度
	float m_fBuildProgress;

	// @brief 建築物レベル
	int m_nBuildLevel;

	// @brief 自分を配置しているフィールドセルのインデックス
	DirectX::XMINT2 m_n2FieldCellIndex;
};

