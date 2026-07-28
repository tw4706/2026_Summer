#pragma once
#include "EnemyAttackType.h"

struct AttackData
{
	//攻撃ID
	int id_ = 0;

	//攻撃の種類
	AttackType type_ = AttackType::NormalAttack;

	//予備動作から攻撃に移行するアニメーションのフレーム
	float attackTransFrame_ = 0.0f;

	//攻撃コライダーの有効となる開始フレーム
	float attackColliderStartFrame_ = 0.0f;

	//攻撃コライダーの無効となる終了フレーム
	float attackColliderEndFrame_ = 0.0f;

	//攻撃コライダーの半径
	float colliderRadius_ = 0.0f;

	//攻撃コライダーの高さ
	float colliderHeight_ = 0.0f;

	//ダメージ
	float damage_ = 0.0f;

	//攻撃の重み(優先度や確率みたいな感じ)
	float weight_ = 1.0f;
};