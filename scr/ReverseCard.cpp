#include "PackageCard.h"
#include "GameManager.h"
#include "Player.h"

PackageCard::PackageCard(CardColor color)
    : Card(color, CardType::PACKAGE) {}

void PackageCard::ApplyEffect(GameManager& game) const {
    Player* currentPlayer = game.GetCurrentPlayer();
    // 效果是丢弃与这张 PackageCard 相同颜色的所有数字牌
    currentPlayer->DiscardAllNumberCardsOfColor(this->GetColor(), 
                                               game.GetDiscardPile());
}
