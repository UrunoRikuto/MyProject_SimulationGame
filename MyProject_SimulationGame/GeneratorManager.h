/**************************************************//*
	@file	| GeneratorManager.h
	@brief	| 生成管理システムのhファイル
	@note	| オブザーバーパターンで作成
			| シングルトンパターンで作成
*//**************************************************/
#pragma once
#include "SubJect.h"
#include "Generator.h"

// @brief 生成管理システムクラス
class CGeneratorManager final : public ISubJect
{
private:
	// @brief コンストラクタ
	CGeneratorManager() = default;

	// @brief コピーコンストラクタ（禁止）
	CGeneratorManager(const CGeneratorManager& other) = delete;

	// @brief コピー代入演算子（禁止）
	CGeneratorManager& operator=(const CGeneratorManager& other) = delete;

public:
	// @brief デストラクタ
	~CGeneratorManager() override = default;

	// @brief オブザーバーへの通知処理
	void NotifyObservers() override;

	// @brief オブザーバーの通知処理
	template<typename T = IGenerator>
	void NotifyObserver();

	// @brief シングルトンインスタンスの取得
	static CGeneratorManager* GetInstance();

	// @brief シングルトンインスタンスの解放
	static void ReleaseInstance();

private:
	// @brief シングルトンインスタンス
	static CGeneratorManager* m_pInstance;
};

// Generator処理のインクルード
#include "WoodGenerator.h"
#include "StoneGenerator.h"
#include "HumanGenerator.h"
