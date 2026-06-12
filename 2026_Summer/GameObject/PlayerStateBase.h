#pragma once
#include<memory>
#include<vector>
class Player;
class PlayerStateBase
{
public:
	PlayerStateBase(std::weak_ptr<Player>pPlayer);
	virtual ~PlayerStateBase();

	virtual void Init()abstract;
	virtual void Update()abstract;
	virtual void End()abstract;
protected:
	std::weak_ptr<Player>pPlayer_;
};

