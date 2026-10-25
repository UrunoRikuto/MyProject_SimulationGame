/**************************************************//*
	@file	| Scene.h
	@brief	| シーンのベースクラスのhファイル
	@note	| シーン内のゲームオブジェクト管理、更新、描画等を行う
*//**************************************************/
#pragma once
#include "GameObject.h"
#include <array>
#include <list>
#include <unordered_map>
#include <typeinfo>

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
        for (const auto& list : m_pGameObject_List)
        {
			// T*型のゲームオブジェクトが見つかった場合はその値を返す
            for (auto obj : list)
            {
				// T*型にキャストを試みる
                T* ret = CastCached<T>(obj);

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
        for (const auto& list : m_pGameObject_List)
        {
			// IDが一致するゲームオブジェクトが見つかった場合はその値を返す
            for (auto obj : list)
            {
				// オブジェクトのIDを取得
                const ObjectID& id = obj->GetID();
				// IDが一致するか確認
                if (id.m_nSameCount == inID.m_nSameCount &&
                    id.m_sName == inID.m_sName)
                {
					// 見つかった場合はそのポインタを返す
                    return obj;
                }
            }
        }

		// 見つからなかった場合はnullptrを返す
		return nullptr;
	}

	// @brief 近い位置にあるゲームオブジェクトを取得する
	// @tparam T：取得するCGameObject型のゲームオブジェクトクラス
	// @param inPos：基準位置
	template<typename T = CGameObject>
	T* GetGameObject(const DirectX::XMFLOAT3 inPos)
	{
		// 最小距離の初期化
		float minDistance = FLT_MAX;
		// 近いオブジェクトのポインタ
		T* pNearObject = nullptr;

		// 自身を紐付けている全てのゲームオブジェクトを探索
		for (const auto& list : m_pGameObject_List)
		{
			// T*型のゲームオブジェクトが見つかった場合はその値を返す
			for (auto obj : list)
			{
				// T*型にキャストを試みる
				T* ret = CastCached<T>(obj);
				// 見つかった場合は距離を計算
				if (ret != nullptr)
				{
					// オブジェクトの位置を取得
					DirectX::XMFLOAT3 objPos = ret->GetPos();

					// 距離を計算
					float dx = inPos.x - objPos.x;
					float dy = inPos.y - objPos.y;
					float dz = inPos.z - objPos.z;
					float distance = dx * dx + dy * dy + dz * dz;
					// 最小距離よりも小さい場合は更新
					if (distance < minDistance)
					{
						minDistance = distance;
						pNearObject = ret;
					}
				}
			}
		}

		// 近いオブジェクトのポインタを返す
		return pNearObject;
	}

	// @brief 近い位置にあるゲームオブジェクトを取得する
	// @tparam T：取得するCGameObject型のゲームオブジェクトクラス
	// @param inPos：基準位置
	// @param inNoFindID：除外するオブジェクトIDリスト
	template<typename T = CGameObject>
	T* GetGameObject(const DirectX::XMFLOAT3 inPos, const std::vector<ObjectID>& inNoFindID)
	{
		// 最小距離の初期化
		float minDistance = FLT_MAX;
		// 近いオブジェクトのポインタ
		T* pNearObject = nullptr;

		// 自身を紐付けている全てのゲームオブジェクトを探索
		for (const auto& list : m_pGameObject_List)
		{
			// T*型のゲームオブジェクトが見つかった場合はその値を返す
			for (auto obj : list)
			{
				bool isSkip = false;
				// T*型にキャストを試みる
				T* ret = CastCached<T>(obj);
				// 見つかった場合は距離を計算
				if (ret != nullptr)
				{
					for(int i = 0; i < inNoFindID.size(); i++)
					{
						// 除外するオブジェクトIDと一致する場合はスキップ
						const ObjectID& id = ret->GetID();
						if (id.m_nSameCount == inNoFindID[i].m_nSameCount &&
							id.m_sName == inNoFindID[i].m_sName)
						{
							isSkip = true;
							break;
						}
					}

					// 除外するオブジェクトIDと一致した場合はスキップ
					if (isSkip)continue;

					// オブジェクトの位置を取得
					DirectX::XMFLOAT3 objPos = ret->GetPos();

					// 距離を計算
					float dx = inPos.x - objPos.x;
					float dy = inPos.y - objPos.y;
					float dz = inPos.z - objPos.z;
					float distance = dx * dx + dy * dy + dz * dz;
					// 最小距離よりも小さい場合は更新
					if (distance < minDistance)
					{
						minDistance = distance;
						pNearObject = ret;
					}
				}
			}
		}

		// 近いオブジェクトのポインタを返す
		return pNearObject;
	}

	// @brief ゲームオブジェクトを名前で取得する
	// @param inName：オブジェクト名
	// @return 一番最初に見つかったCGameObject型のポインタ、見つからなかった場合はnullptr
    CGameObject* GetGameObject(std::string inName)
	{
		// 自身を紐付けている全てのゲームオブジェクトを探索
        for (const auto& list : m_pGameObject_List)
        {
			// 名前が一致するゲームオブジェクトが見つかった場合はその値を返す
            for (auto obj : list)
            {
				// オブジェクトのIDを取得
                const ObjectID& id = obj->GetID();

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

	template<typename T = CGameObject>
	std::list<T*> GetGameObjects()
	{
		std::list<T*> gameObjectList;
		gameObjectList.clear();

		// 自身を紐付けている全てのゲームオブジェクトを探索
		for (const auto& list : m_pGameObject_List)
		{
			// T*型のゲームオブジェクトが見つかった場合はその値をリストに追加
			for (auto obj : list)
			{
				// T*型にキャストを試みる
				T* ret = CastCached<T>(obj);
				// 見つかった場合はリストに追加
				if (ret != nullptr)
				{
					gameObjectList.push_back(ret);
				}
			}
		}
		return gameObjectList;
	}

	// @brief 指定した位置から一定距離内にあるゲームオブジェクトを取得する
	// @tparam T：取得するCGameObject型のゲームオブジェクトクラス
	// @param In_Center：基準位置
	// @param distance：距離
	// @return 指定した位置から一定距離内にあるゲームオブジェクトのリスト
	template<typename T = CGameObject>
	std::list<T*> GetGameObjects(DirectX::XMFLOAT3 In_Center, float distance)
	{
		std::list<T*> gameObjectList;
		gameObjectList.clear();
		float distanceSq = distance * distance;
		// 自身を紐付けている全てのゲームオブジェクトを探索
		for (const auto& list : m_pGameObject_List)
		{
			// T*型のゲームオブジェクトが見つかった場合はその値をリストに追加
			for (auto obj : list)
			{
				// T*型にキャストを試みる
				T* ret = CastCached<T>(obj);
				// 見つかった場合は距離を計算
				if (ret != nullptr)
				{
					// オブジェクトの位置を取得
					DirectX::XMFLOAT3 objPos = ret->GetPos();
					// 距離の二乗を計算
					float distSq = 
						powf(In_Center.x - objPos.x, 2) +
						powf(In_Center.y - objPos.y, 2) +
						powf(In_Center.z - objPos.z, 2);
					// 指定した距離内にある場合はリストに追加
					if (distSq <= distanceSq)
					{
						gameObjectList.push_back(ret);
					}
				}
			}
		}
		return gameObjectList;
	}

	// @brief フェード中かどうかの設定・取得
	// @param isFade：フェード中かどうか
    void SetIsFade(bool isFade) { m_bFade = isFade; }

	// @brief フェード中かどうかの取得
	// @return フェード中かどうか
    bool GetIsFade() { return m_bFade; }

protected:
	// @brief シーン内のゲームオブジェクトリスト
    std::array<std::list<CGameObject*>,(int)Tag::Max> m_pGameObject_List;

private:
	// @brief シーン内の全てのオブジェクトIDリスト
    std::vector<ObjectID> m_tIDVec;

	// @brief オブジェクトをT型として取得する
	// @tparam T：取得するCGameObject型のゲームオブジェクトクラス
	// @param obj：対象のオブジェクト
	// @return T型のポインタ、T型として扱えない場合はnullptr
	// @note オブジェクトの実際の型ごとに「T型として扱えるか」を記憶し、
	//       T型ではないオブジェクトに対するdynamic_castを省いて検索を高速化する
	template<typename T>
	static T* CastCached(CGameObject* obj)
	{
		static std::unordered_map<const std::type_info*, bool> s_IsKindOf;

		const std::type_info* pType = &typeid(*obj);
		auto it = s_IsKindOf.find(pType);
		if (it == s_IsKindOf.end())
		{
			it = s_IsKindOf.emplace(pType, dynamic_cast<T*>(obj) != nullptr).first;
		}

		return it->second ? dynamic_cast<T*>(obj) : nullptr;
	}

	// @brief フェード中かどうかのフラグ
    bool m_bFade = false;

};


// 派生クラスのインクルード
#include "SceneTitle.h"
#include "SceneGame.h"