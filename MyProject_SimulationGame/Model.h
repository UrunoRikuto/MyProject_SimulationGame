/**********************************************************************************//*
	@file		|Model.h
	@brief		|3Dモデル管理
*//***********************************************************************************/
#pragma once
#include <DirectXMath.h>
#include <vector>
#include "Shader.h"
#include "MeshBuffer.h"
#include <functional>

#ifdef _DEBUG
#define MODEL_FORCE_ERROR (1) // エラーメッセージ強制表示
#else
#define MODEL_FORCE_ERROR (0)
#endif

// @brief 3Dモデル管理
class Model
{
public:
	// モデル反転設定
	enum Flip
	{
		None,			// DirectX準拠(反転する
		XFlip,			// Maya準拠
		ZFlip,			// DirectX準拠(Mayaから180回転した状態
		ZFlipUseAnime,	// DirecX準拠(アニメーションさせる場合　※動作チェック不十分
	};

private:
	// アニメーション再生方法
	enum AnimePattern
	{
		MAIN,			// 通常再生
		BLEND,			// ブレンド再生
		PARAMETRIC0,	// 合成A
		PARAMETRIC1,	// 合成B
		MAX_ANIMEPATTERN
	};

public:
	// 型定義
	using NodeIndex	= int;	// ボーン(階層)番号
	using MorphNo	= int;	// モーフデータ番号
	using AnimeNo	= int;	// アニメーション番号

	// 定数定義
	static const NodeIndex	NODE_NONE			= -1;	// 該当ノードなし
	static const MorphNo	MORPH_NONE			= -1;	// 該当モーフなし
	static const AnimeNo	ANIME_NONE			= -1;	// 該当アニメーションなし
	static const AnimeNo	PARAMETRIC_ANIME	= -2;	// 合成アニメーション

private:
	// 内部型定義
	using AnimeTime = float;
	using Children	= std::vector<NodeIndex>;	// ノード階層情報

	// 内部定数定義
	static const UINT	MAX_BONE	= 200;	// １メッシュの最大ボーン数(ここを変更する場合.hlsl側の定義も変更する
	static const UINT	MAX_WEIGHT	= 4;	// １頂点に割り当てられる最大ボーン数

	// 階層情報
	struct Node
	{
		std::string			name;		// 階層名
		NodeIndex			parent;		// 親ボーン
		Children			children;	// 子ボーン
		DirectX::XMMATRIX	mat;		// アニメーション用変換行列
	};
	using Nodes = std::vector<Node>;

	// アニメーション再生情報
	struct AnimePlayInfo
	{
		AnimeTime		nowTime;	// 現在の再生時間
		AnimeTime		totalTime;	// 最大再生時間
		float			speed;		// 再生速度
		bool			isLoop;		// ループ指定
	};

	//===== アニメーション =====
	// アニメーションの変換情報
	struct AnimeTransform
	{
		DirectX::XMFLOAT3	translate;
		DirectX::XMFLOAT4	quaternion;
		DirectX::XMFLOAT3	scale;
	};
	using AnimeKey			= std::pair<AnimeTime, AnimeTransform>;
	using AnimeTimeline		= std::map<AnimeTime, AnimeTransform>;
	using AnimeTransforms	= std::vector<AnimeTransform>;

	// アニメーションとノードの関連付け情報
	struct AnimeChannel
	{
		NodeIndex		node;		// 対応するノード
		AnimeTimeline	timeline;	// ノードに付随するアニメーション情報
	};
	using AnimeChannels = std::vector<AnimeChannel>;

	// アニメーション情報
	struct Animation
	{
		AnimePlayInfo	info;		// 再生情報
		AnimeChannels	channels;	// 変換情報
	};
	using Animations = std::vector<Animation>;

