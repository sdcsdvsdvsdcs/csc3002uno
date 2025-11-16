#pragma once
#include <string>
#include <vector>

// 前向声明 (Forward Declare)
class GameManager;
class Player;

class Card {
public:
    enum class CardColor {
        RED, YELLOW, GREEN, BLUE, WILD
    };

    enum class CardType {
        NUMBER,
        SKIP,
        REVERSE,
        DRAW_TWO,
        WILD,
        WILD_DRAW_FOUR,
        PACKAGE,
        FLASH
    };

    enum class CardValue {
        ZERO, ONE, TWO, THREE, FOUR,
        FIVE, SIX, SEVEN, EIGHT, NINE,
        NONE // 用于所有功能牌
    };

    // 构造函数
    Card(CardColor color, CardType type, CardValue value = CardValue::NONE);
    // 虚析构函数
    virtual ~Card() = default;

    // --- 核心虚函数 ---

    // 纯虚函数：所有子类都必须实现这个
    virtual void ApplyEffect(GameManager& game) const = 0;

    // 虚函数：大多数卡牌使用这个默认规则，但 WildDrawFour 会重写(override)它
    virtual bool IsPlayable(const Card* topCard, const Player& player, 
                            const GameManager& game) const;
    
    // 辅助函数
    virtual std::string ToString() const;

    // --- Getters ---
    CardColor GetColor() const { return m_color; }
    CardType GetType() const { return m_type; }
    CardValue GetValue() const { return m_value; }

    // Wild 卡需要改变颜色
    void SetColor(CardColor color) { m_color = color; }

protected:
    CardColor m_color;
    const CardType m_type;
    const CardValue m_value;
};

// 手牌的类型别名
using PlayerHand = std::vector<std::unique_ptr<Card>>;
