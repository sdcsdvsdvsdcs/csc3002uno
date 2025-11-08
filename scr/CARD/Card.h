#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <memory>

namespace UNO {

// 卡牌颜色枚举
enum class CardColor {
    RED,
    YELLOW, 
    GREEN,
    BLUE,
    WILD  // 万能色
};

// 卡牌类型枚举
enum class CardType {
    NUMBER,         // 数字卡
    SKIP,           // 跳过
    REVERSE,        // 反转
    DRAW_TWO,       // 抽两张
    WILD,           // 万能牌
    WILD_DRAW_FOUR, // 万能抽四张
    PACKAGE,        // 包装卡（自定义）
    FLASH           // 闪光卡（自定义）
};

/**
 * 卡牌数据基础结构
 * 用于序列化和网络传输
 */
struct CardData {
    int id;             // 卡牌唯一ID
    CardColor color;    // 卡牌颜色
    CardType type;      // 卡牌类型
    int number;         // 数字（仅数字卡有效，-1表示非数字卡）
    std::string name;   // 卡牌显示名称

    // 默认构造函数
    CardData() : id(-1), color(CardColor::WILD), type(CardType::NUMBER), number(-1), name("Unknown") {}
    
    // 带参构造函数
    CardData(int cardId, CardColor cardColor, CardType cardType, int cardNumber = -1, const std::string& cardName = "")
        : id(cardId), color(cardColor), type(cardType), number(cardNumber), name(cardName) {}

    // 序列化为JSON
    nlohmann::json toJson() const {
        return {
            {"id", id},
            {"color", static_cast<int>(color)},
            {"type", static_cast<int>(type)},
            {"number", number},
            {"name", name}
        };
    }

    // 从JSON反序列化
    static CardData fromJson(const nlohmann::json& json) {
        CardData card;
        card.id = json.value("id", -1);
        card.color = static_cast<CardColor>(json.value("color", 4));
        card.type = static_cast<CardType>(json.value("type", 0));
        card.number = json.value("number", -1);
        card.name = json.value("name", "Unknown");
        return card;
    }

    // 转换为字符串表示（用于调试和显示）
    std::string toString() const {
        std::string colorStr;
        switch (color) {
            case CardColor::RED: colorStr = "Red"; break;
            case CardColor::YELLOW: colorStr = "Yellow"; break;
            case CardColor::GREEN: colorStr = "Green"; break;
            case CardColor::BLUE: colorStr = "Blue"; break;
            case CardColor::WILD: colorStr = "Wild"; break;
        }

        if (type == CardType::NUMBER) {
            return colorStr + " " + std::to_string(number);
        } else {
            return colorStr + " " + name;
        }
    }

    // 检查是否匹配另一张卡牌（颜色或类型匹配）
    bool matches(const CardData& other) const {
        // 万能牌可以匹配任何颜色
        if (color == CardColor::WILD || other.color == CardColor::WILD) {
            return true;
        }
        
        // 颜色匹配
        if (color == other.color) {
            return true;
        }
        
        // 类型匹配（对于功能卡）
        if (type != CardType::NUMBER && other.type != CardType::NUMBER) {
            if (type == other.type) {
                return true;
            }
        }
        
        // 数字匹配（对于数字卡）
        if (type == CardType::NUMBER && other.type == CardType::NUMBER) {
            if (number == other.number) {
                return true;
            }
        }
        
        return false;
    }
};

// 前向声明
class GameState;

/**
 * 卡牌基类
 * 所有具体卡牌类型的基类
 */
class Card {
public:
    Card(const CardData& data);
    virtual ~Card() = default;

    // 获取卡牌数据
    const CardData& getCardData() const { return data; }
    
    // 获取卡牌ID
    int getId() const { return data.id; }
    
    // 获取卡牌颜色
    CardColor getColor() const { return data.color; }
    
    // 获取卡牌类型  
    CardType getType() const { return data.type; }
    
    // 获取卡牌数字
    int getNumber() const { return data.number; }
    
    // 获取卡牌名称
    const std::string& getName() const { return data.name; }

    /**
     * 检查卡牌是否可以出在指定的顶牌上
     * @param topCard 牌堆顶部的卡牌
     * @return 是否可以出牌
     */
    virtual bool canPlayOn(const Card* topCard) const;

    /**
     * 应用卡牌效果到游戏状态
     * @param gameState 游戏状态引用
     * @param playerId 出牌玩家ID
     * @param params 额外参数（如选择的颜色等）
     */
    virtual void applyEffect(GameState& gameState, int playerId, const nlohmann::json& params = {}) = 0;

    /**
     * 检查是否需要额外参数（如Wild卡需要选择颜色）
     * @return 是否需要参数
     */
    virtual bool requiresParameters() const { return false; }

    /**
     * 获取参数提示信息
     * @return 参数提示JSON
     */
    virtual nlohmann::json getParameterHint() const { return {}; }

    /**
     * 验证参数是否有效
     * @param params 参数
     * @return 是否有效
     */
    virtual bool validateParameters(const nlohmann::json& params) const { return true; }

    /**
     * 转换为字符串表示
     */
    std::string toString() const { return data.toString(); }

    /**
     * 序列化为JSON
     */
    nlohmann::json toJson() const { return data.toJson(); }

protected:
    CardData data;
};

} // namespace UNO
