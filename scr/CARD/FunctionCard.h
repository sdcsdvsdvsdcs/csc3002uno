#pragma once
#include "Card.h"

namespace UNO {

/**
 * 功能卡牌基类
 * 所有特殊功能卡牌的基类
 */
class FunctionCard : public Card {
public:
    FunctionCard(const CardData& data);
    virtual ~FunctionCard() = default;

    // 功能卡总是需要应用效果
    void applyEffect(GameState& gameState, int playerId, const nlohmann::json& params = {}) override = 0;

protected:
    // 辅助方法：让指定玩家抽牌
    void makePlayerDrawCards(GameState& gameState, int playerId, int count);
    
    // 辅助方法：跳过指定玩家的回合
    void skipPlayerTurn(GameState& gameState, int playerId);
    
    // 辅助方法：反转游戏方向
    void reversePlayDirection(GameState& gameState);
    
    // 辅助方法：改变当前颜色
    void changeCurrentColor(GameState& gameState, CardColor newColor);
};

/**
 * 跳过卡
 * 功能：下个玩家失去回合
 */
class SkipCard : public FunctionCard {
public:
    SkipCard(int id, CardColor color);
    void applyEffect(GameState& gameState, int playerId, const nlohmann::json& params = {}) override;
};

/**
 * 反转卡
 * 功能：反转游戏方向
 */
class ReverseCard : public FunctionCard {
public:
    ReverseCard(int id, CardColor color);
    void applyEffect(GameState& gameState, int playerId, const nlohmann::json& params = {}) override;
};

/**
 * 抽两张卡
 * 功能：下个玩家抽两张牌并失去回合
 */
class DrawTwoCard : public FunctionCard {
public:
    DrawTwoCard(int id, CardColor color);
    void applyEffect(GameState& gameState, int playerId, const nlohmann::json& params = {}) override;
};

/**
 * 万能卡
 * 功能：玩家选择新的颜色
 */
class WildCard : public FunctionCard {
public:
    WildCard(int id);
    void applyEffect(GameState& gameState, int playerId, const nlohmann::json& params = {}) override;
    bool requiresParameters() const override { return true; }
    nlohmann::json getParameterHint() const override;
    bool validateParameters(const nlohmann::json& params) const override;
};

/**
 * 万能抽四张卡
 * 功能：玩家选择新颜色，下个玩家抽四张牌并失去回合
 * 规则：只有玩家手中没有匹配当前颜色的牌时才能使用
 */
class WildDrawFourCard : public FunctionCard {
public:
    WildDrawFourCard(int id);
    bool canPlayOn(const Card* topCard) const override;
    void applyEffect(GameState& gameState, int playerId, const nlohmann::json& params = {}) override;
    bool requiresParameters() const override { return true; }
    nlohmann::json getParameterHint() const override;
    bool validateParameters(const nlohmann::json& params) const override;
    
private:
    bool validatePlayCondition(const GameState& gameState, int playerId) const;
};

/**
 * 包装卡（自定义卡牌）
 * 功能：玩家可以丢弃所有指定颜色的数字卡
 */
class PackageCard : public FunctionCard {
public:
    PackageCard(int id, CardColor color);
    void applyEffect(GameState& gameState, int playerId, const nlohmann::json& params = {}) override;
    bool requiresParameters() const override { return true; }
    nlohmann::json getParameterHint() const override;
    bool validateParameters(const nlohmann::json& params) const override;
    
private:
    std::vector<CardData> getDiscardableCards(const GameState& gameState, int playerId, CardColor targetColor) const;
};

/**
 * 闪光卡（自定义卡牌）
 * 功能：玩家选择颜色，其他玩家必须按顺序出该颜色牌，
 *       无法出牌的玩家需抽取等于已出该颜色牌数量的牌
 */
class FlashCard : public FunctionCard {
public:
    FlashCard(int id);
    void applyEffect(GameState& gameState, int playerId, const nlohmann::json& params = {}) override;
    bool requiresParameters() const override { return true; }
    nlohmann::json getParameterHint() const override;
    bool validateParameters(const nlohmann::json& params) const override;
    
private:
    struct FlashRoundResult {
        int cardsPlayed;    // 该回合出的牌数
        int playersSkipped; // 跳过的玩家数
        bool completed;     // 是否完成整个流程
    };
    
    FlashRoundResult processFlashRound(GameState& gameState, int startPlayerId, CardColor flashColor);
    bool canPlayerRespondToFlash(const GameState& gameState, int playerId, CardColor flashColor) const;
};

} // namespace UNO
