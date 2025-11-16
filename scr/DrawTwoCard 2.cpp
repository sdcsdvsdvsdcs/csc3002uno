#include "DrawTwoCard.h"
#include "GameManager.h"

DrawTwoCard::DrawTwoCard(CardColor color)
    : Card(color, CardType::DRAW_TWO) {}

// DrawTwoCard 的效果 
void DrawTwoCard::ApplyEffect(GameManager& game) const {
    game.ForceNextPlayerToDraw(2);
    game.SkipNextPlayer(); // 规则：抽牌并跳过回合 [span_18](start_span)[span_18](end_span)
}
