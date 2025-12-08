/**************************************************//*
	@file	| Animal.h
	@brief	| 動物基底クラスのhファイル
	@note	| 動物の基底クラスを定義
			| CEntityを継承
*//**************************************************/
#include "Entity.h"

// @brief 動物基底クラス
class CAnimal : public CEntity
{
public:
	// @brief コンストラクタ
	CAnimal();

	// @brief デストラクタ
	~CAnimal();

	// @brief 更新処理
	void Update() override;

};

