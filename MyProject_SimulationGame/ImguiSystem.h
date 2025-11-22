/**************************************************//*
	@file	| ImguiSystem.h
	@brief	| Imguiシステムクラスのhファイル
	@note	| Imguiの初期化、更新、描画等を行う
			| シングルトンパターンで作成
*//**************************************************/
#pragma once
#include "GameObject.h"
#include "imgui.h"
#include "Generator.h"



constexpr ImVec2 ce_f2InspecterSize = ImVec2(250, 30);

// @brief Imguiシステムクラス
class CImguiSystem
{
private:

	// @brief デバッグログ構造体
	struct DebugLogInfo
	{
		// @brief ログ文字列
		std::string m_sLog;
		// @brief ログ消去フラグ
		bool m_bClear;
	};

	// @brief デバッグ用ジェネレーター情報構造体
	struct DebugGeneratorInfo
	{
		// @brief ジェネレーター名
		std::string m_sName;

		// @brief ジェネレータータイプ
		IGenerator* m_pGenerator;
	};

	// @brief デバッグ用フラグ列挙型
	enum class DebugSystemFlag
	{
		Update,
		FPS,
		Log,
		Generate,

		MAX
	};

private:

	// @brief コンストラクタ
	CImguiSystem();

public:
	// デフォルトコンストラクタ禁止
	CImguiSystem(const CImguiSystem&) = delete;
	// 代入演算子禁止
	CImguiSystem& operator=(const CImguiSystem&) = delete;

	// @brief デストラクタ
	~CImguiSystem();

	// @brief インスタンスの取得
	static CImguiSystem* GetInstance();

	// @brief インスタンスの解放
	static void ReleaseInstance();

	// @brief 初期化処理
	void Init();

	// @brief 終了処理
	void Uninit();

	// @brief 更新処理
	void Update();

	// @brief 描画処理
	void Draw();

	// @brief 更新処理を管理するフラグの取得
	// @return true:更新処理を行う false:更新処理を止める
	bool IsUpdate() { return m_bUpdate; }

	// @brief 選択しているゲームオブジェクトの取得
	// @return 選択しているゲームオブジェクトのポインタ
	CGameObject* GetSelectedGameObject() { return m_pGameObject; }

	// @brief デバックログの登録
	// @param log：登録するログ文字列
	// @param clear：true:描画後に削除 false:常に残り続ける
	void AddDebugLog(const std::string& log, bool clear = true);
private:

	// @brief 階層表示
	void DrawHierarchy();

	// @brief カメラのパラメータ表示
	void DrawCameraParam();

	// @brief インスペクター表示
	void DrawInspecter();

	// @brief デバッグ用チェックボックス表示
	void DrawDebugSystem();

	// @brief 更新を止めるチェックボックス表示
	void DrawUpdateTick();

	// @brief フレームレート表示
	void DrawFPS();

	// @brief デバックログ表示
	void DrawDebugLog();

	// @brief オブジェクト生成ボタン表示
	void DrawCreateObjectButton();

private:
	// @brief インスタンス
	static CImguiSystem* m_pInstance;

	// @brief デバッグログ表示用バッファ
	std::vector<DebugLogInfo> m_DebugLog;

	// @brief 選択しているゲームオブジェクト
	CGameObject* m_pGameObject;

	// @brief 生成ポインタ配列
	std::vector<DebugGeneratorInfo> m_pGenerator;

	// @brief デバッグ用フラグ配列
	bool m_bDebug[static_cast<int>(DebugSystemFlag::MAX)];

	// @brief 更新処理を管理する
	// @note true:更新処理を行う false:更新処理を止める
	bool m_bUpdate;
};

