#pragma once

#include <vector>
#include <memory>   // 用于 std::unique_ptr
#include "Card.h"     // 必须包含我们新的抽象基类 Card.h

// 我们需要前向声明(Forward Declare) DiscardPile 类，
// 或者（更简单地）只接受一个原始的 vector。
// 为了简单起见，我们暂时只依赖 vector。

/**
 * @brief 牌堆类
 * 负责管理抽牌堆 (m_drawPile)
 */
class Deck {
public:
    /**
     * @brief 构造函数
     * 这个函数将在 Deck.cpp 中被实现，
     * 用来创建您提案 [1] 中所有的卡牌（数字、功能牌、创新牌）
     */
    Deck();
    
    /**
     * @brief 默认析构函数
     */
    ~Deck() = default;

    /**
     * @brief 洗牌
     * 您在 v1/Deck.cpp [1] 中的实现非常完美，可以直接移过来。
     */
    void shuffle();

    /**
     * @brief 从牌堆顶部抽一张牌
     * 您在 v1/Deck.cpp [1] 中的实现也是完美的，可以直接移过来。
     * @return 指向卡牌的 unique_ptr，如果牌堆为空则为 nullptr
     */
    std::unique_ptr<Card> drawCard();

    /**
     * @brief 检查抽牌堆是否为空
     * @return true 如果为空, 否则 false
     */
    bool isEmpty() const;

    /**
     * @brief (v2 的新功能) 将弃牌堆的牌加回抽牌堆并洗牌
     * 
     * 这是一个关键的游戏逻辑：当抽牌堆 (m_drawPile) 为空时，
     * 游戏管理器 (GameManager) 会调用这个函数，
     * 传入 *除了* 顶部卡牌之外的所有弃牌。
     *
     * @param discardPile 
     */
    void addDiscardPile(std::vector<std::unique_ptr<Card>>& discardPile);

private:
    // 您在 v1 [1] 中已经正确使用了这个数据结构！
    // 它是存储多态卡牌的完美方式 [2, 3, 4]。
    std::vector<std::unique_ptr<Card>> m_drawPile;
};
