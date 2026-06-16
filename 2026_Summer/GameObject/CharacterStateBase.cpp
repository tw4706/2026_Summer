#include "CharacterStateBase.h"

CharacterStateBase::CharacterStateBase(std::weak_ptr<Character> pCharacter):
	pCharacter_(pCharacter)
{
}
