#pragma once
#include "Card.h"

/**
 * @brief 万能牌 (标准功能牌) [5]
 * 继承自 Card 抽象基类
 */
class WildCard : public Card {
public:
    /**
     * @brief 构造函数
     * WildCard 总是具有 WILD 颜色和 WILD 类型
     */
    WildCard();

    /**
     * @brief 虚析构函数 (默认)
     */
    virtual ~WildCard() = default;

    /**
     * @brief 重写基类的 ApplyEffect 方法
     * 这个函数的实现 (在.cpp 文件中) 将
     * 允许当前玩家选择一个新的颜色 [5]
     */
    virtual void ApplyEffect(GameManager& game) const override;
};