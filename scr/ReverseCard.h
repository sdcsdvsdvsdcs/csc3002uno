#pragma once
#include "Card.h"

/**
 * @brief 反转牌 (标准功能牌)
 * 继承自 Card 抽象基类
 */
class ReverseCard : public Card {
public:
    /**
     * @brief 构造函数
     * @param color 这张牌的颜色 (红/黄/绿/蓝)
     */
    ReverseCard(CardColor color);

    /**
     * @brief 虚析构函数 (默认)
     */
    virtual ~ReverseCard() = default;

    /**
     * @brief 重写基类的 ApplyEffect 方法
     * 这个函数的实现 (在.cpp 文件中) 将调用 game.ReversePlayDirection()
     * [3]
     */
    virtual void ApplyEffect(GameManager& game) const override;
};