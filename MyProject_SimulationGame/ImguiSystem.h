/**************************************************//*
	@file	| ImguiSystem.h
	@brief	| Imguiシステムクラスのhファイル
	@note	| Imguiの初期化、更新、描画等を行う
			| シングルトンパターンで作成
*//**************************************************/
#pragma once
#include "GameObject.h"
#include "Human.h"
#include "BuildObject.h"
#include "imgui.h"
#include "Generator.h"
#include <string>



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

	// @brief デバッグ用フラグ列挙型
	enum class DebugSystemFlag
	{
		History＆Inspecter,
		CameraParam,
		BuildRequestList,
		GenerateRequestList,
		Update,
		FieldCells,
		FPS,
		AllObjectNum,
		Log,
		AddStoragehouseItem,

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

	// @brief シード値入力モードの表示
	void DrawSeedInputMode();
	// @brief シード値の設定フラグ
	bool IsSettingSeed() { return m_bSettingSeed; }

	// @brief シード値の取得
	unsigned int GetSeed() { return m_nSeed; }
	// @brief シード値の設定
	void SetSeed(unsigned int seed) { m_nSeed = seed; }
	// @brief シード値設定の決定
	void DecideSettingSeed();

	// @brief 更新処理を管理するフラグの取得
	// @return true:更新処理を行う false:更新処理を止める
	bool IsUpdate() { return m_bUpdate; }

	// @brief フィールドセル描画フラグの取得
	bool IsFieldCellsDraw() { return m_bCellsDraw; }
	// @brief フィールドセル表示モードの取得
	int GetFieldCellDisplayMode() { return m_nCellDisplayMode; }

	// @brief 選択しているゲームオブジェクトの取得
	// @return 選択しているゲームオブジェクトのポインタ
	CGameObject* GetSelectedGameObject() { return m_pGameObject; }

	// @brief 選択している人間の取得
	// @return 選択している人間(選択していない場合はnullptr)
	CHuman* GetSelectedHuman() const { return m_pHumanObject; }

	// @brief 選択しているゲームオブジェクトの解除
	// @note 選択しているゲームオブジェクトが破棄された場合に呼び出す
	void ClearSelectedGameObject() { m_pGameObject = nullptr; }

	// @brief デバックログの登録
	// @param log：登録するログ文字列
	// @param clear：true:描画後に削除 false:常に残り続ける
	void AddDebugLog(const std::string& log, bool clear = true);

	// @brief ゲーム速度(倍速)の取得
	// @return 1フレームに行うゲームの更新回数
	int GetGameSpeed() const { return m_nGameSpeed; }

	// @brief ゲーム速度(倍速)の設定
	// @param nSpeed：1フレームに行うゲームの更新回数(1以上、選択できる最大倍率以下に収められる)
	void SetGameSpeed(int nSpeed);

	// @brief 実際に行えたゲームの更新回数の通知
	// @param nUpdateCount：直前のフレームで実際に行った更新回数
	// @note 処理が追いつかず倍速を実現できていない場合の、実効倍率の表示に使用する
	void SetAppliedGameSpeed(int nUpdateCount);


private: //-- デバックモード --//

	// @brief ヒストリー表示
	void DrawHistory();

	// @brief カメラのパラメータ表示
	void DrawCameraParam();

	// @brief インスペクター表示
	void DrawInspecter();

	// @brief 選択しているオブジェクトの確認
	// @note 選択中のオブジェクトが破棄されて解放済みになっている場合は、選択を解除する
	void ValidateSelection();

	// @brief 建築依頼リスト
	void DrawBuildRequestList();

	// @brief 生成依頼リスト
	void DrawGenerateRequestList();

	// @brief デバッグ用チェックボックス表示
	void DrawDebugSystem();

	// @brief 更新を止めるチェックボックス表示
	void DrawUpdateTick();

	// @brief フィールドセル表示
	void DrawFieldCells();

	// @brief フレームレート表示
	void DrawFPS();

	// @brief 全オブジェクト数表示
	void DrawAllObjectNum();

	// @brief デバックログ表示
	void DrawDebugLog();

	// @brief 倉庫にアイテムを収納する
	void DrawStoragehouseItem();

private: //-- リリースモード --//

	// @brief 文明レベル表示
	void Release_DrawCivLevel();

	// @brief ゲーム内時間表示
	void Release_DrawGameTime();

	// @brief ゲーム速度表示
	void Release_DrawGameSpeed();

	// @brief 人間の表示
	void Release_DrawHuman();

	// @brief 人間の一覧表示(職業ごとの人数の増減と、人間ごとの職業設定)
	void Release_DrawHumanList();

	// @brief 建築物の表示
	void Release_DrawBuildObject();

private:
	// @brief インスタンス
	static CImguiSystem* m_pInstance;

	// @brief リリース時のフォントサイズ
	ImFont* m_pReleaseFont;
	// @brief デバッグ時のフォントサイズ
	ImFont* m_pDebugFont;

	// @brief デバッグログ表示用バッファ
	std::vector<DebugLogInfo> m_DebugLog;

	// @brief 選択しているゲームオブジェクト
	CGameObject* m_pGameObject;
	// @brief 人間オブジェクト
	CHuman* m_pHumanObject;
	// @brief 建築物オブジェクト
	CBuildObject* m_pBuildObject;

	// @brief デバッグ用フラグ配列
	bool m_bDebug[static_cast<int>(DebugSystemFlag::MAX)];

	// @brief 更新処理を管理する
	// @note true:更新処理を行う false:更新処理を止める
	bool m_bUpdate;

	// @brief デバッグ用セル描画フラグ
	bool m_bCellsDraw;
	// @brief セルの表示モード
	int m_nCellDisplayMode;

	// @brief 人間のみ表示するフラグ
	bool m_bOnlyHuman;

	// @brief 倉庫の資源表示フラグ
	bool m_bStoragehouseDisplay;

	// @brief シード値
	std::string m_seedInput;
	unsigned int m_nSeed;
	// @brief シード値設定フラグ
	bool m_bSettingSeed = true;

	// @brief ゲーム速度(1フレームに行うゲームの更新回数)
	int m_nGameSpeed = 1;

	// @brief 実際に行えたゲームの更新回数の移動平均
	float m_fAppliedGameSpeed = 1.0f;
};

