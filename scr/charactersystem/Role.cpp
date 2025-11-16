// Role.cpp
#include "Role.h"
#include "SkillUIManager.h"

// 角色基类构造函数
Role::Role(const std::string& n, const std::string& desc, bool passive) 
    : name(n), description(desc), cooldown(0), maxCooldown(1), 
      usesLeft(-1), maxUses(-1), isPassive(passive), uiManager(nullptr) {}

// 设置UI管理器
void Role::setUIManager(SkillUIManager* manager) {
    uiManager = manager;
}

// 检查技能是否可以使用
bool Role::canUseSkill(const Player& user, const GameState& game) const {
    // 被动技能不能主动使用
    if (isPassive) return false;
    // 检查冷却时间和使用次数
    return cooldown == 0 && (usesLeft == -1 || usesLeft > 0);
}

// 更新冷却状态（每回合结束时调用）
void Role::updateCooldown() {
    if (cooldown > 0) cooldown--;
}

// 获取角色信息
std::string Role::getInfo() const {
    std::string info = name + ": " + description;
    info += "\n冷却: " + std::to_string(cooldown) + "回合";
    info += "\n剩余次数: " + (usesLeft == -1 ? "无限" : std::to_string(usesLeft));
    return info;
}

// Getter方法
std::string Role::getName() const { return name; }
int Role::getCooldown() const { return cooldown; }
int Role::getUsesLeft() const { return usesLeft; }
bool Role::getIsPassive() const { return isPassive; }
