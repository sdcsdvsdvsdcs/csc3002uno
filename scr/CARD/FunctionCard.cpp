#include "FunctionCard.h"
#include "GameState.h"
#include <stdexcept>

namespace UNO {

// FunctionCard 基类实现
FunctionCard::FunctionCard(const CardData& data) : Card(data) {}

void FunctionCard::makePlayerDrawCards(GameState& gameState, int playerId, int count) {
    // 这里调用GameState的方法让玩家抽牌
    // 具体实现依赖于GameState的接口
    gameState.makePlayerDrawCards(playerId, count);
}

void FunctionCard::skipPlayerTurn(GameState& gameState, int playerId) {
    // 跳过玩家回合的实现
    gameState.skipPlayerTurn(playerId);
}

void FunctionCard::reversePlayDirection(GameState& gameState) {
    // 反转游戏方向
    gameState.reversePlayDirection();
}

void FunctionCard::changeCurrentColor(GameState& gameState, CardColor newColor) {
    // 改变当前颜色
    gameState.setCurrentColor(newColor);
}

// SkipCard 实现
SkipCard::SkipCard(int id, CardColor color) 
    : FunctionCard(CardData(id, color, CardType::SKIP, -1, "Skip")) {}

void SkipCard::applyEffect(GameState& gameState, int playerId, const nlohmann::json& params) {
    int nextPlayer = gameState.getNextPlayerId();
    skipPlayerTurn(gameState, nextPlayer);
    
    // 发送事件通知
    gameState.notifyCardEffect("skip", {
        {"sourcePlayer", playerId},
        {"targetPlayer", nextPlayer}
    });
}

// ReverseCard 实现
ReverseCard::ReverseCard(int id, CardColor color)
    : FunctionCard(CardData(id, color, CardType::REVERSE, -1, "Reverse")) {}

void ReverseCard::applyEffect(GameState& gameState, int playerId, const nlohmann::json& params) {
    reversePlayDirection(gameState);
    
    // 发送事件通知
    gameState.notifyCardEffect("reverse", {
        {"player", playerId}
    });
}

// DrawTwoCard 实现
DrawTwoCard::DrawTwoCard(int id, CardColor color)
    : FunctionCard(CardData(id, color, CardType::DRAW_TWO, -1, "Draw Two")) {}

void DrawTwoCard::applyEffect(GameState& gameState, int playerId, const nlohmann::json& params) {
    int nextPlayer = gameState.getNextPlayerId();
    makePlayerDrawCards(gameState, nextPlayer, 2);
    skipPlayerTurn(gameState, nextPlayer);
    
    // 发送事件通知
    gameState.notifyCardEffect("draw_two", {
        {"sourcePlayer", playerId},
        {"targetPlayer", nextPlayer},
        {"drawCount", 2}
    });
}

// WildCard 实现
WildCard::WildCard(int id)
    : FunctionCard(CardData(id, CardColor::WILD, CardType::WILD, -1, "Wild")) {}

void WildCard::applyEffect(GameState& gameState, int playerId, const nlohmann::json& params) {
    if (!params.contains("color")) {
        throw std::invalid_argument("Wild card requires color parameter");
    }
    
    CardColor newColor = static_cast<CardColor>(params["color"].get<int>());
    changeCurrentColor(gameState, newColor);
    
    // 发送事件通知
    gameState.notifyCardEffect("wild", {
        {"player", playerId},
        {"newColor", static_cast<int>(newColor)}
    });
}

nlohmann::json WildCard::getParameterHint() const {
    return {
        {"type", "color_selection"},
        {"description", "Choose a color for the wild card"},
        {"options", {0, 1, 2, 3}} // RED, YELLOW, GREEN, BLUE
    };
}

bool WildCard::validateParameters(const nlohmann::json& params) const {
    if (!params.contains("color")) {
        return false;
    }
    
    int colorValue = params["color"];
    return colorValue >= 0 && colorValue <= 3; // 0-3 对应四种颜色
}

// WildDrawFourCard 实现
WildDrawFourCard::WildDrawFourCard(int id)
    : FunctionCard(CardData(id, CardColor::WILD, CardType::WILD_DRAW_FOUR, -1, "Wild Draw Four")) {}

bool WildDrawFourCard::canPlayOn(const Card* topCard) const {
    // Wild Draw Four 的出牌条件需要结合游戏状态验证
    // 基础的颜色匹配逻辑在基类中处理
    return Card::canPlayOn(topCard);
}

bool WildDrawFourCard::validatePlayCondition(const GameState& gameState, int playerId) const {
    // 检查玩家手中是否有匹配当前颜色的牌
    // 如果没有匹配的牌，才能出Wild Draw Four
    CardColor currentColor = gameState.getCurrentColor();
    
    // 获取玩家手牌
    auto handCards = gameState.getPlayerHand(playerId);
    
    for (const auto& card : handCards) {
        // 如果手中有匹配当前颜色的牌，则不能出Wild Draw Four
        if (card.color == currentColor) {
            return false;
        }
    }
    
    return true;
}

void WildDrawFourCard::applyEffect(GameState& gameState, int playerId, const nlohmann::json& params) {
    // 验证出牌条件
    if (!validatePlayCondition(gameState, playerId)) {
        throw std::runtime_error("Cannot play Wild Draw Four: player has matching color cards");
    }
    
    if (!params.contains("color")) {
        throw std::invalid_argument("Wild Draw Four card requires color parameter");
    }
    
    // 改变颜色
    CardColor newColor = static_cast<CardColor>(params["color"].get<int>());
    changeCurrentColor(gameState, newColor);
    
    // 让下个玩家抽4张牌并跳过回合
    int nextPlayer = gameState.getNextPlayerId();
    makePlayerDrawCards(gameState, nextPlayer, 4);
    skipPlayerTurn(gameState, nextPlayer);
    
    // 发送事件通知
    gameState.notifyCardEffect("wild_draw_four", {
        {"sourcePlayer", playerId},
        {"targetPlayer", nextPlayer},
        {"newColor", static_cast<int>(newColor)},
        {"drawCount", 4}
    });
}

nlohmann::json WildDrawFourCard::getParameterHint() const {
    return {
        {"type", "color_selection"},
        {"description", "Choose a color for the wild draw four card"},
        {"options", {0, 1, 2, 3}} // RED, YELLOW, GREEN, BLUE
    };
}

bool WildDrawFourCard::validateParameters(const nlohmann::json& params) const {
    if (!params.contains("color")) {
        return false;
    }
    
    int colorValue = params["color"];
    return colorValue >= 0 && colorValue <= 3;
}

// PackageCard 实现
PackageCard::PackageCard(int id, CardColor color)
    : FunctionCard(CardData(id, color, CardType::PACKAGE, -1, "Package")) {}

void PackageCard::applyEffect(GameState& gameState, int playerId, const nlohmann::json& params) {
    if (!params.contains("color")) {
        throw std::invalid_argument("Package card requires color parameter");
    }
    
    CardColor targetColor = static_cast<CardColor>(params["color"].get<int>());
    
    // 获取可以丢弃的卡牌
    auto discardableCards = getDiscardableCards(gameState, playerId, targetColor);
    
    // 丢弃这些卡牌
    for (const auto& card : discardableCards) {
        gameState.discardPlayerCard(playerId, card.id);
    }
    
    // 发送事件通知
    gameState.notifyCardEffect("package", {
        {"player", playerId},
        {"targetColor", static_cast<int>(targetColor)},
        {"discardedCount", discardableCards.size()}
    });
}

nlohmann::json PackageCard::getParameterHint() const {
    return {
        {"type", "color_selection"},
        {"description", "Choose a color to discard all number cards of that color"},
        {"options", {0, 1, 2, 3}} // RED, YELLOW, GREEN, BLUE
    };
}

bool PackageCard::validateParameters(const nlohmann::json& params) const {
    if (!params.contains("color")) {
        return false;
    }
    
    int colorValue = params["color"];
    return colorValue >= 0 && colorValue <= 3;
}

std::vector<CardData> PackageCard::getDiscardableCards(const GameState& gameState, int playerId, CardColor targetColor) const {
    std::vector<CardData> discardable;
    
    auto handCards = gameState.getPlayerHand(playerId);
    for (const auto& card : handCards) {
        // 只丢弃指定颜色的数字卡（排除功能卡）
        if (card.color == targetColor && card.type == CardType::NUMBER) {
            discardable.push_back(card);
        }
    }
    
    return discardable;
}

// FlashCard 实现
FlashCard::FlashCard(int id)
    : FunctionCard(CardData(id, CardColor::WILD, CardType::FLASH, -1, "Flash")) {}

void FlashCard::applyEffect(GameState& gameState, int playerId, const nlohmann::json& params) {
    if (!params.contains("color")) {
        throw std::invalid_argument("Flash card requires color parameter");
    }
    
    CardColor flashColor = static_cast<CardColor>(params["color"].get<int>());
    
    // 设置闪光效果状态
    gameState.setFlashEffect(flashColor, playerId);
    
    // 处理闪光回合
    int currentPlayer = gameState.getNextPlayerId(); // 从下个玩家开始
    FlashRoundResult result = processFlashRound(gameState, currentPlayer, flashColor);
    
    // 发送事件通知
    gameState.notifyCardEffect("flash", {
        {"player", playerId},
        {"flashColor", static_cast<int>(flashColor)},
        {"cardsPlayed", result.cardsPlayed},
        {"playersSkipped", result.playersSkipped}
    });
}

nlohmann::json FlashCard::getParameterHint() const {
    return {
        {"type", "color_selection"},
        {"description", "Choose a color for the flash effect"},
        {"options", {0, 1, 2, 3}} // RED, YELLOW, GREEN, BLUE
    };
}

bool FlashCard::validateParameters(const nlohmann::json& params) const {
    if (!params.contains("color")) {
        return false;
    }
    
    int colorValue = params["color"];
    return colorValue >= 0 && colorValue <= 3;
}

FlashCard::FlashRoundResult FlashCard::processFlashRound(GameState& gameState, int startPlayerId, CardColor flashColor) {
    FlashRoundResult result = {0, 0, false};
    int currentPlayer = startPlayerId;
    int consecutiveFails = 0;
    
    // 最多遍历所有玩家一轮
    int maxIterations = gameState.getPlayerCount();
    
    for (int i = 0; i < maxIterations; i++) {
        if (canPlayerRespondToFlash(gameState, currentPlayer, flashColor)) {
            // 玩家可以响应，重置连续失败计数
            consecutiveFails = 0;
            result.cardsPlayed++;
            
            // 这里应该触发玩家出牌逻辑
            // 在实际实现中，这里会等待玩家出牌或AI自动出牌
            
        } else {
            // 玩家无法响应，需要抽牌
            consecutiveFails++;
            result.playersSkipped++;
            
            // 抽牌数量等于已出的该颜色牌数量
            int drawCount = result.cardsPlayed;
            if (drawCount > 0) {
                gameState.makePlayerDrawCards(currentPlayer, drawCount);
            }
            
            // 如果连续所有玩家都无法响应，结束闪光效果
            if (consecutiveFails >= gameState.getPlayerCount() - 1) {
                result.completed = true;
                break;
            }
        }
        
        // 移动到下一个玩家
        currentPlayer = gameState.getNextPlayerId(currentPlayer);
    }
    
    // 清除闪光效果状态
    gameState.clearFlashEffect();
    
    return result;
}

bool FlashCard::canPlayerRespondToFlash(const GameState& gameState, int playerId, CardColor flashColor) const {
    auto handCards = gameState.getPlayerHand(playerId);
    
    for (const auto& card : handCards) {
        // 检查是否有指定颜色的牌
        if (card.color == flashColor) {
            return true;
        }
        
        // 万能牌也可以响应
        if (card.color == CardColor::WILD) {
            return true;
        }
    }
    
    return false;
}

} // namespace UNO
