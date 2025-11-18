#include "gameengine.h"
#include <algorithm>
#include <random>
#include <stdexcept>
#include <iostream>

GameEngine::GameEngine() : currentPlayerIndex(0), gameDirection(true), 
                          gameStarted(false), chosenWildColor(CardColor::RED),
                          currentPlayColor(CardColor::RED) {
    std::random_device rd;
    rng.seed(rd());
}

void GameEngine::initializeDeck() {
    drawPile.clear();
    
    for (int color = 0; color < 4; color++) {
        CardColor cardColor = static_cast<CardColor>(color);
        
        drawPile.push_back(Card(cardColor, CardValue::ZERO));
        
        for (int value = 1; value <= 9; value++) {
            CardValue cardValue = static_cast<CardValue>(value);
            drawPile.push_back(Card(cardColor, cardValue));
            drawPile.push_back(Card(cardColor, cardValue));
        }
        
        drawPile.push_back(Card(cardColor, CardValue::SKIP));
        drawPile.push_back(Card(cardColor, CardValue::SKIP));
        drawPile.push_back(Card(cardColor, CardValue::REVERSE));
        drawPile.push_back(Card(cardColor, CardValue::REVERSE));
        drawPile.push_back(Card(cardColor, CardValue::DRAW_TWO));
        drawPile.push_back(Card(cardColor, CardValue::DRAW_TWO));
        drawPile.push_back(Card(cardColor, CardValue::PACKAGE));
        drawPile.push_back(Card(cardColor, CardValue::PACKAGE));
    }
    
    for (int i = 0; i < 4; i++) {
        drawPile.push_back(Card(CardColor::WILD, CardValue::WILD));
    }
    
    for (int i = 0; i < 2; i++) {
        drawPile.push_back(Card(CardColor::WILD, CardValue::WILD_DRAW_FOUR));
    }
    
    for (int i = 0; i < 2; i++) {
        drawPile.push_back(Card(CardColor::WILD, CardValue::FLASH));
    }
}

void GameEngine::shuffleDeck() {
    std::shuffle(drawPile.begin(), drawPile.end(), rng);
}

void GameEngine::dealInitialCards() {
    for (auto& player : players) {
        for (int i = 0; i < 7; i++) {
            player->addCard(drawFromDeck());
        }
    }
}

void GameEngine::assignCharacters() {
    std::vector<CharacterType> characters = {
        CharacterType::LUCKY_STAR,
        CharacterType::COLLECTOR, 
        CharacterType::THIEF,
        CharacterType::DEFENDER
    };
    
    std::shuffle(characters.begin(), characters.end(), rng);
    
    for (size_t i = 0; i < players.size() && i < characters.size(); i++) {
        players[i]->setCharacter(characters[i]);
    }
}

Card GameEngine::drawFromDeck() {
    if (drawPile.empty()) {
        throw std::runtime_error("Draw pile is empty!");
    }
    
    Card card = drawPile.back();
    drawPile.pop_back();
    return card;
}

void GameEngine::addPlayer(const std::string& name) {
    if (gameStarted) return;
    
    int playerId = players.size();
    players.push_back(std::make_unique<Player>(playerId, name));
}

void GameEngine::startGame() {
    if (players.size() < 2) return;
    
    initializeDeck();
    shuffleDeck();
    assignCharacters();
    dealInitialCards();
    
    if (!drawPile.empty()) {
        discardPile.push_back(drawPile.back());
        drawPile.pop_back();
    }
    
    gameStarted = true;
    currentPlayerIndex = 0;
}

Card GameEngine::getCurrentCard() const {
    if (discardPile.empty()) return Card();
    return discardPile.back();
}

std::vector<Card> GameEngine::getPlayerHand(int playerId) const {
    if (playerId >= 0 && playerId < players.size()) {
        return players[playerId]->getHand();
    }
    return {};
}

std::vector<std::string> GameEngine::getPlayerCharacters() const {
    std::vector<std::string> characters;
    for (const auto& player : players) {
        characters.push_back(player->getCharacterString());
    }
    return characters;
}

