#include "Deck.h"
#include <algorithm>
#include <random>

namespace UNO {

Deck::Deck() : randomGenerator(std::random_device{}()) {}

void Deck::initialize() {
    clear();
    createStandardDeck();
    shuffleDrawPile();
}

void Deck::clear() {
    drawPile.clear();
    discardPile.clear();
}

void Deck::createStandardDeck() {
    int cardId = 1;
    
    // 创建数字卡 (0-9，每种颜色0号一张，1-9各两张)
    for (auto color : {CardColor::RED, CardColor::YELLOW, CardColor::GREEN, CardColor::BLUE}) {
        // 数字0，每种颜色一张
        drawPile.push_back(createCard(CardType::NUMBER, cardId++, color, 0));
        
        // 数字1-9，每种颜色两张
        for (int number = 1; number <= 9; number++) {
            drawPile.push_back(createCard(CardType::NUMBER, cardId++, color, number));
            drawPile.push_back(createCard(CardType::NUMBER, cardId++, color, number));
        }
    }
    
    // 创建标准功能卡 (Skip, Reverse, Draw Two)，每种颜色两张
    for (auto color : {CardColor::RED, CardColor::YELLOW, CardColor::GREEN, CardColor::BLUE}) {
        for (int i = 0; i < 2; i++) {
            drawPile.push_back(createCard(CardType::SKIP, cardId++, color));
            drawPile.push_back(createCard(CardType::REVERSE, cardId++, color));
            drawPile.push_back(createCard(CardType::DRAW_TWO, cardId++, color));
        }
    }
    
    // 创建万能卡 (Wild, Wild Draw Four)，各四张
    for (int i = 0; i < 4; i++) {
        drawPile.push_back(createCard(CardType::WILD, cardId++));
        drawPile.push_back(createCard(CardType::WILD_DRAW_FOUR, cardId++));
    }
    
    // 创建自定义卡 (Package Card)，每种颜色两张
    for (auto color : {CardColor::RED, CardColor::YELLOW, CardColor::GREEN, CardColor::BLUE}) {
        for (int i = 0; i < 2; i++) {
            drawPile.push_back(createCard(CardType::PACKAGE, cardId++, color));
        }
    }
    
    // 创建自定义卡 (Flash Card)，两张
    for (int i = 0; i < 2; i++) {
        drawPile.push_back(createCard(CardType::FLASH, cardId++));
    }
}

std::shared_ptr<Card> Deck::createCard(CardType type, int id, CardColor color, int number) {
    CardData data(id, color, type, number);
    
    switch (type) {
        case CardType::NUMBER:
            data.name = std::to_string(number);
            return std::make_shared<Card>(data);
            
        case CardType::SKIP:
            data.name = "Skip";
            return std::make_shared<SkipCard>(id, color);
            
        case CardType::REVERSE:
            data.name = "Reverse";
            return std::make_shared<ReverseCard>(id, color);
            
        case CardType::DRAW_TWO:
            data.name = "Draw Two";
            return std::make_shared<DrawTwoCard>(id, color);
            
        case CardType::WILD:
            data.name = "Wild";
            return std::make_shared<WildCard>(id);
            
        case CardType::WILD_DRAW_FOUR:
            data.name = "Wild Draw Four";
            return std::make_shared<WildDrawFourCard>(id);
            
        case CardType::PACKAGE:
            data.name = "Package";
            return std::make_shared<PackageCard>(id, color);
            
        case CardType::FLASH:
            data.name = "Flash";
            return std::make_shared<FlashCard>(id);
            
        default:
            return std::make_shared<Card>(data);
    }
}

void Deck::shuffleDrawPile() {
    std::shuffle(drawPile.begin(), drawPile.end(), randomGenerator);
}

std::shared_ptr<Card> Deck::drawCard() {
    if (drawPile.empty()) {
        reshuffleFromDiscard();
    }
    
    if (drawPile.empty()) {
        return nullptr; // 牌堆完全空了
    }
    
    auto card = drawPile.back();
    drawPile.pop_back();
    return card;
}

std::vector<std::shared_ptr<Card>> Deck::drawCards(int count) {
    std::vector<std::shared_ptr<Card>> cards;
    for (int i = 0; i < count; i++) {
        auto card = drawCard();
        if (card) {
            cards.push_back(card);
        }
    }
    return cards;
}

void Deck::discardCard(std::shared_ptr<Card> card) {
    discardPile.push_back(card);
}

std::vector<std::shared_ptr<Card>> Deck::dealInitialHand(int cardCount) {
    return drawCards(cardCount);
}

std::shared_ptr<Card> Deck::getTopDiscardCard() const {
    if (discardPile.empty()) {
        return nullptr;
    }
    return discardPile.back();
}

void Deck::reshuffleFromDiscard() {
    if (discardPile.size() <= 1) {
        return; // 需要至少一张牌在弃牌堆中（顶牌）
    }
    
    // 保留顶牌在弃牌堆中
    auto topCard = discardPile.back();
    discardPile.pop_back();
    
    // 将剩余的弃牌堆移到抽牌堆
    drawPile = std::move(discardPile);
    discardPile.clear();
    discardPile.push_back(topCard);
    
    // 洗牌
    shuffleDrawPile();
}

nlohmann::json Deck::toJson() const {
    nlohmann::json drawArray = nlohmann::json::array();
    for (const auto& card : drawPile) {
        drawArray.push_back(card->toJson());
    }
    
    nlohmann::json discardArray = nlohmann::json::array();
    for (const auto& card : discardPile) {
        discardArray.push_back(card->toJson());
    }
    
    return {
        {"drawPile", drawArray},
        {"discardPile", discardArray}
    };
}

void Deck::fromJson(const nlohmann::json& data) {
    drawPile.clear();
    discardPile.clear();
    
    if (data.contains("drawPile")) {
        for (const auto& cardData : data["drawPile"]) {
            // 需要卡牌工厂来重建卡牌对象
            // drawPile.push_back(CardFactory::createFromJson(cardData));
        }
    }
    
    if (data.contains("discardPile")) {
        for (const auto& cardData : data["discardPile"]) {
            // 需要卡牌工厂来重建卡牌对象
            // discardPile.push_back(CardFactory::createFromJson(cardData));
        }
    }
}

} // namespace UNO
