/**********************************************************************************//*
	@file		| Shader.h
	@brief		| シェーダークラス
*//***********************************************************************************/
#pragma once
#include "DirectX.h"
#include "Texture.h"
#include <string>
#include <map>
#include <vector>

/*************************//*
@brief		|頂点シェーダーの種類
*//*************************/
enum class VSType
{
	None,
	Object,
	Sprite,
	MAX
};

/*************************//*
@brief		|ピクセルシェーダーの種類
*//*************************/
enum class PSType
{
	None,
	TexColor,
	Sprite,
	MAX
};

// @brief シェーダーの基本クラス
class Shader
{
protected:
	// @brief シェーダーの種類
	enum Kind
	{
		Vertex,
		Pixel
	};
protected:
	// @brief コンストラクタ
	// @param[in] kind シェーダーの種類
	Shader(Kind kind);
public:
	// @brief デストラクタ
	virtual ~Shader();
public:
	// @brief シェーダーファイル(*.cso)を読み込む処理
	// @param[in] pFileName ファイル名
	// @return 処理結果
	HRESULT Load(const char* pFileName);

	// @brief 文字列からシェーダをコンパイル
	// @param[in] pCode シェーダコード
	// @return 処理結果
	HRESULT Compile(const char* pCode);

	// @brief 定数の書き込み
	// @param[in] slot 定数バッファのスロット
	// @param[in] pData 書き込むデータ
	void WriteBuffer(UINT slot, void* pData);

	// @brief テクスチャの設定
	// @param[in] slot テクスチャのスロット
	// @param[in] tex テクスチャ
	void SetTexture(UINT slot, Texture* tex);
	
	// @brief テクスチャの設定
	// @param[in] slot テクスチャのスロット
	// @param[in] Resouce シェーダーリソースビュー
	void SetTexture(UINT slot, ID3D11ShaderResourceView* Resouce);

	// @brief シェーダーを描画に使用
	virtual void Bind(void) = 0;

private:
	// @brief シェーダーファイルを読み込んだ後、シェーダーの種類別に処理を行う
	// @param[in] pData シェーダーデータ
	// @param[in] size データサイズ
	// @return 処理結果
	HRESULT Make(void* pData, UINT size);
protected:
	// @brief シェーダーファイルを読み込んだ後、シェーダーの種類別に処理を行う
	// @param[in] pData シェーダーデータ
	// @param[in] size データサイズ
	// @return 処理結果
	virtual HRESULT MakeShader(void* pData, UINT size) = 0;

private:
	// シェーダーの種類
	Kind m_kind;
protected:
	// 定数バッファ
	std::vector<ID3D11Buffer*> m_pBuffers;
	std::vector<ID3D11ShaderResourceView*> m_pTextures;
};

// @brief 頂点シェーダ
class VertexShader : public Shader
{
public:
	// @brief コンストラクタ
	VertexShader(VSType In_eType = VSType::None);

	// @brief デストラクタ
	~VertexShader();

	// @brief シェーダーを描画に使用
	void Bind(void);
protected:
	// @brief シェーダーファイルを読み込んだ後、頂点シェーダ用の処理を行う
	// @param[in] pData シェーダーデータ
	// @param[in] size データサイズ
	// @return 処理結果
	HRESULT MakeShader(void* pData, UINT size);

private:
	// @brief 頂点シェーダオブジェクト
	ID3D11VertexShader* m_pVS;
	// @brief 入力レイアウト
	ID3D11InputLayout* m_pInputLayout;
public:
	// @brief 種類
	VSType m_eType;
};

// @brief ピクセルシェーダ
class PixelShader : public Shader
{
public:
	// @brief コンストラクタ
	PixelShader(PSType In_eType = PSType::None);

	// @brief デストラクタ
	~PixelShader();

	// @brief シェーダーを描画に使用
	void Bind(void);
protected:
	// @brief シェーダーファイルを読み込んだ後、ピクセルシェーダ用の処理を行う
	// @param[in] pData シェーダーデータ
	// @param[in] size データサイズ
	// @return 処理結果
	HRESULT MakeShader(void* pData, UINT size);
private:
	// @brief ピクセルシェーダオブジェクト
	ID3D11PixelShader* m_pPS;
public:
	// @brief 種類
	PSType m_eType;
};