	//===== ブレンドシェイプ =====
	// モーフィング用頂点
	struct MorphVertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
	};
	using MorphVertices = std::vector<MorphVertex>;

	// モーフィング用メッシュ
	struct MorphMesh
	{
		std::string		name;		// モーフデータ名
		int				meshNo;		// 対応するメッシュ
		float			weight;		// ブレンド割合
		MorphVertices	vertices;	// 変形頂点情報
	};
	using MorphMeshes = std::vector<MorphMesh>;

	// モーフィングアニメーション中のブレンド情報
	using MorphAnimeWeights = std::vector<float>;	// アニメーション中のブレンド割合
	struct MorphTimeline
	{
		AnimeTime			time;		// アニメーション実行時間
		MorphAnimeWeights	weights;	// モーフターゲットごとのブレンド割合
	};
	using MorphTimelines	= std::vector<MorphTimeline>;

	// モーフィングのアニメーション情報
	using MorphAnimeTargets = std::vector<unsigned int>;
	struct MorphAnimation
	{
		AnimePlayInfo		info;		// 再生情報
		MorphAnimeTargets	morphs;		// 再生中に変更するモーフターゲット
		MorphTimelines		timelines;	// アニメーション情報
	};
	using MorphAnimations = std::vector<MorphAnimation>;

public:
	// 頂点情報
	struct Vertex
	{
		DirectX::XMFLOAT3	pos;
		DirectX::XMFLOAT3	normal;
		DirectX::XMFLOAT2	uv;
		DirectX::XMFLOAT4	color;
		float				weight[MAX_WEIGHT];
		unsigned int		index[MAX_WEIGHT];
	};
	using Vertices	= std::vector<Vertex>;
	using Indices	= std::vector<unsigned long>;

	// ボーン情報
	struct Bone
	{
		NodeIndex			nodeIndex;	// 変形情報は階層情報に格納
		DirectX::XMMATRIX	invOffset;	// 逆行列
	};
	using Bones = std::vector<Bone>;

	// メッシュ
	struct Mesh
	{
		NodeIndex		nodeIndex;	// 階層との紐づけ
		Vertices		vertices;	// 頂点情報
		Indices			indices;	// インデックス情報
		unsigned int	materialID;	// 使用するマテリアル
		Bones			bones;		// メッシュに紐づけらているボーン
		MeshBuffer*		pMesh;		// 描画データ
	};
	using Meshes = std::vector<Mesh>;

	// マテリアル情報
	struct Material
	{
		DirectX::XMFLOAT4	diffuse;	// 拡散光(メインカラー
		DirectX::XMFLOAT4	ambient;	// 環境光(陰の部分のカラー
		DirectX::XMFLOAT4	specular;	// 鏡面反射光(強く光る部分のカラー
		Texture*			pTexture;	// テクスチャ
	};
	using Materials = std::vector<Material>;


