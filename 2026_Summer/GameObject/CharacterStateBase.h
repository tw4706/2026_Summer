#pragma once
#include <memory>

class Character;
class CharacterStateBase
{
public:
    CharacterStateBase(std::weak_ptr<Character> pCharacter);

    virtual ~CharacterStateBase() = default;

    virtual void Enter()abstract;
    virtual void Update()abstract;
    virtual void Exit()abstract;

protected:
    std::weak_ptr<Character> pCharacter_;
};

