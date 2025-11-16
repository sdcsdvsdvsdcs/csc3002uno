#include "SkipCard.h"
#include "GameManager.h" // 需要包含以调用 game 的方法

SkipCard::SkipCard(CardColor color)
    : Card(color, CardType::SKIP) {}

// SkipCard 的效果 
void SkipCard::ApplyEffect(GameManager& game) const {
    game.SkipNextPlayer();
}
