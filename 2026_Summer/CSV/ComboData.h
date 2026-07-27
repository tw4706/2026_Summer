#pragma once
#include<string>

struct ComboData
{
    std::wstring animName;                      //アニメーション名
    float effectStartFrame = 0.0f;              //エフェクト開始フレーム
    float effectEndFrame = 0.0f;                //エフェクト終了フレーム
    float colliderStartFrame = 0.0f;            //当たり判定開始フレーム
    float colliderEndFrame = 0.0f;              //当たり判定終了フレーム
    float inputAcceptStartFrame = 0.0f;         //次のコンボの入力の受付開始フレーム
    float inputAcceptEndFrame = 0.0f;           //次のコンボの受付終了フレーム
    float comboTransitionFrame = 0.0f;          //次のコンボへ遷移可能なフレーム
};