#include "Player.h"
#include"Input.h"
#include"Camera/CameraBase.h"
#include"Matrix4x4.h"
#include "PlayerStateBase.h"
#include "PlayerStateIdle.h"
#include<Dxlib.h>
#include<memory>
#include "Enemy/EnemyBase.h"

namespace
{
	//初期位置
	const Vector3 kFirstPos = { 0.0f, 0.0f, 0.0f };

	//初期スケール
	const Vector3 kFirstScale = { 1.0f, 1.0f, 1.0f };

	//初期回転角度
	const Vector3 kFirstRotate = { 0.0f, DX_PI_F, 0.0f };

	///刀のモデル関連
	//刀の拡大率
	const Vector3 kKatanaScale = { 2.0f, 2.0f, 2.0f };

	//刀の回転
	const Vector3 kKatanaRotate = { 0.0f, -DX_PI_F / 2.0f, DX_PI_F / 2.0f };

	//刀の移動
	const Vector3 kKatanaTransform = { 20.0f, 10.0f, 0.0f };
}

Player::Player() :
	Character(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.0f),
	moveAngle_(0.0f),
	handFrameIndex_(-1)
{
}

Player::~Player()
{
	Character::~Character();
}

void Player::Init()
{

	//ポジションと速度の初期化
	pos_ = kFirstPos;

	vel_ = { 0.0f,0.0f,0.0f };

	isGround_ = true;

	//モデルのロード
	model_.Load("data/Player.mv1");
	katanaModel_.Load("data/Tachi.mv1");
	handFrameIndex_ = model_.SearchFrame("mixamorig:RightHand");

	//アニメーションの初期化
	animation_.Init(model_.GetHandle(), AnimType::Player);
	animation_.ChangeState(AnimationState::Idle);
}

void Player::Update()
{
	if (!pCurrentState_ && pCamera_ && pInput_)
	{
		auto sharedSelf = std::dynamic_pointer_cast<Player>(shared_from_this());
		std::weak_ptr<Player> weakSelf = sharedSelf;

		//今持っているポインタを参照する
		CameraBase& camera = *pCamera_;
		Input& input = *pInput_;

		// ここは new のままで安全に生成
		pCurrentState_ = std::shared_ptr<PlayerStateIdle>(new PlayerStateIdle(weakSelf, input, camera));

		pCurrentState_->Enter();
	}

	if (pCurrentState_)
	{
		pCurrentState_->Update();
	}

	animation_.Update(1.0f / 60.0f);

	//行列を作成
	//拡縮
	Matrix4x4 scaleMat = Matrix4x4::Scale(kFirstScale.x_, kFirstScale.y_, kFirstScale.z_);
	//Y軸回転
	Matrix4x4 rotMat = Matrix4x4::RotateY(moveAngle_);
	//移動
	Matrix4x4 transMat = Matrix4x4::Translate(pos_.x_, pos_.y_, pos_.z_);

	//行列の合成
	Matrix4x4 worldMat = scaleMat * rotMat * transMat;

	//モデルに行列をセット
	model_.SetMatrix(worldMat);

	DrawFormatString(0, 0, GetColor(255, 255, 255), "Hand Index: %d", handFrameIndex_);
}

void Player::Draw()
{
	model_.Draw();

	if (katanaModel_.GetHandle() != -1 && handFrameIndex_ != -1)
	{
		//右手フレームのワールド行列を取得
		MATRIX handMat = MV1GetFrameLocalWorldMatrix(model_.GetHandle(), handFrameIndex_);

		//刀の行列を定義
		Vector3 katanaScale = kKatanaScale;
		Vector3 katanaRotate = kKatanaRotate;
		Vector3 katanaTransform = kKatanaTransform;

		//状態によって刀の角度を変更
		if (animation_.GetState() == AnimationState::Run)
		{
			katanaRotate.x_ += DX_PI_F / 4.0f;
			katanaRotate.z_ += DX_PI_F / 3.0f;
		}

		//各行列の作成
		Matrix4x4 scaleMat = Matrix4x4::Scale(katanaScale.x_, katanaScale.y_, katanaScale.z_);

		//回転 (X->Y->Zの順で合成)
		Matrix4x4 rotMat = Matrix4x4::RotateX(katanaRotate.x_) * Matrix4x4::RotateY(katanaRotate.y_) * Matrix4x4::RotateZ(katanaRotate.z_);

		Matrix4x4 transMat = Matrix4x4::Translate(katanaTransform.x_, katanaTransform.y_, katanaTransform.z_);

		//合成
		Matrix4x4 mat = scaleMat * rotMat * transMat;

		//最終的な刀の位置の合成
		MATRIX swordMat = MMult(mat.ToDxLibMatrix(), handMat);

		//刀に設定して描画
		katanaModel_.SetMatrix(swordMat);
		katanaModel_.Draw();
	}
}

Vector3 Player::GetCameraTarget() const
{
	return pos_;
}

void Player::ChangeState(std::shared_ptr<CharacterStateBase> nextState)
{
	Character::ChangeState(nextState);
}

void Player::ChangeAnimation(AnimationState state)
{
	Character::ChangeAnimation(state);
}
