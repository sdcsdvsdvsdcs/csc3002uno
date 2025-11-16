#pragma once
#include "Card.h"

/**
 * @brief 打包卡 (您的创新卡牌) [3]
 * 继承自 Card 抽象基类
 */
class PackageCard : public Card {
public:
    /**
     * @brief 构造函数
     * @param color 这张牌的颜色 (红/黄/绿/蓝)
     */
    PackageCard(CardColor color);

    /**
     * @brief 虚析构函数 (默认)
     */
    virtual ~PackageCard() = default;

    /**
     * @brief 重写基类的 ApplyEffect 方法
     * 这个函数的实现 (在.cpp 文件中) 将触发
     * "丢弃所有同色非功能牌" 的逻辑 [3]
     */
    virtual void ApplyEffect(GameManager& game) const override;
};
