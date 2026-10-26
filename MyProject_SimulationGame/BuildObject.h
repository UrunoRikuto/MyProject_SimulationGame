/**************************************************//*
	@file	| BuildObject.h
	@brief	| 建築オブジェクトクラスのhファイル
	@note	| 建築オブジェクトの処理を定義
			| CGameObjectを継承
*//**************************************************/
#pragma once
#include "GameObject.h"
#include "BillboardRenderer.h"
#include <sstream>
#include <string>

// @brief 建築オブジェクトクラス
class CBuildObject : public CGameObject
{
public:
	// @brief 建築物レベルの最大値
	static constexpr int MAX_BUILD_LEVEL = 5;

	// @brief 建築完成度の最大値
	static constexpr float BUILD_PROGRESS_MAX = 100.0f;

public:
	// @brief コンストラクタ
	CBuildObject();

	// @brief デストラクタ
	virtual ~CBuildObject();

	// @brief 初期化処理
	virtual void Init() override;

	// @brief 描画処理
	virtual void Draw() override;

	// @brief インスペクター表示処理
	virtual int Inspecter();

	// @brief 施設の説明文を取得
	// @return 施設の役割と、レベルを上げたときの効果の説明(改行区切り)
	virtual std::string GetDescription() const { return std::string(); }

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
	bool IsCompleted() const { return m_fBuildProgress >= BUILD_PROGRESS_MAX; }

	// @brief 建築完成度を進める
	// @param fAmount：進める量
	void ProgressBuild(float fAmount);

	// @brief 建築完成度を初期化
	void InitializeBuildProgress() { m_fBuildProgress = 0.0f; }

protected:
	// @brief レベルごとの値を「1 / 2 / 3」のように並べた文字列にする(説明文用)
	// @param values：レベルごとの値の配列(Lv1から順に)
	// @param fDivisor：値をこの数で割って表示する(Lv1の値を1倍とした倍率で表示したいときに使う)
	// @param pUnit：末尾に付ける単位
	template<typename T, size_t N>
	static std::string FormatLevelValues(const T(&values)[N], float fDivisor, const char* pUnit)
	{
		std::ostringstream stream;
		for (size_t i = 0; i < N; ++i)
		{
			if (i > 0) stream << " / ";
			stream << static_cast<float>(values[i]) / fDivisor;
		}
		stream << pUnit;
		return stream.str();
	}

protected:
	// @brief 建築完成度
	float m_fBuildProgress;

	// @brief 建築物レベル
	int m_nBuildLevel;

	// @brief 自分を配置しているフィールドセルのインデックス
	DirectX::XMINT2 m_n2FieldCellIndex;

	// @brief 建築進行度ビルボードレンダラー
	CBillboardRenderer* m_pBuildProgressBillboard;
};

