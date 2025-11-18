#include <gtest/gtest.h>
#include <memory>
#include "Player.h"
#include "AIPlayer.h"
#include "HumanPlayer.h"

class PlayerTest : public ::testing::Test {
protected:
    void SetUp() override {
        humanPlayer = std::make_shared<UNO::HumanPlayer>(1, "TestPlayer");
        aiPlayer = std::make_shared<UNO::AIPlayer>(2, "TestAI");
        
        // 添加一些测试卡牌到玩家手牌
        auto card1 = std::make_shared<UNO::Card>(UNO::CardData(1, UNO::CardColor::RED, UNO::CardType::NUMBER, 5));
        auto card2 = std::make_shared<UNO::Card>(UNO::CardData(2, UNO::CardColor::BLUE, UNO::CardType::NUMBER, 3));
        
        humanPlayer->addCardToHand(card1);
        aiPlayer->addCardToHand(card2);
    }

    std::shared_ptr<UNO::HumanPlayer> humanPlayer;
    std::shared_ptr<UNO::AIPlayer> aiPlayer;
};

TEST_F(PlayerTest, PlayerCreation) {
    EXPECT_EQ(humanPlayer->getId(), 1);
    EXPECT_EQ(humanPlayer->getName(), "TestPlayer");
    EXPECT_FALSE(humanPlayer->isAI());
    
    EXPECT_EQ(aiPlayer->getId(), 2);
    EXPECT_EQ(aiPlayer->getName(), "TestAI");
    EXPECT_TRUE(aiPlayer->isAI());
}

TEST_F(PlayerTest, HandManagement) {
    EXPECT_EQ(humanPlayer->getHandSize(), 1);
    EXPECT_TRUE(humanPlayer->hasCard(1));
    EXPECT_FALSE(humanPlayer->hasCard(999)); // 不存在的卡牌
    
    // 测试移除卡牌
    EXPECT_TRUE(humanPlayer->removeCardFromHand(1));
    EXPECT_EQ(humanPlayer->getHandSize(), 0);
    EXPECT_FALSE(humanPlayer->hasCard(1));
    
    // 测试移除不存在的卡牌
    EXPECT_FALSE(humanPlayer->removeCardFromHand(999));
}

TEST_F(PlayerTest, PlayerSerialization) {
    auto json = humanPlayer->toJson();
    
    EXPECT_TRUE(json.contains("id"));
    EXPECT_TRUE(json.contains("name"));
    EXPECT_TRUE(json.contains("isAI"));
    EXPECT_TRUE(json.contains("handCards"));
    
    EXPECT_EQ(json["id"], 1);
    EXPECT_EQ(json["name"], "TestPlayer");
    EXPECT_EQ(json["isAI"], false);
}

TEST_F(PlayerTest, AIPlayerDecision) {
    // 创建简单的游戏状态用于测试
    class SimpleGameState : public UNO::GameState {
    public:
        std::shared_ptr<UNO::Card> getTopCard() const override { 
            return std::make_shared<UNO::Card>(UNO::CardData(99, UNO::CardColor::RED, UNO::CardType::NUMBER, 5));
        }
        CardColor getCurrentColor() const override { return UNO::CardColor::RED; }
        int getCurrentPlayer() const override { return 0; }
        int getNextPlayerId() const override { return 1; }
        int getPlayerCount() const override { return 2; }
        bool isClockwise() const override { return true; }
        void makePlayerDrawCards(int playerId, int count) override {}
        void skipPlayerTurn(int playerId) override {}
        void reversePlayDirection() override {}
        void setCurrentColor(CardColor color) override {}
        void setFlashEffect(CardColor color, int playerId) override {}
        void clearFlashEffect() override {}
        bool isFlashEffectActive() const override { return false; }
        CardColor getFlashEffectColor() const override { return UNO::CardColor::RED; }
        std::vector<UNO::CardData> getPlayerHand(int playerId) const override { return {}; }
        void discardPlayerCard(int playerId, int cardId) override {}
        void notifyCardEffect(const std::string& effect, const nlohmann::json& data) override {}
        bool validateWildDrawFour(int playerId) const override { return true; }
        bool isSkillPhase() const override { return false; }
        bool isCardPlayPhase() const override { return true; }
        std::vector<std::shared_ptr<UNO::Card>> getPlayerHandCards(int playerId) const override { return {}; }
    };
    
    SimpleGameState gameState;
    
    // 测试AI决策不会崩溃
    EXPECT_NO_THROW({
        auto skillDecision = aiPlayer->decideUseSkill(gameState);
        auto skillParams = aiPlayer->getSkillParameters(gameState);
        auto colorChoice = aiPlayer->chooseWildColor(gameState);
        auto playDecision = aiPlayer->playTurn(gameState);
    });
}

TEST_F(PlayerTest, ScoreManagement) {
    EXPECT_EQ(humanPlayer->getScore(), 0);
    
    humanPlayer->addScore(10);
    EXPECT_EQ(humanPlayer->getScore(), 10);
    
    humanPlayer->addScore(5);
    EXPECT_EQ(humanPlayer->getScore(), 15);
    
    humanPlayer->resetScore();
    EXPECT_EQ(humanPlayer->getScore(), 0);
}

TEST_F(PlayerTest, UNOStatus) {
    // 初始状态不应该有UNO
    EXPECT_FALSE(humanPlayer->hasUno());
    
    // 当手牌只剩一张时应该有UNO状态
    humanPlayer->setUno(true);
    EXPECT_TRUE(humanPlayer->hasUno());
    
    humanPlayer->setUno(false);
    EXPECT_FALSE(humanPlayer->hasUno());
}
