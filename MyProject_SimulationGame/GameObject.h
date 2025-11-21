/**************************************************//*
	@file	| GameObject.h
	@brief	| ゲームオブジェクト基底クラス
*//**************************************************/
#pragma once
#include "Defines.h"
#include "Component.h"
#include <vector>
#include <list>
#include "RendererComponent.h"
#include "CollisionBase.h"

// @brief オブジェクトタグ
enum class Tag
{
    // なし
	None,

    // スカイボックス
    SkyBox,    

    // フィールド
    Field,     

    // 音
    Sound,

    // ゲームオブジェクト
	GameObject,

    // UI
	UI,        

    Max
};

// @brief オブジェクト識別用ID
struct ObjectID
{
    // オブジェクトの名前
    std::string m_sName; 

    // 同オブジェクトの数
    int m_nSameCount;      
};


// @brief ゲームオブジェクト基底クラス
class CGameObject
{
public:
    // 初期化処理時に値を参照したい時は、その値をコンストラクタで初期化する
    // ※ここでのコンストラクタが各種Initより先に呼ばれる為

	// @brief コンストラクタ
    CGameObject();

	// @brief デストラクタ
    virtual ~CGameObject();

	// @brief 初期化処理
	virtual void Init();

	// @brief 終了処理
	virtual void Uninit();

	// @brief 更新処理
	virtual void Update();

	// @brief 描画処理
	virtual void Draw();

	// @brief 他のオブジェクトと衝突した時の処理
	// @param other：衝突先のオブジェクト
	// @param thisTag：衝突したオブジェクトのコリジョン識別用タグ
	 virtual void OnColliderHit(CCollisionBase* other, std::string thisTag = "None");

	// @brief オブジェクトが破棄された時の処理
    virtual void OnDestroy();

	// @brief オブジェクトを破棄する
    virtual void Destroy();

	// @brief オブジェクトが破棄予定かどうかを取得
	// @return true:破棄予定 false:破棄されていない
    bool IsDestroy();

	// @brief インスペクター表示処理
	// @param isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	// @return 表示した項目数
	// @note ImGuiを使用してオブジェクトのパラメータを表示、編集する
    virtual int Inspecter(bool isEnd = true);

	// @brief オブジェクトの前方向ベクトルの取得
	// @return (DirectX::XMFLOAT3)前方向単位ベクトル
    DirectX::XMFLOAT3 GetForward();

	// @brief オブジェクトの右方向ベクトルの取得
	// @return (DirectX::XMFLOAT3)右方向単位ベクトル
    DirectX::XMFLOAT3 GetRight();

	// @brief オブジェクトの上方向ベクトルの取得
	// @return (DirectX::XMFLOAT3)上方向単位ベクトル
    DirectX::XMFLOAT3 GetUp();

public:
	// @brief コンポーネントの追加
	// @tparam T：追加するCComponent型のコンポーネントクラス
	// @return 追加したコンポーネントクラスのポインタ
	template<typename T = CComponent>
	T* AddComponent()
	{
        // コンポーネントをインスタンス化し、自身を紐付ける
		T* pComponent = new(std::nothrow) T(this);

        // コンポーネントのリストに追加する
		m_pComponent_List.push_back(pComponent);

        // 初期化処理
		((CComponent*)pComponent)->Init();

		return pComponent;
	}
    
	// @brief コンポーネントの取得
	// @tparam T：取得するCComponent型のコンポーネントクラス
	// @param inTag：識別用のタグ
	// @return 一番最初に見つかったCComponent型のポインタ、見つからなかった場合はnullptr
	template<typename T = CComponent>
	T* GetComponent(std::string inTag = "")
	{
        if (inTag != "")
        {
            // 自身を紐付けている全てのコンポーネントを探索
            for (CComponent* pComponent : m_pComponent_List)
            {
                // 指定されたタグと一致するコンポーネントを探索
                if (pComponent->GetTag() == inTag)
                {
                    // T*型のコンポーネントが見つかった場合はその値を返す
                    T* pRet = dynamic_cast<T*>(pComponent);
                    if (pRet != nullptr) return pRet;
                }
            }
            // 見つからなかった場合はnullptrを返す
            return nullptr;
		}
        else
        {
            // 自身を紐付けている全てのコンポーネントを探索
            for (CComponent* pComponent : m_pComponent_List)
            {
                // T*型のコンポーネントが見つかった場合はその値を返す
                T* pRet = dynamic_cast<T*>(pComponent);
                if (pRet != nullptr) return pRet;
            }
        }

        // 見つからなかった場合はnullptrを返す
		return nullptr;
	}

