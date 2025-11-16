#pragma once
#include "Card.h"

/**
 * @brief 闪光卡 (您的创新卡牌) [3]
 * 继承自 Card 抽象基类
 */
class FlashCard : public Card {
public:
    /**
     * @brief 构造函数
     * 这张牌是 "Wild" 类型的，所以它没有初始颜色
     */
    FlashCard();

    /**
     * @brief 虚析构函数 (默认)
     */
    virtual ~FlashCard() = default;

    /**
     * @brief 重写基类的 ApplyEffect 方法
     * 这个函数的实现 (在.cpp 文件中) 将触发
     * "选择一种颜色，其他玩家按顺序出牌或抽牌" 的子循环 [3]
     */
    virtual void ApplyEffect(GameManager& game) const override;
};