std::vector<int> GameEngine::getPlayerCardCounts() const {
    std::vector<int> counts;
    for (const auto& player : players) {
        counts.push_back(player->getHandSize());
    }
    return counts;
}

std::string GameEngine::getCurrentPlayerName() const {
    if (currentPlayerIndex < players.size()) {
        return players[currentPlayerIndex]->getName();
    }
    return "";
}

bool GameEngine::canPlayCard(int playerId, const Card& card) const {
    if (playerId != currentPlayerIndex) return false;
    
    Card currentCard = getCurrentCard();
    
    // Special restriction for Wild Draw Four
    if (card.value == CardValue::WILD_DRAW_FOUR) {
        Player* player = players[playerId].get();
        if (player->hasColorCard(currentPlayColor)) {
            return false; // Cannot play Wild Draw Four if you have matching color
        }
    }
    
    // If current card is wild, check against the chosen color
    if (currentCard.color == CardColor::WILD) {
        return card.color == currentPlayColor || card.value == currentCard.value;
    }
    
    return card.canPlayOn(currentCard);
}

bool GameEngine::playCard(int playerId, const Card& card, CardColor chosenColor) {
    if (!canPlayCard(playerId, card)) return false;
    
    Player* player = players[playerId].get();
    if (!player->removeCard(card)) return false;
    
    discardPile.push_back(card);
    
    // Set the current play color based on card type
    if (card.isWildCard()) {
        // For wild cards, use the chosen color
        currentPlayColor = chosenColor;
        chosenWildColor = chosenColor;
        handleWildCard(card, chosenColor);
    } else {
        // For normal cards, use the card's actual color
        currentPlayColor = card.color;
        if (card.isActionCard()) {
            handleActionCard(card);
        }
    }
    
    return true;
}

Card GameEngine::drawCard(int playerId) {
    if (playerId >= 0 && playerId < players.size()) {
        Card card = drawFromDeck();
        players[playerId]->addCard(card);
        return card;
    }
    return Card();
}

void GameEngine::nextTurn() {
    if (gameDirection) {
        currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
    } else {
        currentPlayerIndex = (currentPlayerIndex - 1 + players.size()) % players.size();
    }
}

void GameEngine::handleActionCard(const Card& card) {
    switch(card.value) {
        case CardValue::SKIP:
            nextTurn(); // Skip next player
            break;
        case CardValue::REVERSE:
            gameDirection = !gameDirection;
            break;
        case CardValue::DRAW_TWO:
            {
                int nextPlayer = getNextPlayerIndex();
                drawCard(nextPlayer);
                drawCard(nextPlayer);
                nextTurn(); // Skip their turn
            }
            break;
        case CardValue::PACKAGE:
            // Package card logic would go here
            break;
        default:
            break;
    }
}

void GameEngine::handleWildCard(const Card& card, CardColor chosenColor) {
    switch(card.value) {
        case CardValue::WILD_DRAW_FOUR:
            {
                int nextPlayer = getNextPlayerIndex();
                for (int i = 0; i < 4; i++) {
                    drawCard(nextPlayer);
                }
                nextTurn(); // Skip their turn
            }
            break;
        case CardValue::FLASH:
            // Flash card logic would go here
            break;
        default:
            break;
    }
    chosenWildColor = chosenColor;
    currentPlayColor = chosenColor;
}

int GameEngine::getNextPlayerIndex() const {
    if (gameDirection) {
        return (currentPlayerIndex + 1) % players.size();
    } else {
        return (currentPlayerIndex - 1 + players.size()) % players.size();
    }
}

bool GameEngine::canUseSkill(int playerId) const {
    if (playerId >= 0 && playerId < players.size()) {
        return players[playerId]->canUseSkill();
    }
    return false;
}

void GameEngine::useSkill(int playerId) {
    if (canUseSkill(playerId)) {
        players[playerId]->useSkill();
    }
}

void GameEngine::resetSkillCooldowns() {
    for (auto& player : players) {
        player->resetSkillCooldown();
    }
}

