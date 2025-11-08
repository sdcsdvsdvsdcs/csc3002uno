#pragma once
#include "Player.h"
#include <algorithm>
#include <random>

namespace UNO {

/**
 * AI玩家类
 * 实现简单的AI策略：优先出数字牌，有牌就出，没牌就抽牌
 */
class AIPlayer : public Player {
public:
    AIPlayer(int id, const std::string& name = "AI Player");
    
    // 游戏操作实现
    std::shared_ptr<Card> playTurn(GameState& gameState) override;
    bool decideUseSkill(GameState& gameState) override;
    nlohmann::json getSkillParameters(GameState& gameState) override;
    CardColor chooseWildColor(GameState& gameState) override;

private:
    std::mt19937 randomGenerator;
    
    // AI策略方法
    std::shared_ptr<Card> selectCardToPlay(const GameState& gameState);
    CardColor selectColorForWildCard(const GameState& gameState);
    
    // 辅助方法
    bool isNumberCard(const std::shared_ptr<Card>& card) const;
    std::vector<std::shared_ptr<Card>> getPlayableNumberCards(const GameState& gameState) const;
    std::vector<std::shared_ptr<Card>> getPlayableFunctionCards(const GameState& gameState) const;
    CardColor getMostFrequentColorInHand() const;
};

} // namespace UNO
