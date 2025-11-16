// Role.h - 只包含最基础的依赖
#pragma once
#include <string>
#include <memory>

// 前向声明
class Player;
class GameState;
class SkillUIManager;

struct SkillResult {
    bool success;
    std::string message;
};

class Role {
    // 基类定义...
};