std::vector<Card> GameEngine::peekTopCards(int count) {
    std::vector<Card> result;
    for (int i = 0; i < count && i < drawPile.size(); i++) {
        result.push_back(drawPile[drawPile.size() - 1 - i]);
    }
    return result;
}

bool GameEngine::isGameOver() const {
    for (const auto& player : players) {
        if (player->getHandSize() == 0) {
            return true;
        }
    }
    return false;
}

int GameEngine::getWinner() const {
    for (size_t i = 0; i < players.size(); i++) {
        if (players[i]->getHandSize() == 0) {
            return i;
        }
    }
    return -1;
}

bool GameEngine::canPlayMultipleCards(int playerId, const std::vector<Card>& cards) const {
    if (playerId != currentPlayerIndex || cards.empty()) {
        return false;
    }
    
    // First card must be playable on current card
    if (!canPlayCard(playerId, cards[0])) {
        return false;
    }
    
    // All subsequent cards must have the same value as the first card
    CardValue firstValue = cards[0].value;
    for (size_t i = 1; i < cards.size(); i++) {
        if (cards[i].value != firstValue) {
            return false;
        }
    }
    
    return true;
}

bool GameEngine::playMultipleCards(int playerId, const std::vector<Card>& cards, CardColor chosenColor) {
    if (!canPlayMultipleCards(playerId, cards)) {
        return false;
    }
    
    Player* player = players[playerId].get();
    
    // Remove all cards from player's hand
    for (const Card& card : cards) {
        if (!player->removeCard(card)) {
            return false; // Card not found in hand
        }
        discardPile.push_back(card);
    }
    
    // Handle the played cards
    Card firstCard = cards[0];
    if (firstCard.isWildCard()) {
        handleWildCard(firstCard, chosenColor);
        currentPlayColor = chosenColor;
    } else if (firstCard.isActionCard()) {
        handleActionCard(firstCard);
        currentPlayColor = firstCard.color;
    } else {
        currentPlayColor = firstCard.color;
    }
    
    return true;
}

void GameEngine::playCPUTurn() {
    if (currentPlayerIndex == 0) return; // Skip if it's human player's turn
    
    Player* cpuPlayer = players[currentPlayerIndex].get();
    
    // CPU might use skill (simple 30% chance if skill is available)
    if (cpuPlayer->canUseSkill() && (std::uniform_int_distribution<>(1, 100)(rng) <= 30)) {
        useSkill(currentPlayerIndex);
        std::cout << cpuPlayer->getName() << " used skill!" << std::endl;
    }
    
    std::vector<Card> cpuHand = cpuPlayer->getHand();
    Card currentCard = getCurrentCard();
    
    // Try to find a playable card
    for (const Card& card : cpuHand) {
        if (canPlayCard(currentPlayerIndex, card)) {
            // CPU plays the first playable card it finds
            if (card.isWildCard()) {
                // CPU randomly chooses a color
                CardColor randomColor = static_cast<CardColor>(std::uniform_int_distribution<>(0, 3)(rng));
                playCard(currentPlayerIndex, card, randomColor);
            } else {
                playCard(currentPlayerIndex, card);
            }
            
            std::cout << cpuPlayer->getName() << " played: " 
                      << card.getColorString() << " " << card.getValueString() << std::endl;
            return;
        }
    }
    
    // If no playable card, draw a card
    Card drawnCard = drawCard(currentPlayerIndex);
    std::cout << cpuPlayer->getName() << " drew a card" << std::endl;
    
    // Check if the drawn card can be played immediately
    if (canPlayCard(currentPlayerIndex, drawnCard)) {
        if (drawnCard.isWildCard()) {
            CardColor randomColor = static_cast<CardColor>(std::uniform_int_distribution<>(0, 3)(rng));
            playCard(currentPlayerIndex, drawnCard, randomColor);
        } else {
            playCard(currentPlayerIndex, drawnCard);
        }
        std::cout << cpuPlayer->getName() << " played the drawn card: " 
                  << drawnCard.getColorString() << " " << drawnCard.getValueString() << std::endl;
    }
}