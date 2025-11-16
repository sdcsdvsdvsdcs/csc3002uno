#pragma once

#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "Card.h"
#include "FunctionCard.h"
#include "Player.h"

namespace UNO {

// 前向声明
class GameBoard;

/**
 * 游戏状态接口类
 * 为卡牌效果和玩家决策提供游戏状态访问
 */
class GameState {
public:
    virtual ~GameState() = default;

    // 游戏状态查询
    virtual std::shared_ptr<Card> getTopCard() const = 0;
    virtual CardColor getCurrentColor() const = 0;
    virtual int getCurrentPlayer() const = 0;
    virtual int getNextPlayerId() const = 0;
    virtual int getPlayerCount() const = 0;
    virtual bool isClockwise() const = 0;
    
    // 玩家操作
    virtual void makePlayerDrawCards(int playerId, int count) = 0;
    virtual void skipPlayerTurn(int playerId) = 0;
    virtual void reversePlayDirection() = 0;
    virtual void setCurrentColor(CardColor color) = 0;
    
    // 特殊效果管理
    virtual void setFlashEffect(CardColor color, int playerId) = 0;
    virtual void clearFlashEffect() = 0;
    virtual bool isFlashEffectActive() const = 0;
    virtual CardColor getFlashEffectColor() const = 0;
    
    // 玩家手牌访问
    virtual std::vector<CardData> getPlayerHand(int playerId) const = 0;
    virtual void discardPlayerCard(int playerId, int cardId) = 0;
    
    // 事件通知
    virtual void notifyCardEffect(const std::string& effect, const nlohmann::json& data) = 0;
    
    // 验证方法
    virtual bool validateWildDrawFour(int playerId) const = 0;
    
    // 回合阶段管理
    virtual bool isSkillPhase() const = 0;
    virtual bool isCardPlayPhase() const = 0;
};

/**
 * GameBoard 适配器，实现 GameState 接口
 * 将 GameBoard 的功能暴露给卡牌效果系统
 */
class GameBoardAdapter : public GameState {
public:
    explicit GameBoardAdapter(Game::GameBoard* gameBoard);
    
    // GameState 接口实现
    std::shared_ptr<Card> getTopCard() const override;
    CardColor getCurrentColor() const override;
    int getCurrentPlayer() const override;
    int getNextPlayerId() const override;
    int getPlayerCount() const override;
    bool isClockwise() const override;
    
    void makePlayerDrawCards(int playerId, int count) override;
    void skipPlayerTurn(int playerId) override;
    void reversePlayDirection() override;
    void setCurrentColor(CardColor color) override;
    
    void setFlashEffect(CardColor color, int playerId) override;
    void clearFlashEffect() override;
    bool isFlashEffectActive() const override;
    CardColor getFlashEffectColor() const override;
    
    std::vector<CardData> getPlayerHand(int playerId) const override;
    void discardPlayerCard(int playerId, int cardId) override;
    
    void notifyCardEffect(const std::string& effect, const nlohmann::json& data) override;
    
    bool validateWildDrawFour(int playerId) const override;
    
    bool isSkillPhase() const override;
    bool isCardPlayPhase() const override;

private:
    Game::GameBoard* m_gameBoard;
    
    // 辅助方法
    CardColor convertToCardColor(Game::CardColor color) const;
    Game::CardColor convertFromCardColor(CardColor color) const;
    CardType convertToCardType(Game::CardText text) const;
    std::shared_ptr<Card> createCardFromGameCard(const Game::Card& gameCard) const;
};

} // namespace UNO
