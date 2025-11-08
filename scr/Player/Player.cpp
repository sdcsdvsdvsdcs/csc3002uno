#include "Player.h"
#include "GameState.h"
#include <algorithm>

namespace UNO {

Player::Player(int id, const std::string& name, bool isAI) 
    : id(id), name(name), isAIPlayer(isAI), character(CharacterType::LUCKY_STAR), 
      hasUnoStatus(false), score(0) {}

void Player::addCardToHand(std::shared_ptr<Card> card) {
    handCards.push_back(card);
    hasUnoStatus = (handCards.size() == 1); // 如果只剩一张牌，自动标记UNO状态
}

bool Player::removeCardFromHand(int cardId) {
    auto it = std::find_if(handCards.begin(), handCards.end(),
        [cardId](const std::shared_ptr<Card>& card) {
            return card->getId() == cardId;
        });
    
    if (it != handCards.end()) {
        handCards.erase(it);
        hasUnoStatus = (handCards.size() == 1); // 更新UNO状态
        return true;
    }
    return false;
}

bool Player::hasCard(int cardId) const {
    return std::any_of(handCards.begin(), handCards.end(),
        [cardId](const std::shared_ptr<Card>& card) {
            return card->getId() == cardId;
        });
}

bool Player::hasPlayableCards(const GameState& gameState) const {
    auto topCard = gameState.getTopCard();
    return std::any_of(handCards.begin(), handCards.end(),
        [&topCard](const std::shared_ptr<Card>& card) {
            return card->canPlayOn(topCard.get());
        });
}

std::vector<std::shared_ptr<Card>> Player::getPlayableCards(const GameState& gameState) const {
    std::vector<std::shared_ptr<Card>> playableCards;
    auto topCard = gameState.getTopCard();
    
    for (const auto& card : handCards) {
        if (card->canPlayOn(topCard.get())) {
            playableCards.push_back(card);
        }
    }
    
    return playableCards;
}

nlohmann::json Player::toJson() const {
    nlohmann::json handArray = nlohmann::json::array();
    for (const auto& card : handCards) {
        handArray.push_back(card->toJson());
    }
    
    return {
        {"id", id},
        {"name", name},
        {"isAI", isAIPlayer},
        {"character", static_cast<int>(character)},
        {"hasUno", hasUnoStatus},
        {"score", score},
        {"handCards", handArray}
    };
}

void Player::fromJson(const nlohmann::json& data) {
    id = data.value("id", 0);
    name = data.value("name", "");
    isAIPlayer = data.value("isAI", false);
    character = static_cast<CharacterType>(data.value("character", 0));
    hasUnoStatus = data.value("hasUno", false);
    score = data.value("score", 0);
    
    // 注意：卡牌对象的重建需要卡牌工厂
    handCards.clear();
    if (data.contains("handCards")) {
        for (const auto& cardData : data["handCards"]) {
            // 这里需要卡牌工厂来重建卡牌对象
            // handCards.push_back(CardFactory::createFromJson(cardData));
        }
    }
}

} // namespace UNO
