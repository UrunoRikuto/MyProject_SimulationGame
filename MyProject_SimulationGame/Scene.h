/**************************************************//*
	@file	| Scene.h
	@brief	| シーンのベースクラスのhファイル
	@note	| シーン内のゲームオブジェクト管理、更新、描画等を行う
*//**************************************************/
#pragma once
#include "GameObject.h"
#include <array>
#include <list>
#include "CollisionBase.h"

// @brief シーンベースクラス
class CScene
{
public:
	// @brief コンストラクタ
	CScene();

	// @brief デストラクタ
	virtual ~CScene();

	// @brief 初期化処理
	virtual void Init();

	// @brief 終了処理
	virtual void Uninit();

	// @brief 更新処理
	virtual void Update();

	// @brief 描画処理
	virtual void Draw();

    // @brief ゲームオブジェクトを追加する
	// @tparam T：追加するCGameObject型のゲームオブジェクトクラス
	// @param inTag：識別用のタグ
	// @param inName：オブジェクトの名前
	// @return 追加したゲームオブジェクトクラスのポインタ
	template<typename T = CGameObject>
	T* AddGameObject(Tag inTag, std::string inName)
	{
		// ゲームオブジェクトをインスタンス化
		T* gameObject;
		gameObject = new(std::nothrow) T();
        if (gameObject == nullptr)
        {
            return nullptr;
		}
		// ゲームオブジェクトリストに追加
		m_pGameObject_List[(int)inTag].push_back(gameObject);

		// オブジェクトIDの設定
        ObjectID id{};
		// 同じ名前のオブジェクトがいくつあるかカウント
        for (auto itr : m_tIDVec)
        {
			// もし同じ名前のオブジェクトがいたらカウントを増やす
            if (itr.m_sName == inName)
            {
				// 同じ名前のオブジェクト数をカウントアップ
                id.m_nSameCount++;
            }
        }
		// オブジェクトIDの名前と同じ名前のオブジェクト数を設定
        id.m_sName = inName;
        m_tIDVec.push_back(id);
		// ゲームオブジェクトにIDを設定
        gameObject->SetID(id);

		// ゲームオブジェクトの初期化処理
		gameObject->Init();

		// ゲームオブジェクトにタグを設定
        gameObject->SetTag(inTag);

		// 衝突判定用コンポーネントリストに追加
        std::list<CCollisionBase*> pCollisionList = gameObject->GetSameComponents<CCollisionBase>();

		// 衝突判定用コンポーネントをシーンのリストに追加
        for (auto itr : pCollisionList)
        {
			// シーンの衝突判定用コンポーネントリストに追加
            m_pCollisionVec.push_back(itr);
        }

		// 追加したゲームオブジェクトを返す
		return gameObject;
	}

    // @brief ゲームオブジェクトを取得する
	// @tparam T：取得するCGameObject型のゲームオブジェクトクラス
	// @return 一番最初に見つかったCGameObject型のポインタ、見つからなかった場合はnullptr
	template<typename T = CGameObject>
	T* GetGameObject()
	{
		// 自身を紐付けている全てのゲームオブジェクトを探索
        for (auto list : m_pGameObject_List)
        {
			// T*型のゲームオブジェクトが見つかった場合はその値を返す
            for (auto obj : list)
            {
				// T*型にキャストを試みる
                T* ret = dynamic_cast<T*>(obj);

				// 見つかった場合はそのポインタを返す
                if (ret != nullptr) return ret;
            }
        }

		// 見つからなかった場合はnullptrを返す
		return nullptr;
	}

	// @brief ゲームオブジェクトをIDで取得する
	// @param inID：オブジェクトID
	// @return 一番最初に見つかったCGameObject型のポインタ、見つからなかった場合はnullptr
	CGameObject* GetGameObject(ObjectID inID)
	{
		// 自身を紐付けている全てのゲームオブジェクトを探索
        for (auto list : m_pGameObject_List)
        {
			// IDが一致するゲームオブジェクトが見つかった場合はその値を返す
            for (auto obj : list)
            {
				// オブジェクトのIDを取得
                ObjectID id = obj->GetID();
				// IDが一致するか確認
                if (id.m_sName == inID.m_sName &&
                    id.m_nSameCount == inID.m_nSameCount)
                {
					// 見つかった場合はそのポインタを返す
                    return obj;
                }
            }
        }

		// 見つからなかった場合はnullptrを返す
		return nullptr;
	}

	// @brief ゲームオブジェクトを名前で取得する
	// @param inName：オブジェクト名
	// @return 一番最初に見つかったCGameObject型のポインタ、見つからなかった場合はnullptr
    CGameObject* GetGameObject(std::string inName)
	{
		// 自身を紐付けている全てのゲームオブジェクトを探索
        for (auto list : m_pGameObject_List)
        {
			// 名前が一致するゲームオブジェクトが見つかった場合はその値を返す
            for (auto obj : list)
            {
				// オブジェクトのIDを取得
                ObjectID id = obj->GetID();

				// 名前が一致するか確認
                if (id.m_sName == inName)
                {
                    return obj;
                }
            }
        }

		// 見つからなかった場合はnullptrを返す
		return nullptr;
	}

	// @brief シーン内の全てのオブジェクトIDを取得する
	// @return オブジェクトIDのベクター
    std::vector<ObjectID> GetIDVec();

	// @brief ゲームオブジェクトリストの取得
	// @return ゲームオブジェクトリストの配列
    std::array<std::list<CGameObject*>, (int)Tag::Max> GetGameObjectList();

	// @brief 衝突判定用コンポーネントリストの取得
	// @return 衝突判定用コンポーネントリストのベクター
    std::vector<CCollisionBase*> GetCollisionVec() { return m_pCollisionVec; }

	// @brief フェード中かどうかの設定・取得
	// @param isFade：フェード中かどうか
    void SetIsFade(bool isFade) { m_bFade = isFade; }

	// @brief フェード中かどうかの取得
	// @return フェード中かどうか
    bool GetIsFade() { return m_bFade; }

protected:
	// @brief シーン内のゲームオブジェクトリスト
    std::array<std::list<CGameObject*>,(int)Tag::Max> m_pGameObject_List;

	// @brief 衝突判定用のコンポーネントリスト
    std::vector<CCollisionBase*> m_pCollisionVec;

private:
	// @brief シーン内の全てのオブジェクトIDリスト
    std::vector<ObjectID> m_tIDVec;

	// @brief フェード中かどうかのフラグ
    bool m_bFade = false;

};


// 派生クラスのインクルード
#include "SceneGame.h"