#pragma once
#include "GameObject.h"
#include <memory>

class CharacterStateBase;

class Character : public GameObject,public std::enable_shared_from_this<Character>
{
public:
    Character(Vector3 pos, Vector3 vel, float dir);
    virtual ~Character();

    virtual void Init()abstract;
    virtual void Update()abstract;
    virtual void Draw()abstract;

    /// <summary>
    /// 状態の切り替え
    /// </summary>
    virtual void ChangeState(std::shared_ptr<CharacterStateBase> pNextState);

protected:
    int hp_;
    std::shared_ptr<CharacterStateBase> pCurrentState_=nullptr;//現在のステートを管理するポインタ
};

