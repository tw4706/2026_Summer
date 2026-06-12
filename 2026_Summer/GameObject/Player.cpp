#include "Player.h"
#include"Input.h"
#include"Camera.h"
#include"Matrix4x4.h"
#include<Dxlib.h>

namespace
{
	//初期位置
	const Vector3 kFirstPos = { 0.0f, 0.0f, 0.0f };

	//初期スケール
	const Vector3 kFirstScale = { 1.0f, 1.0f, 1.0f };

	//初期回転角度
	const Vector3 kFirstRatate = { 0.0f, DX_PI_F, 0.0f };

	//プレイヤーの移動速度
	constexpr float kSpeed = 10.0f;

	//入力のしきい値
	constexpr float kInputEpsilon = 0.01f;

	//アナログスティックのデッドゾーン
	constexpr float kStickDeadZone = 0.15f;

	//停止する際の線形補間
	constexpr float kStopLerp = 0.2f;

	//カメラの回転速度
	constexpr float kCameraSpeed = 0.03f;
	constexpr float kCameraPitch = 0.3f;

	constexpr float kRotateLerpAnalogStick = 0.3f;

	//移動時の線形補間
	constexpr float kMoveLerp = 0.15f;

	//移動の入力のしきい値
	constexpr float kRunEpsilon = 0.1f;
}

Player::Player() :
	GameObject(pos_, vel_, dir_),
	moveAngle_(0.0f),
	modelH_(-1),
	katanaH_(-1),
	handFrameIndex_(-1),
	state_(PlayerState::Idle)
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

	MV1SetRotationXYZ(modelH_, kFirstRatate.ToDxlibVector());

	//モデルの位置のセット
	MV1SetPosition(modelH_, kFirstPos.ToDxlibVector());


	//アニメーションの初期化
	animation_.Init(modelH_, AnimType::Player);
	animation_.ChangeState(AnimationState::Idle);
}

void Player::Update(Input& input)
{
	//移動処理
	Move(input);

	//ジャンプ処理
	Jump(input);

	//攻撃処理
	Attack(input);

	//状態遷移の更新
	UpdateState();

	//アニメーションの更新
	UpdateAnimation(1.0f / 60.0f);

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
		if (state_ == PlayerState::Run)
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

void Player::Move(Input& input)
{
	//入力から方向ベクトルを作る 
	Vector3 inputDir = { 0.0f, 0.0f, 0.0f };
	if (input.IsPressed("up"))    inputDir.z_ += 1.0f; //前
	if (input.IsPressed("down"))  inputDir.z_ -= 1.0f; //後
	if (input.IsPressed("left"))  inputDir.x_ -= 1.0f; //左
	if (input.IsPressed("right")) inputDir.x_ += 1.0f; //右

	//攻撃時は動かさない
	if (state_ == PlayerState::Attack)
	{
		vel_ = { 0.0f,0.0f,0.0f };
		return;
	}

	bool isKeyboardMoving = (fabs(inputDir.x_) > 0.01f || fabs(inputDir.z_) > kInputEpsilon);

	//キーボードの入力があるとき
	if (isKeyboardMoving)
	{
		float cameraYaw = pCamera_ ? pCamera_->GetYaw() : 0.0f;

		Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);
		Vector3 playerDir = rotMat.TransformForVector(-inputDir).Normalize();

		//キーボード移動もLerpを効かせる
		Vector3 targetVel = playerDir * kSpeed;
		vel_.x_ = Vector3::Lerp(vel_.x_, targetVel.x_, kMoveLerp);
		vel_.z_ = Vector3::Lerp(vel_.z_, targetVel.z_, kMoveLerp);

		//向きの更新
		float playerAngle = atan2f(playerDir.x_, -playerDir.z_);
		float diff = playerAngle - moveAngle_;

		while (diff > DX_PI_F) diff -= DX_PI_F * 2;
		while (diff < -DX_PI_F) diff += DX_PI_F * 2;

		moveAngle_ += diff * kRotateLerpAnalogStick; //回転も滑らかに
	}
	//キーボード入力がないときはアナログスティックの更新
	else
	{
		UpdateAnalogStick(input);
	}

	//カメラの回転
	if (pCamera_)
	{
		Vector3 stickR = input.GetStickRight();
		pCamera_->AddRotation(-stickR.x_ * kCameraSpeed, -stickR.z_ * kCameraSpeed);
	}

	//位置の反映
	pos_ += vel_;
}

