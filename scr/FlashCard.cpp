#include "FlashCard.h"
#include "GameManager.h" // 必须包含 GameManager 来触发 "Flash 事件"
#include <iostream>

/**
 * @brief FlashCard 构造函数
 * 这是一张 "Wild" 类型的牌，没有初始颜色 [5]
 */
FlashCard::FlashCard()
    : Card(CardColor::WILD, CardType::FLASH) {
}

/**
 * @brief FlashCard 的效果实现
 * 规则：玩家选一个颜色，其他玩家按顺序出牌，
 *      否则抽牌 [5]
 */
void FlashCard::ApplyEffect(GameManager& game) const {
    std::cout << "闪光卡事件触发！" << std::endl;

    // 1. 规则的第一步：玩家选择一种颜色 [5]
    CardColor chosenColor = game.AskPlayerToChooseColor();

    std::cout << "已选择颜色：" << chosenColor << "。所有其他玩家必须跟牌！" << std::endl;

    // 2. 委托：将这个复杂的多人子循环逻辑
    //    完全交给 GameManager 来协调处理 [5]。
    //    卡牌本身的任务到此结束。
    game.StartFlashEvent(chosenColor);
}
