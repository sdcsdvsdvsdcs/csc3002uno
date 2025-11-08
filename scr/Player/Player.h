#pragma once
#include <vector>
#include <string>
#include <memory>
#include <nlohmann/json.hpp>
#include "Card.h"
#include "common/GameTypes.h"

namespace UNO {

// 前向声明
class GameState;

/**
 * 玩家基类
 * 定义所有玩家的通用接口和行为
 */
class Player {
public:
    Player(int id, const std::string& name, bool isAI = false);
    virtual ~Player() = default;

    // 基本信息
    int getId() const { return id; }
    const std::string& getName() const { return name; }
    bool isAI() const { return isAIPlayer; }
    CharacterType getCharacter() const { return character; }
    void setCharacter(CharacterType newCharacter) { character = newCharacter; }

    // 手牌管理
    void addCardToHand(std::shared_ptr<Card> card);
    bool removeCardFromHand(int cardId);
    const std::vector<std::shared_ptr<Card>>& getHandCards() const { return handCards; }
    size_t getHandSize() const { return handCards.size(); }
    bool hasUno() const { return hasUnoStatus; }
    void setUno(bool status) { hasUnoStatus = status; }
    bool isHandEmpty() const { return handCards.empty(); }

    // 游戏操作接口
    virtual std::shared_ptr<Card> playTurn(GameState& gameState) = 0;
    virtual bool decideUseSkill(GameState& gameState) = 0;
    virtual nlohmann::json getSkillParameters(GameState& gameState) = 0;
    virtual CardColor chooseWildColor(GameState& gameState) = 0;

    // 状态查询
    bool hasCard(int cardId) const;
    bool hasPlayableCards(const GameState& gameState) const;
    std::vector<std::shared_ptr<Card>> getPlayableCards(const GameState& gameState) const;

    // 序列化
    nlohmann::json toJson() const;
    void fromJson(const nlohmann::json& data);

    // 统计信息
    int getScore() const { return score; }
    void addScore(int points) { score += points; }
    void resetScore() { score = 0; }

protected:
    int id;
    std::string name;
    bool isAIPlayer;
    CharacterType character;
    std::vector<std::shared_ptr<Card>> handCards;
    bool hasUnoStatus;
    int score;
};

} // namespace UNO
