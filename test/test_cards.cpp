#include <gtest/gtest.h>
#include <memory>
#include "Card.h"
#include "FunctionCard.h"

// 简单的测试用 GameState 实现
class TestGameState : public UNO::GameState {
public:
    std::shared_ptr<UNO::Card> getTopCard() const override { 
        return std::make_shared<UNO::Card>(UNO::CardData(1, UNO::CardColor::RED, UNO::CardType::NUMBER, 5));
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

class CardTest : public ::testing::Test {
protected:
    void SetUp() override {
        numberCard = std::make_shared<UNO::Card>(UNO::CardData(1, UNO::CardColor::RED, UNO::CardType::NUMBER, 5));
        skipCard = std::make_shared<UNO::SkipCard>(2, UNO::CardColor::BLUE);
        wildCard = std::make_shared<UNO::WildCard>(3);
        gameState = std::make_shared<TestGameState>();
    }

    std::shared_ptr<UNO::Card> numberCard;
    std::shared_ptr<UNO::SkipCard> skipCard;
    std::shared_ptr<UNO::WildCard> wildCard;
    std::shared_ptr<TestGameState> gameState;
};

TEST_F(CardTest, NumberCardCreation) {
    EXPECT_EQ(numberCard->getType(), UNO::CardType::NUMBER);
    EXPECT_EQ(numberCard->getColor(), UNO::CardColor::RED);
    EXPECT_EQ(numberCard->getNumber(), 5);
    EXPECT_EQ(numberCard->getName(), "5");
}

TEST_F(CardTest, FunctionCardCreation) {
    EXPECT_EQ(skipCard->getType(), UNO::CardType::SKIP);
    EXPECT_EQ(skipCard->getColor(), UNO::CardColor::BLUE);
    EXPECT_EQ(skipCard->getName(), "Skip");
}

TEST_F(CardTest, CardMatching) {
    auto redSeven = std::make_shared<UNO::Card>(UNO::CardData(4, UNO::CardColor::RED, UNO::CardType::NUMBER, 7));
    
    // 相同颜色应该匹配
    EXPECT_TRUE(numberCard->canPlayOn(redSeven.get()));
    
    auto blueFive = std::make_shared<UNO::Card>(UNO::CardData(5, UNO::CardColor::BLUE, UNO::CardType::NUMBER, 5));
    
    // 相同数字应该匹配
    EXPECT_TRUE(numberCard->canPlayOn(blueFive.get()));
}

TEST_F(CardTest, WildCardProperties) {
    EXPECT_EQ(wildCard->getType(), UNO::CardType::WILD);
    EXPECT_TRUE(wildCard->requiresParameters());
    
    auto hint = wildCard->getParameterHint();
    EXPECT_TRUE(hint.contains("type"));
    EXPECT_EQ(hint["type"], "color_selection");
}

TEST_F(CardTest, WildCardValidation) {
    nlohmann::json validParams = {{"color", 0}}; // RED
    nlohmann::json invalidParams = {{"color", 5}}; // 无效颜色
    nlohmann::json missingParams = {}; // 缺少参数
    
    EXPECT_TRUE(wildCard->validateParameters(validParams));
    EXPECT_FALSE(wildCard->validateParameters(invalidParams));
    EXPECT_FALSE(wildCard->validateParameters(missingParams));
}

TEST_F(CardTest, SkipCardEffect) {
    // 测试跳过卡效果不会崩溃
    EXPECT_NO_THROW({
        nlohmann::json params;
        skipCard->applyEffect(*gameState, 0, params);
    });
}

TEST_F(CardTest, PackageCardCreation) {
    auto packageCard = std::make_shared<UNO::PackageCard>(10, UNO::CardColor::GREEN);
    EXPECT_EQ(packageCard->getType(), UNO::CardType::PACKAGE);
    EXPECT_TRUE(packageCard->requiresParameters());
}

TEST_F(CardTest, FlashCardCreation) {
    auto flashCard = std::make_shared<UNO::FlashCard>(11);
    EXPECT_EQ(flashCard->getType(), UNO::CardType::FLASH);
    EXPECT_TRUE(flashCard->requiresParameters());
}
