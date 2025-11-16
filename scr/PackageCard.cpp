#include "PackageCard.h"
#include "GameManager.h" // 需要访问 GameManager 来获取玩家
#include "Player.h"       // 需要访问 Player 来调用其手牌管理方法
#include <iostream>

/**
 * @brief PackageCard 构造函数
 * @param color 这张牌的颜色 (红/黄/绿/蓝)
 */
PackageCard::PackageCard(CardColor color)
    : Card(color, CardType::PACKAGE) {
}

/**
 * @brief PackageCard 的效果实现
 * 规则：玩家可以丢弃手中所有同色（非功能）卡牌 [5]
 */
void PackageCard::ApplyEffect(GameManager& game) const {
    // 1. 从 GameManager 获取当前玩家
    Player* currentPlayer = game.GetCurrentPlayer();

    // 2. 获取这张 PackageCard 自身的颜色
    CardColor colorToDiscard = this->GetColor();

    std::cout << currentPlayer->GetName() << " 打出了打包卡！正在丢弃所有 "
              << this->ToString() << " 颜色的数字牌..." << std::endl;

    // 3. 将复杂的逻辑委托给 Player 类自己处理
    //    这个函数 (DiscardAllNumberCardsOfColor) 将遍历玩家手牌，
    //    找出所有匹配的数字牌，将它们移动到弃牌堆，
    //    并从手牌中移除 [5]。
    currentPlayer->DiscardAllNumberCardsOfColor(colorToDiscard, game.GetDiscardPile());
}
