#include "Card.h"
#include "GameState.h"

namespace UNO {

Card::Card(const CardData& data) : data(data) {}

bool Card::canPlayOn(const Card* topCard) const {
    if (!topCard) {
        return true; // 第一张牌总是可以出
    }
    
    return data.matches(topCard->getCardData());
}

} // namespace UNO
