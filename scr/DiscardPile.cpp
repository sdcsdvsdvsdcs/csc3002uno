#include "DiscardPile.h"

/**
 * @brief 构造函数 (目前无需做任何事)
 */
DiscardPile::DiscardPile() {
    // m_pile 自动初始化为空 vector
}

/**
 * @brief 将一张卡牌添加到弃牌堆的顶部
 */
void DiscardPile::Add(std::unique_ptr<Card> card) {
    // 将卡牌的所有权转移到 m_pile 向量的末尾
    m_pile.push_back(std::move(card));
}

/**
 * @brief 获取弃牌堆顶部的卡牌
 */
const Card* DiscardPile::GetTopCard() const {
    if (m_pile.empty()) {
        // 游戏开始时，弃牌堆可能为空 (直到 Deck 翻开第一张)
        return nullptr;
    }
    //.get() 返回 std::unique_ptr 所管理的原始指针，
    // 而不放弃所有权
    return m_pile.back().get();
}

/**
 * @brief (v2 关键功能) 获取弃牌堆中的所有卡牌（用于重洗）
 */
std::vector<std::unique_ptr<Card>> DiscardPile::TakeAllExceptTop() {
    std::vector<std::unique_ptr<Card>> cardsToReturn;

    // 只有在弃牌堆中有多于一张牌时才执行
    if (m_pile.size() > 1) {
        
        // 1. 保存顶部的卡牌 (最后一张)
        // 我们使用 std::move 将其所有权从 m_pile 转移到 topCard
        std::unique_ptr<Card> topCard = std::move(m_pile.back());
        m_pile.pop_back(); // 移除 (现在为空的) unique_ptr

        // 2. 将 m_pile 中剩余的所有卡牌的所有权
        //    高效地转移到 cardsToReturn 
        cardsToReturn = std::move(m_pile);

        // 3. 清空 m_pile (它现在是空的，因为
        //    它"移交"了它的内容)
        m_pile.clear();

        // 4. 将我们保存的顶部卡牌放回
        // (它现在是 m_pile 中唯一的卡牌)
        m_pile.push_back(std::move(topCard));
    }
    
    // 5. 返回所有被取出的卡牌
    return cardsToReturn;
}

/**
 * @brief (v2 关键功能) 查看弃牌堆中的所有卡牌
 */
const std::vector<std::unique_ptr<Card>>& DiscardPile::ViewAll() const {
    // 返回对内部向量的常量引用，
    // "Collector" 技能 [1] 可以查看它，但不能修改它
    return m_pile;
}
