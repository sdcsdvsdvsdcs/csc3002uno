// LuckyStar.h
#pragma once
#include "Role.h"

class LuckyStar : public Role {
public:
    LuckyStar();
    SkillResult useSkill(Player& user, GameState& game, Player* target = nullptr) override;
};
