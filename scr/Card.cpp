#include "Card.h"
#include "GameManager.h" // 需要包含 GameManager 来获取游戏状态
#include "Player.h"       // 需要包含 Player 来检查手牌 (用于WildDrawFour)

// --- 构造函数实现 ---
Card::Card(CardColor color, CardType type, CardValue value)
    : m_color(color), m_type(type), m_value(value) {
    // 构造函数的主体为空，所有工作都在初始化列表中完成
}

// --- 通用 IsPlayable 实现 ---
/**
 * @brief 检查这张卡是否可以合法地打出（默认规则）。
 * 这是大多数卡牌（数字牌、跳过、反转、+2）将使用的标准匹配逻辑。
 * WildDrawFour [1] 将重写(override)此函数以实现其特殊规则。
 */
bool Card::IsPlayable(const Card* topCard, const Player& player, 
                      const GameManager& game) const {
    
    // 规则 1: 万能牌 (Wild) 总是可以出的
    // (注意：常规 WildDrawFour 的限制由它自己的 override 版本处理)
    if (this->GetType() == CardType::WILD) {
        return true;
    }
    
    // 规则 2: 匹配当前颜色 [1]
    // 我们必须使用 game.GetCurrentColor() 而不是 topCard->GetColor()
    // 因为弃牌堆顶的可能是Wild牌，而当前生效的颜色是玩家选择的。
    if (this->GetColor() == game.GetCurrentColor()) {
        return true;
    }

    // 规则 3: 匹配数字 (仅限数字牌) [1]
    if (this->GetType() == CardType::NUMBER && 
        topCard->GetType() == CardType::NUMBER &&
        this->GetValue() == topCard->GetValue()) {
        return true;
    }

    // 规则 4: 匹配功能 (Skip, Reverse, DrawTwo, Package) [1]
    // 允许在同类型的功能牌上出牌（例如，红色的Skip可以出在蓝色的Skip上）
    if (this->GetType()!= CardType::NUMBER && 
        this->GetType()!= CardType::WILD &&
        this->GetType()!= CardType::WILD_DRAW_FOUR &&
        this->GetType() == topCard->GetType()) {
        return true;
    }

    // 如果都不匹配
    return false;
}

// --- 通用 ToString 实现 ---
/**
 * @brief 将卡牌转换为可读的字符串（用于调试和日志）
 */
std::string Card::ToString() const {
    std::string s_color;
    switch (m_color) {
        case CardColor::RED:    s_color = "红色"; break;
        case CardColor::YELLOW: s_color = "黄色"; break;
        case CardColor::GREEN:  s_color = "绿色"; break;
        case CardColor::BLUE:   s_color = "蓝色"; break;
        case CardColor::WILD:   s_color = "万能"; break;
    }

    switch (m_type) {
        case CardType::NUMBER:
            return s_color + " " + std::to_string(static_cast<int>(m_value));
        case CardType::SKIP:
            return s_color + " 跳过";
        case CardType::REVERSE:
            return s_color + " 反转";
        case CardType::DRAW_TWO:
            return s_color + " +2";
        case CardType::PACKAGE:
            return s_color + " 打包";
        case CardType::WILD:
            return s_color + " (选色)";
        case CardType::WILD_DRAW_FOUR:
            return s_color + " +4";
        case CardType::FLASH:
            return s_color + " 闪光";
    }
    return "未知卡牌";
}
