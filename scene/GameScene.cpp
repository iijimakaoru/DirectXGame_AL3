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

	// 乱数範囲(回転角)
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);

	// 乱数範囲(座標)
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

	for (size_t i = 0; i < _countof(worldTransform); i++) {
		// x,y,z方向のスケーリングを設定
		worldTransform[i].scale_ = {1.0f, 1.0f, 1.0f};

		// x,y,z軸周りの回転角を設定
		worldTransform[i].rotation_ = {rotDist(engine), rotDist(engine), rotDist(engine)};

		// x,y,z軸周りの平行移動を設定
		worldTransform[i].translation_ = {posDist(engine), posDist(engine), posDist(engine)};
	}

	// カメラ視点座標を設定
	viewProjection.eye = {0, 0, -10};

	Update();

	for (size_t i = 0; i < _countof(worldTransform); i++) {
		// ワールドトランスフォームの初期化
		worldTransform[i].Initialize();
	}

	// ビュープロジェクションの初期化
	viewProjection.Initialize();
}

void GameScene::Update() {
	// 視点移動処理
	// 視点の移動ベクトル
	XMFLOAT3 move = {0, 0, 0};

	// 視点の移動の速さ
	const float kEyeSpeed = 0.2f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_W)) {
		move = {0, 0, kEyeSpeed};
	} else if (input_->PushKey(DIK_S)) {
		move = {0, 0, -kEyeSpeed};
	}

	// 視点の移動(ベクトルの加算)
	viewProjection.eye.x += move.x;
	viewProjection.eye.y += move.y;
	viewProjection.eye.z += move.z;

	// 行列の再計算
	viewProjection.UpdateMatrix();

	//// デバッグ用
	// debugText_->SetPos(50, 50);
	// debugText_->Printf(
	//   "eye(%f,%f,%f)", viewProjection.eye.x, viewProjection.eye.y, viewProjection.eye.z);
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
		model->Draw(worldTransform[i], viewProjection, textureHandle);
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
