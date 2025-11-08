#pragma once
#include <vector>
#include <memory>
#include <random>
#include <nlohmann/json.hpp>
#include "Card.h"
#include "FunctionCard.h"

namespace UNO {

/**
 * 牌堆管理器
 * 管理抽牌堆和弃牌堆，处理发牌、抽牌、洗牌等操作
 */
class Deck {
public:
    Deck();
    
    // 初始化方法
    void initialize();
    void clear();
    
    // 牌堆操作
    std::shared_ptr<Card> drawCard();
    std::vector<std::shared_ptr<Card>> drawCards(int count);
    void discardCard(std::shared_ptr<Card> card);
    
    // 发牌
    std::vector<std::shared_ptr<Card>> dealInitialHand(int cardCount = 7);
    
    // 牌堆状态
    size_t getDrawPileSize() const { return drawPile.size(); }
    size_t getDiscardPileSize() const { return discardPile.size(); }
    bool isDrawPileEmpty() const { return drawPile.empty(); }
    
    // 弃牌堆操作
    std::shared_ptr<Card> getTopDiscardCard() const;
    const std::vector<std::shared_ptr<Card>>& getDiscardPile() const { return discardPile; }
    
    // 重洗牌堆
    void reshuffleFromDiscard();
    
    // 序列化
    nlohmann::json toJson() const;
    void fromJson(const nlohmann::json& data);
    
    // 统计信息
    int getTotalCards() const { return getDrawPileSize() + getDiscardPileSize(); }

private:
    std::vector<std::shared_ptr<Card>> drawPile;
    std::vector<std::shared_ptr<Card>> discardPile;
    std::mt19937 randomGenerator;
    
    void createStandardDeck();
    void shuffleDrawPile();
    std::shared_ptr<Card> createCard(CardType type, int id, CardColor color = CardColor::WILD, int number = -1);
};

} // namespace UNO
