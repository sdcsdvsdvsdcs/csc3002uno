#include "Player.h"
#include "DiscardPile.h"
#include <algorithm> // for std::partition

void Player::DiscardAllNumberCardsOfColor(Card::CardColor color, 
                                          DiscardPile& discardPile) {
    // std::partition 会根据一个谓词（lambda函数）重新排序 m_hand
    // 它将所有“应该被保留”的卡牌（谓词返回true）移动到 m_hand 的最前面
    // 并返回一个指向“不保留”（即我们要丢弃的）卡牌的起始迭代器 
    aut[span_22](start_span)[span_22](end_span)o partitionPoint = std::partition(m_hand.begin(), m_hand.end(),
        [color](const std::unique_ptr<Card>& card) {
            // 谓词：什么情况下我们 *保留* 这张卡？
            // 1. 如果它的颜色 *不* 是目标颜色
            // 2. 或者，如果它 *不是* 一张数字牌 (规则要求只丢弃数字牌 )
            return card->GetColor()!= color |

| 
                   card->GetType()!= Card::CardType::NUMBER;
        });

    // 现在，从 partitionPoint 到 m_hand.end() 的所有卡牌都是我们要丢弃的
    // 我们将它们移动到弃牌堆
    for (auto it = partitionPoint; it!= m_hand.end(); ++it) {
        // (*it) 是一个 std::unique_ptr<Card>
        discardPile.Add(std::move(*it));
    }

    // 最后，从手牌中移除这些卡牌（它们现在是空的 unique_ptr）
    // 这就是“erase-remove” (或在此处为 "erase-partition") 范式 [span_25](start_span)[span_25](end_span)
    m_hand.erase(partitionPoint, m_hand.end());
}