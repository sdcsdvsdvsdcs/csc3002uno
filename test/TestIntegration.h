#pragma once
#include <memory>
#include <vector>
#include <iostream>
#include "GameState.h"
#include "CharacterManager.h"
#include "Player.h"
#include "AIPlayer.h"
#include "Deck.h"
#include "cards/FunctionCard.h"

namespace UNO {

class IntegrationTest {
private:
    std::shared_ptr<GameState> gameState;
    std::shared_ptr<CharacterManager> characterManager;
    std::shared_ptr<Deck> deck;
    std::vector<std::shared_ptr<Player>> players;

public:
    IntegrationTest();
    
    // 测试方法
    void testCardSystem();
    void testPlayerSystem();
    void testRoleSystem();
    void testGameFlow();
    void runAllTests();
    
    // 辅助方法
    void setupBasicGame();
    void printGameState();
    void printPlayerState(int playerId);
};

} // namespace UNO
