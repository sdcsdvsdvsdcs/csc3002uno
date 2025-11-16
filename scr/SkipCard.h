#pragma once
#include "Card.h"
class SkipCard : public Card {
public:
    SkipCard(CardColor color);
    virtual void ApplyEffect(GameManager& game) const override;
};
