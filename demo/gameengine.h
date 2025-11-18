#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "player.h"
#include "card.h"
#include <vector>
#include <memory>
#include <random>

class GameEngine {
private:
    std::vector<std::unique_ptr<Player>> players;
    std::vector<Card> drawPile;
    std::vector<Card> discardPile;
    int currentPlayerIndex;
    bool gameDirection;
    bool gameStarted;
    std::mt19937 rng;
    CardColor chosenWildColor;
    CardColor currentPlayColor;

    void initializeDeck();
    void shuffleDeck();
    void dealInitialCards();
    void assignCharacters();
    Card drawFromDeck();
    void handleActionCard(const Card& card);
    void handleWildCard(const Card& card, CardColor chosenColor);
    int getNextPlayerIndex() const;
    
public:
    GameEngine();
    
    void addPlayer(const std::string& name);
    void startGame();
    bool isGameStarted() const { return gameStarted; }
    
    Card getCurrentCard() const;
    std::vector<Card> getPlayerHand(int playerId) const;
    std::vector<std::string> getPlayerCharacters() const;
    std::vector<int> getPlayerCardCounts() const;
    int getCurrentPlayerIndex() const { return currentPlayerIndex; }
    std::string getCurrentPlayerName() const;
    
    bool canPlayCard(int playerId, const Card& card) const;
    bool playCard(int playerId, const Card& card, CardColor chosenColor = CardColor::RED);
    
    bool canPlayMultipleCards(int playerId, const std::vector<Card>& cards) const;
    bool playMultipleCards(int playerId, const std::vector<Card>& cards, CardColor chosenColor = CardColor::RED);
    
    Card drawCard(int playerId);
    void nextTurn();
    
    bool canUseSkill(int playerId) const;
    void useSkill(int playerId);
    void resetSkillCooldowns();
    
    std::vector<Card> peekTopCards(int count);
    std::vector<Card> getDiscardPile() const { return discardPile; }
    bool isGameOver() const;
    int getWinner() const;
    
    void setChosenWildColor(CardColor color) { chosenWildColor = color; }
    CardColor getChosenWildColor() const { return chosenWildColor; }
    CardColor getCurrentPlayColor() const { return currentPlayColor; }
    void setCurrentPlayColor(CardColor color) { currentPlayColor = color; }
    
    void playCPUTurn();
};

#endif