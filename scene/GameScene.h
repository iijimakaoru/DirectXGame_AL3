﻿#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "DebugText.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <DirectXMath.h>

using namespace DirectX;

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

  public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

  private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	// 基準ベクトル
	XMFLOAT3 defVec;

	// 正面用のベクトル
	XMFLOAT3 centerVec;

	// テクスチャハンドル
	uint32_t textureHandle = 0;

	// 3Dモデル
	Model* model = nullptr;

	// ワールドトランスフォーム
	WorldTransform worldTransform[2];

	// ビュープロジェクション
	ViewProjection viewProjection;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
