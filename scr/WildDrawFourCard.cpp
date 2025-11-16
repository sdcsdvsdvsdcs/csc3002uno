#include "WildDrawFourCard.h"
#include "GameManager.h"
#include "Player.h"

WildDrawFourCard::WildDrawFourCard()
    : Card(CardColor::WILD, CardType::WILD_DRAW_FOUR) {}

void WildDrawFourCard::ApplyEffect(GameManager& game) const {
    // 1. 让玩家选择一个新颜色
    CardColor chosenColor = game.AskPlayerToChooseColor();
    game.SetCurrentColor(chosenColor);
    
    // 2. 下一个玩家抽4张并跳过
    game.ForceNextPlayerToDraw(4);
    game.SkipNextPlayer();
}

bool WildDrawFourCard::IsPlayable(const Card* topCard, const Player& player,
                                  const GameManager& game) const {
    // 您的规则  是一个 *播放限制*
    CardColor colorToMatch = game.GetCurrentColor(); // 必须使用游戏状态中的当前颜色

    // 迭代玩家手牌 [span_19](start_span)[span_19](end_span)
    for (const auto& cardInHand : player.GetHand()) {
        if (cardInHand->GetColor() == colorToMatch) {
            // 找到了匹配颜色的牌！
            return false; // 不允许打出 WildDrawFour
        }
    }
    
    // 手上没有匹配颜色的牌，可以打出
    return true; 
    
    // 注意：标准规则允许“虚张声势” (bluffing) [span_20](start_span)[span_20](end_span)，
    // 但您的提案规则似乎更严格。我们遵循您的提案 。
}
