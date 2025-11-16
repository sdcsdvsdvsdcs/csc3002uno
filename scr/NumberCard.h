#pragma once
#include "Card.h"

class NumberCard : public Card {
public:
    NumberCard(CardColor color, CardValue value);
    virtual void ApplyEffect(GameManager& game) const override;
    virtual std::string ToString() const override;
};