public:
	Model();
	~Model();

	//========================================
	//     基本処理
	//========================================
	/*
	* @brief リセット
	*/
	void Reset();

	/*
	* @brief 頂点シェーダーの設定
	* @param[in] vs 頂点シェーダー
	*/
	void SetVertexShader(Shader* vs);

	/*
	* @brief ピクセルシェーダーの設定
	* @param[in] ps ピクセルシェーダー
	*/
	void SetPixelShader(Shader* ps);

	/*
	* @brief モデルデータの読み込み
	* @param[in] file 読み込み先パス
	* @param[in] scale 拡大率
	* @param[in] flip 反転設定
	* @return 読み込み結果
	*/
	bool Load(const char* file, float scale = 1.0f, Flip flip = Flip::None);

	/*
	* @brief 更新処理
	* @param[in] tick 経過時間(秒)
	*/
	void Step(float tick);

	/*
	* @brief 描画処理
	* @param[in] meshNo 描画メッシュ番号(-1ですべて表示
	*/
	void Draw(int meshNo = -1);


	//========================================
	//     チェック処理
	//========================================
	/*
	* @brief 読み込み処理実行後のエラーメッセージ取得
	* @return エラーメッセージ
	*/
	static std::string GetError();

	/*
	* @brief ボーンのデバッグ表示
	* @param[in] world ワールド行列
	*/
	void DrawBone(DirectX::XMMATRIX world = DirectX::XMMatrixIdentity());

	//========================================
	//     アニメーション
	//========================================
	/*
	* @brief アニメーションデータの追加読み込み
	* @param[in] file 読み込み先パス
	* @return アニメーション番号
	*/
	AnimeNo AddAnimation(const char* file);

	/*
	* @brief アニメーションの再生
	* @param[in] no 再生するアニメーション番号
	* @param[in] loop ループ再生フラグ
	* @param[in] speed 再生速度
	*/
	void PlayAnime(AnimeNo no, bool loop, float speed = 1.0f);

	/*
	* @brief ブレンド再生
	* @param[in] no アニメーション番号
	* @param[in] blendTime ブレンドに掛ける時間
	* @param[in] loop ループフラグ
	* @param[in] speed 再生速度
	*/
	void PlayBlend(AnimeNo no, AnimeTime blendTime, bool loop, float speed = 1.0f);

	/*
	* @brief アニメーションの合成再生
	* @param[in] no1 合成元１
	* @param[in] no2 合成元２
	*/
	void SetParametric(AnimeNo no1, AnimeNo no2);

	/*
	* @brief 合成割合を指定
	* @param[in] blendRate 0～1の間でno1とno2のアニメを合成(0でno1,1でno2)
	*/
	void SetParametricBlend(float blendRate);

	/*
	* @brief アニメーションの現在の再生時間を変更
	* @param[in] no 変更対象のアニメーション番号
	* @param[in] time 変更後の時間
	*/
	void SetAnimeTime(AnimeNo no, AnimeTime time);



	//========================================
	//     ブレンドシェイプ
	//========================================
	/*
	* @brief モーフデータの追加
	* @param[in] file 読み込み先パス
	* @param[out] out 読み取ったデータに割り当てられたインデックス(複数読み込む可能性があるため
	* @return 読み取り結果
	*/
	bool AddMorph(const char* file, Indices* out = nullptr);

	/*
	* @brief モーフの合成割合を指定
	* @param[in] no 割合を変更するモーフ
	* @param[in] weight 合成割合
	*/
	void SetMorphWeight(MorphNo no, float weight);

	/*
	* @brief モーフアニメーションの追加読み込み
	* @param[in] file 読み込み先パス
	* @return アニメーション番号
	*/
	AnimeNo AddMorphAnime(const char* file);

	/*
	* @brief モーフアニメーションの再生
	* @param[in] no 再生するアニメーション
	* @param[in] loop ループ再生フラグ
	* @param[in] speed 再生速度
	*/
	void PlayMorph(AnimeNo no, bool loop, float speed = 1.0f);


	//========================================
	//     情報取得
	//========================================
	/*
	* @brief メッシュ数を取得
	* @return メッシュ数
	*/
	unsigned int GetMeshNum();

	/*
	* @brief メッシュ情報を取得
	* @param[in] index 取得するメッシュ番号
	* @return メッシュ情報
	*/
	const Mesh* GetMesh(unsigned int index);

	/*
	* @brief マテリアル数を取得
	* @return マテリアル数
	*/
	unsigned int GetMaterialNum();

	/*
	* @brief マテリアル情報を取得
	* @param[in] index 取得するマテリアル番号
	* @return マテリアル情報
	*/
	const Material* GetMaterial(unsigned int index);

	/*
	* @brief アニメーション後の変換行列取得
	* @param[in] index ボーン番号
	* @return 該当ボーンの変換行列
	*/
	DirectX::XMMATRIX GetBoneMatrix(NodeIndex index);
	
	/*
	* @brief アニメーション再生判定
	* @param[in] no アニメーション番号
	* @return 再生状況
	*/
	bool IsAnimePlay(AnimeNo no);

	/*
	* @brief 再生中のアニメ番号取得
	* @return アニメーション番号
	*/
	AnimeNo GetAnimePlayNo();

	/*
	* @brief 再生中のブレンド番号取得
	* @return アニメーション番号
	*/
	AnimeNo GetAnimeBlendNo();

	/*
	* @brief アニメーション情報取得
	* @return アニメーション情報
	*/
	const AnimePlayInfo* GetPlayAnimeInfo();