void Player::Jump(Input& input)
{
	if (isGround_ && input.IsTriggered("jump"))
	{
		vel_.y_ = jumpPower_;
		isGround_ = false;
	}

	//空中にいる時の処理
	if (!isGround_)
	{
		vel_.y_ -= gravity_; //重力を加算


		//地面との接地判定
		if (pos_.y_ + vel_.y_ <= 0.0f)
		{
			pos_.y_ = 0.0f;     //地面固定
			vel_.y_ = 0.0f;     //速度をゼロにする
			isGround_ = true;   //フラグを戻す
		}
	}
}

void Player::Attack(Input& input)
{

	if (input.IsTriggered("attack"))
	{
		isAttack_ = true;
	}
	if (animation_.IsEnd())
	{
		isAttack_ = false;
	}
}

void Player::UpdateAnalogStick(Input& input)
{
	Vector3 stickL = input.GetStickLeft();

	if (stickL.LengthSq() > kStickDeadZone)
	{
		float cameraYaw = pCamera_ ? pCamera_->GetYaw() : 0.0f;

		Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);
		Vector3 playerDir = rotMat.TransformForVector(-stickL).Normalize();

		//速度変化の影響を受けないよう、入力方向から先に角度を計算
		float playerAngle = atan2f(playerDir.x_, -playerDir.z_);
		float diff = playerAngle - moveAngle_;

		while (diff > DX_PI_F) diff -= DX_PI_F * 2.0f;
		while (diff < -DX_PI_F) diff += DX_PI_F * 2.0f;

		moveAngle_ += diff * kRotateLerpAnalogStick;

		//速度の線形補間
		Vector3 targetVel = playerDir * kSpeed;
		vel_.x_ = Vector3::Lerp(vel_.x_, targetVel.x_, kMoveLerp);
		vel_.z_ = Vector3::Lerp(vel_.z_, targetVel.z_, kMoveLerp);
	}
	else
	{
		//入力がない場合は減速させる
		vel_.x_ = Vector3::Lerp(vel_.x_, 0.0f, kStopLerp);
		vel_.z_ = Vector3::Lerp(vel_.z_, 0.0f, kStopLerp);

		//完全に停止させる
		if (vel_.LengthSq() < 0.01f) {
			vel_ = { 0.0f, 0.0f, 0.0f };
		}
	}
}

void Player::UpdateState()
{
	if (!isGround_)
	{
		state_ = PlayerState::Jump;
		return;
	}

	if (isAttack_)
	{
		state_ = PlayerState::Attack;
		return;
	}

	//速度ベクトルを求める
	float speedVec = sqrtf(vel_.x_ * vel_.x_ + vel_.z_ * vel_.z_);

	//速度ベクトルの長さに応じてアニメーションを変更
	if (speedVec > 0.5f)
	{
		state_ = PlayerState::Run;
	}
	else
	{
		state_ = PlayerState::Idle;
	}
}

void Player::UpdateAnimation(float dt)
{
	AnimationState animState = AnimationState::Idle;

	//プレイヤーの状態とアニメーションの状態を紐づける
	switch (state_)
	{
	case PlayerState::Idle:		animState = AnimationState::Idle; break;
	case PlayerState::Run:		animState = AnimationState::Run; break;
	case PlayerState::Jump:		animState = AnimationState::Jump; break;
	case PlayerState::Attack:	animState = AnimationState::Attack; break;
	}

	if (animation_.GetState() != animState)
	{
		animation_.ChangeState(animState);
	}

	animation_.Update(dt);
}
