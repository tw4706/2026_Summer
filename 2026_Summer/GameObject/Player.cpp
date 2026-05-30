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

	//モデルのロード
	modelH_ = MV1LoadModel("data/Player.mv1");

	katanaH_ = MV1LoadModel("data/Tachi.mv1");
	handFrameIndex_ = MV1SearchFrame(modelH_, "mixamorig:RightHand");

	//モデルの位置のセット
	MV1SetPosition(modelH_, kFirstPos.ToDxlibVector());

	//モデルの拡大率
	MV1SetScale(modelH_, kFirstScale.ToDxlibVector());

	//アニメーションの初期化
	animation_.Init(modelH_, AnimType::Player);
	animation_.ChangeState(AnimationState::Idle);
}

void Player::Update(Input& input)
{
	//移動
	Move(input);

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
		// 1. 右手フレームのワールド行列を取得
		MATRIX handMatrix = MV1GetFrameLocalWorldMatrix(modelH_, handFrameIndex_);

		// 2. 刀用のベースとなるトランスフォーム変数を設定 (現在のサイズ 2.0f を維持)
		Vector3 katanaScale = { 2.0f, 2.0f, 2.0f };
		Vector3 katanaRot = { 0.0f, 0.0f, DX_PI_F/4.0f }; // 基本の回転度数
		Vector3 katanaTrans = { 0.0f, 0.0f, 0.0f }; // 位置の微調整用

		// 3. 【状態による分岐】Idle状態のときだけ特定の軸を90度曲げる
		// ※ ここでは例として X 軸を回しています。モデルの向きに合わせて X, Y, Z のどこを回すか選んでください。
		if (state_ == PlayerState::Idle)
		{
			katanaRot.z_ = DX_PI_F / 1.5f; // 90度（ラジアン）
		}
		else if (state_ == PlayerState::Run)
		{
			katanaRot.z_ = DX_PI_F/2.0f;           // 0度（真っ直ぐ）
		}

		// 4. 各オフセット行列の作成
		Matrix4x4 sMat = Matrix4x4::Scale(katanaScale.x_, katanaScale.y_, katanaScale.z_);

		// 回転 (X->Y->Zの順で合成)
		Matrix4x4 rMat = Matrix4x4::RotateX(katanaRot.x_) * Matrix4x4::RotateY(katanaRot.y_) * Matrix4x4::RotateZ(katanaRot.z_);

		Matrix4x4 tMat = Matrix4x4::Translate(katanaTrans.x_, katanaTrans.y_, katanaTrans.z_);

		// 5. オフセット行列の合成 (拡縮 → 回転 → 移動 の順番が鉄則です)
		Matrix4x4 offsetMat = sMat * rMat * tMat;

		// 6. オフセット → 手のワールド行列 の順で合成
		MATRIX finalSwordMatrix = MMult(offsetMat.ToDxLibMatrix(), handMatrix);

		// 7. 刀に設定して描画
		MV1SetMatrix(katanaH_, finalSwordMatrix);
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
	//idle状態またはRun状態の場合
	if (state_ == PlayerState::Idle || state_ == PlayerState::Run)
	{
		//速度ベクトルの平面上の長さを取得
		float speedXZ = sqrtf(vel_.x_ * vel_.x_ + vel_.z_ * vel_.z_);

		if (speedXZ > 0.5f)
		{
			state_ = PlayerState::Run;
		}
		else
		{
			state_ = PlayerState::Idle;
		}
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
	}

	if (animation_.GetState() != animState)
	{
		animation_.ChangeState(animState);
	}

	animation_.Update(dt);
}
