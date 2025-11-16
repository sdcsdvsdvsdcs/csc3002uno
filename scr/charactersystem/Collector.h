// Collector.h
#pragma once
#include "Role.h"

class Collector : public Role {
public:
    Collector();
    SkillResult useSkill(Player& user, GameState& game, Player* target = nullptr) override;
};
