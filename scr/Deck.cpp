#include "Deck.h"
#include <algorithm> // for std::shuffle
#include <random>    // for std::mt19937

// 包含所有具体的卡牌类型
#include "NumberCard.h"
#include "SkipCard.h"
#include "ReverseCard.h"
#include "DrawTwoCard.h"
#include "WildCard.h"
#include "WildDrawFourCard.h"
#include "PackageCard.h"
#include "FlashCard.h"

Deck::Deck() {
    // 您的日志记录很好
    std::cout << "正在创建一副新牌..." << std::endl;

    // 1. 创建数字牌 (0-9) 
    // 标准UNO规则中0号牌每色1张，1-9号牌每色2张 [span_13](start_span)[span_13](end_span)
    for (auto color : {Card::CardColor::RED, Card::CardColor::YELLOW, 
                       Card::CardColor::GREEN, Card::CardColor::BLUE}) {
        // 1 张 0
        m_drawPile.push_back(std::make_unique<NumberCard>(color, Card::CardValue::ZERO));
        
        // 2 张 1-9
        for (int i = 0; i < 2; ++i) {
            m_drawPile.push_back(std::make_unique<NumberCard>(color, Card::CardValue::ONE));
            m_drawPile.push_back(std::make_unique<NumberCard>(color, Card::CardValue::TWO));
            m_drawPile.push_back(std::make_unique<NumberCard>(color, Card::CardValue::THREE));
            m_drawPile.push_back(std::make_unique<NumberCard>(color, Card::CardValue::FOUR));
            m_drawPile.push_back(std::make_unique<NumberCard>(color, Card::CardValue::FIVE));
            m_drawPile.push_back(std::make_unique<NumberCard>(color, Card::CardValue::SIX));
            m_drawPile.push_back(std::make_unique<NumberCard>(color, Card::CardValue::SEVEN));
            m_drawPile.push_back(std::make_unique<NumberCard>(color, Card::CardValue::EIGHT));
            m_drawPile.push_back(std::make_unique<NumberCard>(color, Card::CardValue::NINE));
        }
    }

    // 2. 创建标准功能牌 
    // 您的提案要求每种颜色2张
    for (auto color : {Card::CardColor::RED, Card::CardColor::YELLOW, 
                       Card::CardColor::GREEN, Card::CardColor::BLUE}) {
        for (int i = 0; i < 2; ++i) {
            m_drawPile.push_back(std::make_unique<SkipCard>(color));
            m_drawPile.push_back(std::make_unique<ReverseCard>(color));
            m_drawPile.push_back(std::make_unique<DrawTwoCard>(color));
        }
    }

    // 3. 创建万能牌 (Wilds) 
    // 您的提案要求 "2 cards"
    for (int i = 0; i < 2; ++i) {
        m_drawPile.push_back(std::make_unique<WildCard>());
        m_drawPile.push_back(std::make_unique<WildDrawFourCard>());
    }
    
    // 4. 创建您的创新卡 
    // "Package Card" (每种颜色2张)
    for (auto color : {Card::CardColor::RED, Card::CardColor::YELLOW, 
                       Card::CardColor::GREEN, Card::CardColor::BLUE}) {
        for (int i = 0; i < 2; ++i) {
            m_drawPile.push_back(std::make_unique<PackageCard>(color));
        }
    }
    // "Flash Card" ("2 cards")
    for (int i = 0; i < 2; ++i) {
        m_drawPile.push_back(std::make_unique<FlashCard>());
    }

    std::cout << "牌堆创建完毕！" << std::endl;

    // 您的洗牌逻辑  非常好，保持不变
    shuffle();
}

// 您的 shuffle() 和 drawCard()  实现是完美的，无需更改。