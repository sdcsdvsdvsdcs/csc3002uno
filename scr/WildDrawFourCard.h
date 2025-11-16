#pragma once
#include "Card.h"
class WildDrawFourCard : public Card {
public:
    WildDrawFourCard();
    virtual void ApplyEffect(GameManager& game) const override;
    
    // 重写 IsPlayable 来执行特殊规则检查
    virtual bool IsPlayable(const Card* topCard, const Player& player, 
                            const GameManager& game) const override;
};