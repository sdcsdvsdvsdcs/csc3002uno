#include "AIPlayer.h"
#include "GameState.h"
#include <algorithm>
#include <chrono>

namespace UNO {

AIPlayer::AIPlayer(int id, const std::string& name) 
    : Player(id, name, true),
      randomGenerator(std::chrono::system_clock::now().time_since_epoch().count()) {}

std::shared_ptr<Card> AIPlayer::playTurn(GameState& gameState) {
    return selectCardToPlay(gameState);
}

bool AIPlayer::decideUseSkill(GameState& gameState) {
    // 简单策略：暂时不使用技能
    // 后续可以扩展为根据情况使用技能
    return false;
}

nlohmann::json AIPlayer::getSkillParameters(GameState& gameState) {
    // 不使用技能，返回空参数
    return nlohmann::json::object();
}

CardColor AIPlayer::chooseWildColor(GameState& gameState) {
    return selectColorForWildCard(gameState);
}

std::shared_ptr<Card> AIPlayer::selectCardToPlay(const GameState& gameState) {
    // 获取所有可出的牌
    auto playableCards = getPlayableCards(gameState);
    
    if (playableCards.empty()) {
        return nullptr; // 没有可出的牌，需要抽牌
    }
    
    // 策略：优先出数字牌
    auto numberCards = getPlayableNumberCards(gameState);
    if (!numberCards.empty()) {
        // 有数字牌，随机选择一张数字牌
        std::uniform_int_distribution<int> dist(0, numberCards.size() - 1);
        return numberCards[dist(randomGenerator)];
    }
    
    // 没有数字牌，出功能牌
    auto functionCards = getPlayableFunctionCards(gameState);
    if (!functionCards.empty()) {
        // 随机选择一张功能牌
        std::uniform_int_distribution<int> dist(0, functionCards.size() - 1);
        return functionCards[dist(randomGenerator)];
    }
    
    // 理论上不会执行到这里，因为playableCards不为空
    return nullptr;
}

CardColor AIPlayer::selectColorForWildCard(const GameState& gameState) {
    return getMostFrequentColorInHand();
}

bool AIPlayer::isNumberCard(const std::shared_ptr<Card>& card) const {
    return card->getType() == CardType::NUMBER;
}

std::vector<std::shared_ptr<Card>> AIPlayer::getPlayableNumberCards(const GameState& gameState) const {
    std::vector<std::shared_ptr<Card>> numberCards;
    auto playableCards = getPlayableCards(gameState);
    
    for (const auto& card : playableCards) {
        if (isNumberCard(card)) {
            numberCards.push_back(card);
        }
    }
    
    return numberCards;
}

std::vector<std::shared_ptr<Card>> AIPlayer::getPlayableFunctionCards(const GameState& gameState) const {
    std::vector<std::shared_ptr<Card>> functionCards;
    auto playableCards = getPlayableCards(gameState);
    
    for (const auto& card : playableCards) {
        if (!isNumberCard(card)) {
            functionCards.push_back(card);
        }
    }
    
    return functionCards;
}

CardColor AIPlayer::getMostFrequentColorInHand() const {
    // 统计手牌中各种颜色的数量（排除万能色）
    std::unordered_map<CardColor, int> colorCount;
    
    for (const auto& card : handCards) {
        CardColor color = card->getColor();
        if (color != CardColor::WILD) {
            colorCount[color]++;
        }
    }
    
    if (colorCount.empty()) {
        // 如果手牌中只有万能牌，随机选择一个非万能色
        std::vector<CardColor> colors = {
            CardColor::RED, CardColor::YELLOW, CardColor::GREEN, CardColor::BLUE
        };
        std::uniform_int_distribution<int> dist(0, colors.size() - 1);
        return colors[dist(randomGenerator)];
    }
    
    // 找到数量最多的颜色
    CardColor mostFrequent = colorCount.begin()->first;
    int maxCount = colorCount.begin()->second;
    
    for (const auto& pair : colorCount) {
        if (pair.second > maxCount) {
            mostFrequent = pair.first;
            maxCount = pair.second;
        }
    }
    
    return mostFrequent;
}

} // namespace UNO