	// @brief コンポーネントの取得
	// @tparam T：取得するCComponent型のコンポーネントクラス
	// @return CComponent型のポインタリスト、見つからなかった場合は要素数0のポインタリスト
    template<typename T = CComponent>
    std::list<T*> GetSameComponents()
    {
        std::list<T*> componentList;
        componentList.clear();

        // 自身を紐付けている全てのコンポーネントを探索
        for (CComponent* pComponent : m_pComponent_List)
        {
            // T*型のコンポーネントが見つかった場合はその値をリストに格納する
            T* pRet = dynamic_cast<T*>(pComponent);
            if (pRet != nullptr) componentList.push_back(pRet);
        }

        // 探索した結果のリストを返す
        return componentList;
    }

public:

    // @brief オブジェクトの座標をセット
	// @param inPos：オブジェクトの座標
    void SetPos(DirectX::XMFLOAT3 inPos) { m_tParam.m_f3Pos = inPos; }

	// @brief オブジェクトの座標を取得
	// @return (DirectX::XMFLOAT3)オブジェクトの座標
    DirectX::XMFLOAT3 GetPos() { return m_tParam.m_f3Pos; }
    
	// @brief オブジェクトのサイズをセット
	// @param inSize：オブジェクトのサイズ
    void SetSize(DirectX::XMFLOAT3 inSize) { m_tParam.m_f3Size = inSize; }

	// @brief オブジェクトのサイズを取得
	// @return (DirectX::XMFLOAT3)オブジェクトのサイズ
    DirectX::XMFLOAT3 GetSize() { return m_tParam.m_f3Size; }

	// @brief オブジェクトの回転をセット
	// @param inRotate：オブジェクトの回転
    void SetRotate(DirectX::XMFLOAT3 inRotate) { m_tParam.m_f3Rotate = inRotate; }

	// @brief オブジェクトの回転を取得
	// @return (DirectX::XMFLOAT3)オブジェクトの回転
    DirectX::XMFLOAT3 GetRotate() { return m_tParam.m_f3Rotate; }

	// @brief オブジェクトの色(RGBA)をセット
	// / @param inColor：オブジェクトの色
    void SetColor(DirectX::XMFLOAT4 inColor) { m_tParam.m_f4Color = inColor; }

	// @brief オブジェクトの色(RGBA)を取得
	// @return (DirectX::XMFLOAT4)オブジェクトの色
    DirectX::XMFLOAT4 GetColor() { return m_tParam.m_f4Color; }

	// @brief オブジェクトのタグをセット
	// @param inTag：オブジェクトのタグ
    void SetTag(Tag inTag) { m_eTag = inTag; }

	// @brief オブジェクトのタグを取得
	// @return (Tag)オブジェクトのタグ
    Tag GetTag() { return m_eTag; }
    
	// @brief オブジェクトの識別用IDをセット
	// @param inTag：オブジェクトの識別用ID
    void SetID(ObjectID inTag) { m_tID = inTag; }
    
	// @brief オブジェクトの識別用IDを取得
	// @return (ObjectID)オブジェクトの識別用ID
    ObjectID GetID() { return m_tID; }
    
	// @brief オブジェクトのワールド行列を取得
	// @return (DirectX::XMFLOAT4X4*)オブジェクトのワールド行列参照
    DirectX::XMFLOAT4X4* GetWorld();

	// @brief オブジェクトの移動処理
	// @param initPos：移動開始位置
	// @param targetPos：移動目標位置
	// @param time：経過時間
	// @param duration：移動にかかる時間
	// @param ease：イージングの種類(デフォルト0:なし)
    void MoveTo(DirectX::XMFLOAT3 initPos, DirectX::XMFLOAT3 targetPos,float time, float duration, int ease = 0);

	// @brief オブジェクトの移動処理
	// @param initPos：移動開始位置
	// @param targetPos：移動目標位置
	// @param moveObjectPos：移動させるオブジェクトの位置参照
	// @param time：経過時間
	// @param duration：移動にかかる時間
	// @param ease：イージングの種類(デフォルト0:なし)
    void MoveTo(DirectX::XMFLOAT3 initPos, DirectX::XMFLOAT3 targetPos, DirectX::XMFLOAT3* moveObjectPos, float time, float duration, int ease = 0);

public:

    // @brief コンポーネントのリスト
    std::list<CComponent*> m_pComponent_List;

protected:
    // @brief 描画パラメータ
    RendererParam m_tParam;

    // @brief 前フレームの位置
    DirectX::XMFLOAT3 m_f3OldPos;

    // @brief オブジェクトが破棄されているかのフラグ
    bool m_bDestroy;

    // @brief オブジェクトのタグ
    Tag m_eTag;

    // @brief オブジェクト識別用ID
    ObjectID m_tID;
    
};

