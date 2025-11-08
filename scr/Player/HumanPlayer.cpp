#include "HumanPlayer.h"
#include "GameState.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace UNO {

HumanPlayer::HumanPlayer(int id, const std::string& name) 
    : Player(id, name, false), selectedCardIndex(-1), useSkillDecision(false),
      wildColorChoice(CardColor::RED), inputReceived(false) {}

std::shared_ptr<Card> HumanPlayer::playTurn(GameState& gameState) {
    resetInputState();
    
    // 等待用户输入
    waitForInput();
    
    if (selectedCardIndex >= 0 && selectedCardIndex < static_cast<int>(handCards.size())) {
        return handCards[selectedCardIndex];
    }
    
    return nullptr; // 用户选择不出牌或抽牌
}

bool HumanPlayer::decideUseSkill(GameState& gameState) {
    resetInputState();
    waitForInput();
    return useSkillDecision;
}

nlohmann::json HumanPlayer::getSkillParameters(GameState& gameState) {
    return skillParams;
}

CardColor HumanPlayer::chooseWildColor(GameState& gameState) {
    resetInputState();
    waitForInput();
    return wildColorChoice;
}

void HumanPlayer::setCardChoice(int cardIndex) {
    selectedCardIndex = cardIndex;
    inputReceived = true;
}

void HumanPlayer::setSkillUsage(bool useSkill) {
    useSkillDecision = useSkill;
    inputReceived = true;
}

void HumanPlayer::setSkillParameters(const nlohmann::json& params) {
    skillParams = params;
    inputReceived = true;
}

void HumanPlayer::setWildColorChoice(CardColor color) {
    wildColorChoice = color;
    inputReceived = true;
}

void HumanPlayer::waitForInput() {
    // 在实际游戏中，这里会等待UI的输入事件
    // 对于控制台版本，可以使用阻塞输入
    // 对于GUI版本，这里应该等待UI回调
    
    // 模拟等待输入（实际实现取决于UI系统）
    while (!inputReceived) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void HumanPlayer::resetInputState() {
    selectedCardIndex = -1;
    useSkillDecision = false;
    skillParams = nlohmann::json::object();
    wildColorChoice = CardColor::RED;
    inputReceived = false;
}

} // namespace UNO
