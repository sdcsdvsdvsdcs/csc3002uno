// Defender.h
#pragma once
#include "Role.h"

class Defender : public Role {
private:
    bool canDefendThisRound;

public:
    Defender();
    
    // 防御者重写基类方法
    bool canUseSkill(const Player& user, const GameState& game) const override;
    SkillResult useSkill(Player& user, GameState& game, Player* target = nullptr) override;
    void updateCooldown() override;
    
    // 防御者特有方法
    bool tryDefend(Player& attacker, Player& defender, GameState& game);
    void resetDefense();
    
private:
    bool isDefendableSkill(const std::string& skillName);
};
