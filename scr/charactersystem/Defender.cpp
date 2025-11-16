// Defender.cpp
#include "Defender.h"
#include "GameState.h"
#include "Player.h"
#include <iostream>
#include <algorithm>

// 防御者构造函数
Defender::Defender() : Role("防御者", "被动抵挡其他玩家的技能", true) {
    canDefendThisRound = true;
    maxCooldown = 1;      // 成功防御后冷却1回合
}

// 防御者不能主动使用技能
bool Defender::canUseSkill(const Player& user, const GameState& game) const {
    return false; // 被动技能，永远不能主动使用
}

// 防御者不能主动使用技能
SkillResult Defender::useSkill(Player& user, GameState& game, Player* target) {
    SkillResult result;
    result.success = false;
    result.message = "防御者技能是被动技能，不能主动使用！";
    return result;
}

// 更新冷却状态
void Defender::updateCooldown() {
    Role::updateCooldown();
    // 冷却结束后重置防御状态
    if (cooldown == 0) {
        resetDefense();
    }
}

// 尝试防御技能（由游戏系统调用）
bool Defender::tryDefend(Player& attacker, Player& defender, GameState& game) {
    // 检查是否可以防御
    if (!canDefendThisRound || cooldown > 0) {
        return false;
    }
    
    // 检查是否为可防御的技能类型
    if (!isDefendableSkill(attacker.getCurrentSkill())) {
        return false;
    }
    
    std::cout << "【防御者技能发动】" << defender.getName() << "抵挡了" 
              << attacker.getName() << "的技能！" << std::endl;
    
    // 向所有玩家揭示防御者身份
    game.revealRole(defender);
    
    // 通知所有玩家
    game.broadcastMessage(defender.getName() + " 的防御者技能抵挡了 " + 
                         attacker.getName() + " 的技能！");
    
    // 防御者进入冷却
    canDefendThisRound = false;
    cooldown = maxCooldown;
    
    return true;
}

// 每回合开始时重置防御状态
void Defender::resetDefense() {
    canDefendThisRound = true;
}

// 检查技能是否可防御
bool Defender::isDefendableSkill(const std::string& skillName) {
    // 不能防御游戏规则效果和非指向性全局技能
    std::vector<std::string> undefendableSkills = {
        "draw_two", "wild_draw_four", "global_skill", ""
    };
    
    // 空技能名表示不是角色技能
    if (skillName.empty()) {
        return false;
    }
    
    return std::find(undefendableSkills.begin(), 
                    undefendableSkills.end(), skillName) == undefendableSkills.end();
}
