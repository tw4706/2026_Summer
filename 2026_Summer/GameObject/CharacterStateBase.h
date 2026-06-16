#pragma once
#include <memory>

class Character;
class CharacterStateBase
{
public:
    CharacterStateBase(Character* pCharacter);

    virtual ~CharacterStateBase() = default;

    virtual void Enter()abstract;
    virtual void Update()abstract;
    virtual void Exit()abstract;

protected:
    Character* pCharacter_;
};

