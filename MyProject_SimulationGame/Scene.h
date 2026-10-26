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
#include <typeindex>
#include <vector>
#include <memory>
#include <algorithm>
#include <cfloat>

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
		NotifyObjectAdded(gameObject);

		// オブジェクトIDの設定(同じ名前のオブジェクトが既にいくつあるかは、名前ごとのカウンタで数える)
		ObjectID id{};
		id.m_sName = inName;
		id.m_nSameCount = m_NameCounts[inName]++;
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
		// 型別のキャッシュの先頭(最初に見つかったもの)を返す
		const std::vector<T*>& items = GetKindOf<T>();
		return items.empty() ? nullptr : items.front();
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

	// @brief ゲームオブジェクトがこのシーンに存在するかどうかの確認
	// @param pObj：確認するゲームオブジェクトのポインタ
	// @return true:存在する false:存在しない(破棄・解放済み)
	// @note 保持しているポインタが、破棄されて解放済みになっていないかの確認に使用する
	bool Contains(const CGameObject* pObj)
	{
		if (pObj == nullptr) return false;

		for (const CGameObject* pCandidate : GetKindOf<CGameObject>())
		{
			if (pCandidate == pObj) return true;
		}
		return false;
	}

	// @brief ゲームオブジェクトをIDで取得する(型指定版)
	// @tparam T：取得するCGameObject型のゲームオブジェクトクラス
	// @param inID：オブジェクトID
	// @return 一致したT*型のポインタ、見つからなかった場合はnullptr
	// @note T型のオブジェクトだけを探索するため、全オブジェクトを探索するGetGameObject(ObjectID)より高速
	template<typename T>
	T* FindGameObjectByID(const ObjectID& inID)
	{
		for (T* ret : GetKindOf<T>())
		{
			const ObjectID& id = ret->GetID();
			if (id.m_nSameCount == inID.m_nSameCount && id.m_sName == inID.m_sName)
			{
				return ret;
			}
		}
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

		// T*型のゲームオブジェクトだけを探索
		for (T* ret : GetKindOf<T>())
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

		// T*型のゲームオブジェクトだけを探索
		for (T* ret : GetKindOf<T>())
		{
			bool isSkip = false;
			const ObjectID& id = ret->GetID();
			for (size_t i = 0; i < inNoFindID.size(); i++)
			{
				// 除外するオブジェクトIDと一致する場合はスキップ
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
		// 型別のキャッシュから、呼び出し側が自由に扱えるようにコピーして返す
		const std::vector<T*>& items = GetKindOf<T>();
		return std::list<T*>(items.begin(), items.end());
	}

	// @brief 条件に合う、一番近い位置にあるゲームオブジェクトを取得する
	// @tparam T：取得するCGameObject型のゲームオブジェクトクラス
	// @param inPos：基準位置
	// @param pred：候補にするかどうかを判定する処理(T*を受け取り、候補にする場合はtrueを返す)
	// @return 条件に合う最も近いT*型のポインタ、無ければnullptr
	template<typename T, typename Pred>
	T* GetNearestGameObjectIf(const DirectX::XMFLOAT3& inPos, Pred&& pred)
	{
		float minDistance = FLT_MAX;
		T* pNearObject = nullptr;
		for (T* ret : GetKindOf<T>())
		{
			if (!pred(ret)) continue;

			const DirectX::XMFLOAT3 objPos = ret->GetPos();
			const float dx = inPos.x - objPos.x;
			const float dy = inPos.y - objPos.y;
			const float dz = inPos.z - objPos.z;
			const float distance = dx * dx + dy * dy + dz * dz;
			if (distance < minDistance)
			{
				minDistance = distance;
				pNearObject = ret;
			}
		}
		return pNearObject;
	}

	// @brief T型として扱えるゲームオブジェクト全てに対して、順に処理を行う
	// @tparam T：対象のCGameObject型のゲームオブジェクトクラス
	// @param func：各オブジェクト(T*)に対して呼び出す処理
	// @note 一覧のコピーを作らないため、GetGameObjects<T>()より軽い。処理の中でオブジェクトの追加・破棄は行わないこと
	template<typename T, typename F>
	void ForEachGameObject(F&& func)
	{
		for (T* pObj : GetKindOf<T>())
		{
			func(pObj);
		}
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
		float distanceSq = distance * distance;

		// T*型のゲームオブジェクトだけを探索
		for (T* ret : GetKindOf<T>())
		{
			// オブジェクトの位置を取得
			DirectX::XMFLOAT3 objPos = ret->GetPos();
			// 距離の二乗を計算
			float dx = In_Center.x - objPos.x;
			float dy = In_Center.y - objPos.y;
			float dz = In_Center.z - objPos.z;
			float distSq = dx * dx + dy * dy + dz * dz;
			// 指定した距離内にある場合はリストに追加
			if (distSq <= distanceSq)
			{
				gameObjectList.push_back(ret);
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
	// @brief ゲームオブジェクトを追加したことを、型別の検索結果のキャッシュと更新の対象の一覧に反映する
	void NotifyObjectAdded(CGameObject* pObj)
	{
		for (auto& kv : m_KindCaches)
		{
			kv.second->OnAdded(pObj);
		}
		if (!pObj->IsStaticObject())
		{
			m_UpdateList.push_back(pObj);
		}
	}

	// @brief ゲームオブジェクトを解放する前に、型別の検索結果のキャッシュと更新の対象の一覧から外す
	// @note リストから削除・解放する直前に必ず呼ぶこと(解放済みのポインタが一覧に残らないようにするため)
	void NotifyObjectRemoving(CGameObject* pObj)
	{
		for (auto& kv : m_KindCaches)
		{
			kv.second->OnRemoved(pObj);
		}
		auto it = std::find(m_UpdateList.begin(), m_UpdateList.end(), pObj);
		if (it != m_UpdateList.end())
		{
			m_UpdateList.erase(it);
		}
	}

	// @brief 型別の検索結果のキャッシュと更新の対象の一覧を、全て破棄する
	void ClearObjectCaches()
	{
		m_KindCaches.clear();
		m_UpdateList.clear();
	}

	// @brief シーン内のゲームオブジェクトリスト
    std::array<std::list<CGameObject*>,(int)Tag::Max> m_pGameObject_List;

private:
	// @brief 型別の検索結果のキャッシュの基底
	struct IKindCache
	{
		virtual ~IKindCache() = default;

		// @brief オブジェクトが追加された時に、一覧へ反映する
		virtual void OnAdded(CGameObject* pObj) = 0;

		// @brief オブジェクトが解放される前に、一覧から外す
		virtual void OnRemoved(CGameObject* pObj) = 0;
	};

	// @brief T型として扱えるオブジェクトの一覧
	template<typename T>
	struct KindCache : IKindCache
	{
		std::vector<T*> m_Items;

		void OnAdded(CGameObject* pObj) override
		{
			if (T* p = dynamic_cast<T*>(pObj))
			{
				m_Items.push_back(p);
			}
		}

		void OnRemoved(CGameObject* pObj) override
		{
			if (T* p = dynamic_cast<T*>(pObj))
			{
				auto it = std::find(m_Items.begin(), m_Items.end(), p);
				if (it != m_Items.end())
				{
					m_Items.erase(it);
				}
			}
		}
	};

	// @brief T型として扱えるオブジェクトの一覧を取得する
	// @tparam T：取得するCGameObject型のゲームオブジェクトクラス
	// @return T型のオブジェクトの一覧
	// @note 最初の1回だけ全オブジェクトを調べて作り、以降はオブジェクトの追加・解放のたびに更新する
	//       (増減のたびに作り直すと、オブジェクトが多いほど、増減の直後の検索が重くなってカクつくため)
	//       返す参照は、シーンにオブジェクトを追加・解放するまでの間だけ有効(呼び出し側は検索中にシーンを操作しないこと)
	template<typename T>
	const std::vector<T*>& GetKindOf()
	{
		std::unique_ptr<IKindCache>& slot = m_KindCaches[std::type_index(typeid(T))];
		if (!slot)
		{
			auto pNewCache = std::make_unique<KindCache<T>>();
			for (const auto& list : m_pGameObject_List)
			{
				for (auto obj : list)
				{
					T* ret = CastCached<T>(obj);
					if (ret != nullptr)
					{
						pNewCache->m_Items.push_back(ret);
					}
				}
			}
			slot = std::move(pNewCache);
		}
		return static_cast<KindCache<T>*>(slot.get())->m_Items;
	}

	// @brief 型別の検索結果のキャッシュ
	std::unordered_map<std::type_index, std::unique_ptr<IKindCache>> m_KindCaches;

	// @brief シーン内の全てのオブジェクトIDリスト
    std::vector<ObjectID> m_tIDVec;

	// @brief 更新の対象になるオブジェクトの一覧(静的なオブジェクトを除く。追加・解放のたびに更新する)
	std::vector<CGameObject*> m_UpdateList;


	// @brief 名前ごとに追加したオブジェクトの数(オブジェクトIDの同名の通し番号の計算用)
	std::unordered_map<std::string, int> m_NameCounts;

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