private:
	//========================================
	//     基本処理
	//========================================
	// assimpでデータを読み込み
	const void* LoadAssimpScene(const char* file);
	// 階層情報の構築
	void MakeNodes(const void* ptr);
	// メッシュの作成
	void MakeMesh(const void* ptr);
	// マテリアルの作成
	void MakeMaterial(const void* ptr, std::string directory);
	// 頂点ブレンドの作成
	void MakeVertexWeight(const void* ptr, int meshIndex);
	// ボーンありの頂点ブレンド作成
	void MakeVertexWeightHasBone(const void* ptr, Mesh& mesh);
	// 親子関係をもとに頂点ブレンド作成
	void MakeVertexWeightFromNode(const void* scene, const void* ptr, Mesh& mesh);

	//========================================
	//     チェック処理
	//========================================
	// アニメーションのループチェック
	void CheckAnimePlayLoop(AnimePlayInfo& info);
	// エラーチェック
	bool IsError(bool condition, std::string message);
	// エラーメッセージの設定
	void SetErrorMessage(std::string message);
	// エラーメッセージの表示
	void ShowErrorMessage(const char* caption, bool isWarning);
	// ノード探索
	NodeIndex FindNode(const char* name);
	// フリーズ済みメッシュのチェック
	bool CheckMeshFreeze(const void* ptr);
	// アニメーション番号のチェック
	bool CheckAnimeNo(AnimeNo no);


	//========================================
	//     アニメーション
	//========================================
	// アニメーションの更新
	void StepAnime(float tick);
	// アニメーションの初期化
	void InitAnime(AnimeNo no);
	// アニメーションの再生情報の更新
	void UpdateAnime(AnimeNo no, float tick);
	// アニメーションの姿勢行列を計算
	void CalcAnime(AnimePattern kind, AnimeNo no);
	// 再生方法別の行列結果の合成
	void CalcBones(NodeIndex index, const DirectX::XMMATRIX parent);
	// 行列結果同士の補間
	void LerpTransform(AnimeTransform* pOut, const AnimeTransform& a, const AnimeTransform& b, float rate);

	//========================================
	//     ブレンドシェイプ
	//========================================
	// モーフのメッシュデータ作成
	void MakeMorphMesh(const void* ptr, int meshIndex, Indices* out);
	// モーフのメッシュに割り当てられる頂点データの作成
	void MakeMorphVertices(MorphVertices& out, const void* ptr);
	// モーフアニメーションのタイムライン作成
	void MakeMorphTimeline(MorphAnimation& anime, const void* ptr);
	// モーフの更新
	void UpdateMorph();
	// モーフアニメーションの更新
	void StepMorph(float tick);
	// モーフ合成時の頂点計算
	void AddMorphVtxWeight(Vertex* out, MorphVertex in, float weight);

	//========================================
	//     情報取得
	//========================================
	// ディレクトリの取得
	std::string GetDirectory(const char* file);

private:
	static VertexShader*	m_pDefVS;		// デフォルト頂点シェーダー
	static PixelShader*		m_pDefPS;		// デフォルトピクセルシェーダー
	static unsigned int		m_shaderRef;	// シェーダー参照数
#ifdef _DEBUG
	static std::string m_errorStr;	
#endif

private:
	float			m_loadScale;	// 読み込み時の拡大率
	Flip			m_loadFlip;		// 読み込み時の反転設定
	VertexShader*	m_pVS;			// 設定中の頂点シェーダ
	PixelShader*	m_pPS;			// 設定中のピクセルシェーダ
	Nodes			m_nodes;		// 階層情報
	Meshes			m_meshes;		// メッシュ配列
	Materials		m_materials;	// マテリアル配列
	
	AnimeTransforms	m_animeTransform[MAX_ANIMEPATTERN];	// アニメーション再生方法別変形情報
	Animations		m_animes;			// アニメ配列
	AnimeNo			m_playNo;			// 現在再生中のアニメ番号
	AnimeNo			m_blendNo;			// ブレンド再生を行うアニメ番号
	AnimeNo			m_parametric[2];	// 合成再生を行うアニメ番号
	AnimeTime		m_blendTime;		// 現在の遷移経過時間
	AnimeTime		m_blendTotalTime;	// アニメ遷移にかかる合計時間
	float			m_parametricBlend;	// パラメトリックの再生割合

	MorphMeshes		m_morphes;		// モーフ配列
	MorphAnimations m_morphAnimes;	// モーフアニメ配列
	AnimeNo			m_morphPlayNo;	// 再生中のモーフアニメーション
};