#pragma once

#include <vector>
#include <memory>   // 用于 std::unique_ptr
#include "Card.h"     // 必须包含我们的抽象基类 Card.h

/**
 * @brief 弃牌堆类
 * 负责管理所有被打出的卡牌
 */
class DiscardPile {
public:
    /**
     * @brief 构造函数
     */
    DiscardPile();

    /**
     * @brief 默认析构函数
     */
    ~DiscardPile() = default;

    /**
     * @brief (v2 关键功能) 将一张卡牌添加到弃牌堆的顶部
     * 这将被 CardPlayingState 或 PackageCard 的效果调用 [1]
     * @param card 从玩家手牌中移出的卡牌
     */
    void Add(std::unique_ptr<Card> card);

    /**
     * @brief (v2 关键功能) 获取弃牌堆顶部的卡牌
     * 这是游戏用来检查下一张牌是否合法的卡牌
     * @return 指向顶部卡牌的原始指针 (不转移所有权)
     */
    const Card* GetTopCard() const;

    /**
     * @brief (v2 关键功能) 获取弃牌堆中的所有卡牌（用于重洗）
     * 
     * 当抽牌堆为空时，GameManager 将调用此方法。
     * 此方法会移交(move) *除了* 顶部卡牌之外的所有卡牌，
     * 并返回它们，以便 Deck 可以将它们洗牌。
     * 
     * @return 一个包含所有旧弃牌的 vector
     */
    std::vector<std::unique_ptr<Card>> TakeAllExceptTop();

    /**
     * @brief (v2 关键功能) 查看弃牌堆中的所有卡牌
     * 专为 "Collector" (收藏家) 技能提供支持 [1]
     * @return 对卡牌向量的常量引用
     */
    const std::vector<std::unique_ptr<Card>>& ViewAll() const;

private:
    // 和 Deck 一样，我们使用
    // std::vector<std::unique_ptr<Card>> 来管理多态卡牌 [1, 1]
    std::vector<std::unique_ptr<Card>> m_pile;
};
