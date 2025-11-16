#include "WildCard.h"
#include "GameManager.h" // 必须包含 GameManager 来调用其方法

/**
 * @brief WildCard 构造函数
 * 调用基类 Card 的构造函数，设置固定的颜色和类型
 */
WildCard::WildCard()
    : Card(CardColor::WILD, CardType::WILD) {
}

/**
 * @brief WildCard 的效果实现
 * 规则：打出此牌的玩家选择下一个颜色 [5]
 */
void WildCard::ApplyEffect(GameManager& game) const {
    // 1. 将逻辑委托给 GameManager：询问当前玩家选择一种颜色
    CardColor chosenColor = game.AskPlayerToChooseColor();

    // 2. 将逻辑委托给 GameManager：设置游戏当前的生效颜色
    game.SetCurrentColor(chosenColor);
    
    // 注意：标准的 WildCard 不会跳过下一位玩家 [5]
}
