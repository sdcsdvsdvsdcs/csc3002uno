#include "GameState.h"
#include "game_board.h"
#include "stat.h"
#include "cards.h"
#include <algorithm>

namespace UNO {

GameBoardAdapter::GameBoardAdapter(Game::GameBoard* gameBoard) 
    : m_gameBoard(gameBoard) {}

std::shared_ptr<Card> GameBoardAdapter::getTopCard() const {
    if (!m_gameBoard) return nullptr;
    
    auto discardPile = m_gameBoard->GetDiscardPile();
    if (!discardPile || discardPile->GetPile().empty()) {
        return nullptr;
    }
    
    // 获取弃牌堆顶部的牌
    const auto& pile = discardPile->GetPile();
    Game::Card topGameCard = pile.back();
    
    return createCardFromGameCard(topGameCard);
}

CardColor GameBoardAdapter::getCurrentColor() const {
    if (!m_gameBoard) return CardColor::WILD;
    
    auto gameStat = m_gameBoard->GetGameStat();
    if (!gameStat) return CardColor::WILD;
    
    Game::Card lastCard = gameStat->GetLastPlayedCard();
    return convertToCardColor(lastCard.mColor);
}

int GameBoardAdapter::getCurrentPlayer() const {
    if (!m_gameBoard) return 0;
    
    auto gameStat = m_gameBoard->GetGameStat();
    if (!gameStat) return 0;
    
    return gameStat->GetCurrentPlayer();
}

int GameBoardAdapter::getNextPlayerId() const {
    if (!m_gameBoard) return 0;
    
    auto gameStat = m_gameBoard->GetGameStat();
    if (!gameStat) return 0;
    
    int current = gameStat->GetCurrentPlayer();
    bool clockwise = gameStat->IsInClockwise();
    
    // 计算下一个玩家
    int nextPlayer = clockwise ? 
        (current + 1) % getPlayerCount() :
        (current - 1 + getPlayerCount()) % getPlayerCount();
    
    return nextPlayer;
}

int GameBoardAdapter::getPlayerCount() const {
    if (!m_gameBoard) return 0;
    
    auto playerStats = m_gameBoard->GetPlayerStats();
    return static_cast<int>(playerStats.size());
}

bool GameBoardAdapter::isClockwise() const {
    if (!m_gameBoard) return true;
    
    auto gameStat = m_gameBoard->GetGameStat();
    if (!gameStat) return true;
    
    return gameStat->IsInClockwise();
}

void GameBoardAdapter::makePlayerDrawCards(int playerId, int count) {
    if (!m_gameBoard || count <= 0) return;
    
    // 在实际实现中，这里应该调用 GameBoard 的抽牌方法
    // 由于 GameBoard 的接口限制，我们可能需要扩展 GameBoard 类
    // 目前先记录日志
    std::cout << "Player " << playerId << " draws " << count << " cards" << std::endl;
}

void GameBoardAdapter::skipPlayerTurn(int playerId) {
    if (!m_gameBoard) return;
    
    // 在实际实现中，这里应该调用 GameBoard 的跳过回合方法
    std::cout << "Player " << playerId << "'s turn is skipped" << std::endl;
}

void GameBoardAdapter::reversePlayDirection() {
    if (!m_gameBoard) return;
    
    auto gameStat = m_gameBoard->GetGameStat();
    if (!gameStat) return;
    
    gameStat->Reverse();
}

void GameBoardAdapter::setCurrentColor(CardColor color) {
    if (!m_gameBoard) return;
    
    // 在实际实现中，这里应该更新游戏状态中的当前颜色
    // 由于 GameBoard 的架构，这可能需要在 PlayInfo 中处理
    std::cout << "Current color set to: " << static_cast<int>(color) << std::endl;
}

void GameBoardAdapter::setFlashEffect(CardColor color, int playerId) {
    if (!m_gameBoard) return;
    
    // 在实际实现中，这里应该设置闪光效果状态
    // 目前 GameBoard 已经有相关的成员变量
    std::cout << "Flash effect activated by player " << playerId 
              << " with color: " << static_cast<int>(color) << std::endl;
}

void GameBoardAdapter::clearFlashEffect() {
    if (!m_gameBoard) return;
    
    // 在实际实现中，这里应该清除闪光效果状态
    std::cout << "Flash effect cleared" << std::endl;
}

bool GameBoardAdapter::isFlashEffectActive() const {
    if (!m_gameBoard) return false;
    
    // 在实际实现中，这里应该检查 GameBoard 的闪光效果状态
    // 目前返回 false 作为占位符
    return false;
}

CardColor GameBoardAdapter::getFlashEffectColor() const {
    if (!m_gameBoard) return CardColor::RED;
    
    // 在实际实现中，这里应该返回闪光效果的颜色
    return CardColor::RED;
}

std::vector<CardData> GameBoardAdapter::getPlayerHand(int playerId) const {
    std::vector<CardData> hand;
    
    if (!m_gameBoard || playerId < 0 || playerId >= getPlayerCount()) {
        return hand;
    }
    
    // 注意：GameBoard 目前没有直接提供获取玩家手牌的方法
    // 在实际实现中，我们需要扩展 GameBoard 或通过其他方式获取手牌信息
    
    return hand;
}

void GameBoardAdapter::discardPlayerCard(int playerId, int cardId) {
    if (!m_gameBoard) return;
    
    // 在实际实现中，这里应该从玩家手牌中移除指定卡牌
    std::cout << "Player " << playerId << " discards card " << cardId << std::endl;
}

void GameBoardAdapter::notifyCardEffect(const std::string& effect, const nlohmann::json& data) {
    if (!m_gameBoard) return;
    
    // 在实际实现中，这里应该通知所有玩家卡牌效果
    std::cout << "Card effect: " << effect << " with data: " << data.dump() << std::endl;
}

bool GameBoardAdapter::validateWildDrawFour(int playerId) const {
    if (!m_gameBoard || playerId < 0 || playerId >= getPlayerCount()) {
        return false;
    }
    
    // 调用 GameBoard 的验证方法
    return m_gameBoard->CanPlayWildDrawFour(playerId);
}

bool GameBoardAdapter::isSkillPhase() const {
    if (!m_gameBoard) return false;
    
    auto gameStat = m_gameBoard->GetGameStat();
    if (!gameStat) return false;
    
    return gameStat->GetCurrentPhase() == Game::GameStat::TurnPhase::SKILL;
}

bool GameBoardAdapter::isCardPlayPhase() const {
    if (!m_gameBoard) return false;
    
    auto gameStat = m_gameBoard->GetGameStat();
    if (!gameStat) return false;
    
    return gameStat->GetCurrentPhase() == Game::GameStat::TurnPhase::CARD_PLAY;
}

// 辅助方法实现
CardColor GameBoardAdapter::convertToCardColor(Game::CardColor color) const {
    switch (color) {
        case Game::CardColor::RED: return CardColor::RED;
        case Game::CardColor::YELLOW: return CardColor::YELLOW;
        case Game::CardColor::GREEN: return CardColor::GREEN;
        case Game::CardColor::BLUE: return CardColor::BLUE;
        case Game::CardColor::BLACK: return CardColor::WILD;
        default: return CardColor::WILD;
    }
}

Game::CardColor GameBoardAdapter::convertFromCardColor(CardColor color) const {
    switch (color) {
        case CardColor::RED: return Game::CardColor::RED;
        case CardColor::YELLOW: return Game::CardColor::YELLOW;
        case CardColor::GREEN: return Game::CardColor::GREEN;
        case CardColor::BLUE: return Game::CardColor::BLUE;
        case CardColor::WILD: return Game::CardColor::BLACK;
        default: return Game::CardColor::BLACK;
    }
}

CardType GameBoardAdapter::convertToCardType(Game::CardText text) const {
    switch (text) {
        case Game::CardText::NUMBER_0: case Game::CardText::NUMBER_1:
        case Game::CardText::NUMBER_2: case Game::CardText::NUMBER_3:
        case Game::CardText::NUMBER_4: case Game::CardText::NUMBER_5:
        case Game::CardText::NUMBER_6: case Game::CardText::NUMBER_7:
        case Game::CardText::NUMBER_8: case Game::CardText::NUMBER_9:
            return CardType::NUMBER;
            
        case Game::CardText::SKIP:
            return CardType::SKIP;
            
        case Game::CardText::REVERSE:
            return CardType::REVERSE;
            
        case Game::CardText::DRAW_TWO:
            return CardType::DRAW_TWO;
            
        case Game::CardText::WILD:
            return CardType::WILD;
            
        case Game::CardText::DRAW_FOUR:
            return CardType::WILD_DRAW_FOUR;
            
        // 自定义卡牌类型映射
        case Game::CardText::PACKAGE:
            return CardType::PACKAGE;
            
        case Game::CardText::FLASH:
            return CardType::FLASH;
            
        default:
            return CardType::NUMBER;
    }
}

std::shared_ptr<Card> GameBoardAdapter::createCardFromGameCard(const Game::Card& gameCard) const {
    // 提取数字（如果是数字卡）
    int number = -1;
    if (gameCard.mText >= Game::CardText::NUMBER_0 && gameCard.mText <= Game::CardText::NUMBER_9) {
        number = static_cast<int>(gameCard.mText) - static_cast<int>(Game::CardText::NUMBER_0);
    }
    
    // 创建 CardData
    CardData data(
        -1, // 需要从游戏卡牌获取真实ID
        convertToCardColor(gameCard.mColor),
        convertToCardType(gameCard.mText),
        number
    );
    
    // 根据类型创建具体卡牌对象
    switch (data.type) {
        case CardType::SKIP:
            return std::make_shared<SkipCard>(data.id, data.color);
        case CardType::REVERSE:
            return std::make_shared<ReverseCard>(data.id, data.color);
        case CardType::DRAW_TWO:
            return std::make_shared<DrawTwoCard>(data.id, data.color);
        case CardType::WILD:
            return std::make_shared<WildCard>(data.id);
        case CardType::WILD_DRAW_FOUR:
            return std::make_shared<WildDrawFourCard>(data.id);
        case CardType::PACKAGE:
            return std::make_shared<PackageCard>(data.id, data.color);
        case CardType::FLASH:
            return std::make_shared<FlashCard>(data.id);
        default:
            return std::make_shared<Card>(data);
    }
}

} // namespace UNO
