#pragma once
#include "Card.h"
class DrawTwoCard : public Card {
public:
    DrawTwoCard(CardColor color);
    virtual void ApplyEffect(GameManager& game) const override;
};