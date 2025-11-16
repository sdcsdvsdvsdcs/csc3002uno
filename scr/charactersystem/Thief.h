// Thief.h
#pragma once
#include "Role.h"

class Thief : public Role {
public:
    Thief();
    SkillResult useSkill(Player& user, GameState& game, Player* target = nullptr) override;
};
