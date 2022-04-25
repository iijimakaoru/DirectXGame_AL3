#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() { delete model; }

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	// ファイル名を指定してテクスチャを読み込み
	textureHandle = TextureManager::Load("mario.jpg");

	// 3Dモデルの生成
	model = Model::Create();

	// 乱数シード生成器
	std::random_device seed_gen;

	// メルセンヌ・ツイスター
	std::mt19937_64 engine(seed_gen());

	defVec = {0, 0, 1};

	centerVec = defVec;

	for (size_t i = 0; i < _countof(worldTransform); i++) {
		// x,y,z方向のスケーリングを設定
		worldTransform[0].scale_ = {4.0f, 4.0f, 4.0f};
		worldTransform[1].scale_ = {0.5f, 0.5f, 0.5f};

		// x,y,z軸周りの回転角を設定
		worldTransform[0].rotation_ = {0, 0, 0};
		worldTransform[1].rotation_ = {0, 0, 0};

		// x,y,z軸周りの平行移動を設定
		worldTransform[0].translation_ = {0, 0, 0};
		worldTransform[1].translation_ = {0, 0, 3};

		// ワールドトランスフォームの初期化
		worldTransform[0].Initialize();

		worldTransform[1].parent_ = &worldTransform[0];
		worldTransform[1].Initialize();
	}

	viewProjection.eye.y = 20;

	// ビュープロジェクションの初期化
	viewProjection.Initialize();
}

void GameScene::Update() {
	// 物体の移動
	// 移動ベクトル
	XMFLOAT3 bMove = {0, 0, 0};

	// 方向ベクトル
	XMFLOAT3 vMove = {0, 0, 0};

	// 速さ
	const float kBodySpeed = 1.0f;
	const float kVecSpeed = 0.02f;

	// 押した方向に移動
	if (input_->PushKey(DIK_UP)) {
		bMove.z = kBodySpeed * (centerVec.z);
		bMove.x = kBodySpeed * (centerVec.x);
	} else if (input_->PushKey(DIK_DOWN)) {
		bMove.z = -kBodySpeed * (centerVec.z);
		bMove.x = -kBodySpeed * (centerVec.x);
	}

	// 方向転換
	if (input_->PushKey(DIK_RIGHT)) {
		worldTransform[0].rotation_.y += kVecSpeed;
		centerVec.x = sinf(worldTransform[0].rotation_.y);
		centerVec.z = cosf(worldTransform[0].rotation_.y);
	} else if (input_->PushKey(DIK_LEFT)) {
		worldTransform[0].rotation_.y -= kVecSpeed;
		centerVec.x = sinf(worldTransform[0].rotation_.y);
		centerVec.z = cosf(worldTransform[0].rotation_.y);
	}

	worldTransform[0].translation_.z += bMove.z;
	worldTransform[0].translation_.x += bMove.x;
	
	// 視点移動処理
	// 視点の移動ベクトル
	XMFLOAT3 move = {0, 0, 0};

	// 視点の移動の速さ
	const float kEyeSpeed = 0.2f;

	for (size_t i = 0; i < _countof(worldTransform); i++) {
		if (input_->PushKey(DIK_Z)) {
			worldTransform[i].translation_ = {0, 0, 0};
		}
	}

	// 行列の再計算
	worldTransform[0].UpdateMatrix();
	worldTransform[1].UpdateMatrix();
	viewProjection.UpdateMatrix();

	// デバッグ用
	debugText_->SetPos(50, 50);
	debugText_->Printf("centerVec(%f,%f,%f)", centerVec.x, centerVec.y, centerVec.z);
	debugText_->SetPos(50, 70);
	debugText_->Printf(
	  "rotateD(%f, %f, %f)", XMConvertToDegrees(worldTransform[0].rotation_.x),
	  XMConvertToDegrees(worldTransform[0].rotation_.y),
	  XMConvertToDegrees(worldTransform[0].rotation_.z));
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	for (size_t i = 0; i < _countof(worldTransform); i++) {
		model->Draw(worldTransform[0], viewProjection, textureHandle);
		model->Draw(worldTransform[1], viewProjection, textureHandle);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}