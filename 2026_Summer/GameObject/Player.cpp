#include "Player.h"
#include"Input.h"
#include"Camera.h"
#include"Matrix4x4.h"
#include "PlayerStateBase.h"
#include "PlayerStateIdle.h"
#include<Dxlib.h>
#include<memory>

namespace
{
	//初期位置
	const Vector3 kFirstPos = { 0.0f, 0.0f, 0.0f };

	//初期スケール
	const Vector3 kFirstScale = { 1.0f, 1.0f, 1.0f };

	//初期回転角度
	const Vector3 kFirstRotate = { 0.0f, DX_PI_F, 0.0f };
}

Player::Player() :
	Character(Vector3{ 0.0f,0.0f,0.0f }, Vector3{0.0f,0.0f,0.0f},0.0f),
	moveAngle_(0.0f),
	modelH_(-1),
	katanaH_(-1),
	handFrameIndex_(-1)
{
}

Player::~Player()
{
	//モデルの削除
	if (modelH_ != -1) MV1DeleteModel(modelH_);
	if (katanaH_ != -1) MV1DeleteModel(katanaH_);
}

void Player::Init()
{

	//ポジションと速度の初期化
	pos_ = kFirstPos;

	vel_ = { 0.0f,0.0f,0.0f };

	isGround_ = true;

	//モデルのロード
	modelH_ = MV1LoadModel("data/Player.mv1");

	katanaH_ = MV1LoadModel("data/Tachi.mv1");
	handFrameIndex_ = MV1SearchFrame(modelH_, "mixamorig:RightHand");

	//モデルの拡大率
	MV1SetScale(modelH_, kFirstScale.ToDxlibVector());

	//モデルの回転
	MV1SetRotationXYZ(modelH_, kFirstRotate.ToDxlibVector());

	//モデルの位置のセット
	MV1SetPosition(modelH_, kFirstPos.ToDxlibVector());

	//アニメーションの初期化
	animation_.Init(modelH_, AnimType::Player);
	animation_.ChangeState(AnimationState::Idle);
}

void Player::Update(Input& input)
{
	if (!pCurrentState_ && pCamera_)
	{
		auto sharedSelf = std::dynamic_pointer_cast<Player>(shared_from_this());
		std::weak_ptr<Player> weakSelf = sharedSelf;

		Camera& cameraRef = *pCamera_;

		// ここは new のままで安全に生成
		pCurrentState_ = std::shared_ptr<PlayerStateIdle>(new PlayerStateIdle(weakSelf, input, cameraRef));

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
	MV1SetMatrix(modelH_, worldMat.ToDxLibMatrix());

	DrawFormatString(0, 0, GetColor(255, 255, 255), "Hand Index: %d", handFrameIndex_);
}

void Player::Draw()
{
	//モデルの描画
	MV1DrawModel(modelH_);

	if (katanaH_ != -1 && handFrameIndex_ != -1)
	{
		//右手フレームのワールド行列を取得
		MATRIX handMat = MV1GetFrameLocalWorldMatrix(modelH_, handFrameIndex_);

		//刀の行列を定義
		Vector3 katanaScale = { 2.0f, 2.0f, 2.0f };
		Vector3 katanaRotate = { 0.0f, -DX_PI_F / 2.0f, DX_PI_F / 2.0f };
		Vector3 katanaTransform = { 4.0f, 10.0f, 0.0f };

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
		MV1SetMatrix(katanaH_, swordMat);
		MV1DrawModel(katanaH_);
	}
}

Vector3 Player::GetCameraTarget() const
{
	return pos_;
}

void Player::AddPosition(const Vector3& offset)
{
	pos_ += offset;
}

void Player::ChangeState(std::shared_ptr<PlayerStateBase> nextState)
{
	if (!nextState) return;

	if (pCurrentState_)
	{
		pCurrentState_->Exit();
	}

	pCurrentState_ = nextState;
	pCurrentState_->Enter();
}


void Player::ChangeAnimation(AnimationState state)
{
	if (animation_.GetState() != state)
	{
		animation_.ChangeState(state);
	}
}
