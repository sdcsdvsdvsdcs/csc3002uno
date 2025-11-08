#pragma once
#include "Player.h"

namespace UNO {

/**
 * 人类玩家类
 * 处理人类玩家的输入和决策
 */
class HumanPlayer : public Player {
public:
    HumanPlayer(int id, const std::string& name);
    
    // 游戏操作实现
    std::shared_ptr<Card> playTurn(GameState& gameState) override;
    bool decideUseSkill(GameState& gameState) override;
    nlohmann::json getSkillParameters(GameState& gameState) override;
    CardColor chooseWildColor(GameState& gameState) override;

    // UI交互方法
    void setCardChoice(int cardIndex);
    void setSkillUsage(bool useSkill);
    void setSkillParameters(const nlohmann::json& params);
    void setWildColorChoice(CardColor color);

private:
    // 用户输入状态（由UI设置）
    int selectedCardIndex;
    bool useSkillDecision;
    nlohmann::json skillParams;
    CardColor wildColorChoice;
    bool inputReceived;
    
    void waitForInput();
    void resetInputState();
};

} // namespace UNO
