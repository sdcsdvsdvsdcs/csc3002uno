#include "NumberCard.h"
NumberCard::NumberCard(CardColor color, CardValue value)
    : Card(color, CardType::NUMBER, value) {}

// 数字牌没有效果 [span_17](start_span)[span_17](end_span)
void NumberCard::ApplyEffect(GameManager& game) const {
    // 明确地什么都不做
}
//... ToString() 的实现...
