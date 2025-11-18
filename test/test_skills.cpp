#include <gtest/gtest.h>
#include <memory>
#include "stat.h"

// 简单的测试用 GameBoard
class TestGameBoard {
public:
    void SomeMethod() {} // 简单实现以满足编译
};

class SkillTest : public ::testing::Test {
protected:
    void SetUp() override {
        luckyStar = std::make_unique<UNO::Game::LuckyStar>();
        collector = std::make_unique<UNO::Game::Collector>();
        thief = std::make_unique<UNO::Game::Thief>();
        defender = std::make_unique<UNO::Game::Defender>();
        
        // 创建测试玩家状态
        playerStat = std::make_unique<UNO::Game::PlayerStat>("TestPlayer", 5);
    }

    std::unique_ptr<UNO::Game::LuckyStar> luckyStar;
    std::unique_ptr<UNO::Game::Collector> collector;
    std::unique_ptr<UNO::Game::Thief> thief;
    std::unique_ptr<UNO::Game::Defender> defender;
    std::unique_ptr<UNO::Game::PlayerStat> playerStat;
    TestGameBoard gameBoard;
};

TEST_F(SkillTest, CharacterCreation) {
    EXPECT_EQ(luckyStar->GetType(), UNO::Game::CharacterType::LUCKY_STAR);
    EXPECT_EQ(luckyStar->GetName(), "Lucky Star");
    
    EXPECT_EQ(collector->GetType(), UNO::Game::CharacterType::COLLECTOR);
    EXPECT_EQ(collector->GetName(), "Collector");
    
    EXPECT_EQ(thief->GetType(), UNO::Game::CharacterType::THIEF);
    EXPECT_EQ(thief->GetName(), "Thief");
    
    EXPECT_EQ(defender->GetType(), UNO::Game::CharacterType::DEFENDER);
    EXPECT_EQ(defender->GetName(), "Defender");
}

TEST_F(SkillTest, SkillAvailability) {
    // 初始状态下技能应该可用（除了Defender）
    EXPECT_TRUE(luckyStar->CanUseSkill());
    EXPECT_TRUE(collector->CanUseSkill());
    EXPECT_TRUE(thief->CanUseSkill());
    EXPECT_FALSE(defender->CanUseSkill()); // Defender是被动技能
}

TEST_F(SkillTest, SkillUsage) {
    // 测试使用技能
    EXPECT_NO_THROW({
        luckyStar->UseSkill();
        collector->UseSkill();
        thief->UseSkill();
        defender->UseSkill(); // Defender使用技能应该不会做任何事情
    });
    
    // 使用后应该进入冷却或减少使用次数
    EXPECT_TRUE(luckyStar->IsInCooldown());
    EXPECT_TRUE(collector->IsInCooldown());
    EXPECT_TRUE(thief->IsInCooldown());
    
    EXPECT_EQ(luckyStar->GetUsesRemaining(), 2); // 初始3次，用了1次
    EXPECT_EQ(collector->GetUsesRemaining(), 0); // 每回合只能用1次
    EXPECT_EQ(thief->GetUsesRemaining(), 0);     // 每回合只能用1次
}

TEST_F(SkillTest, CooldownMechanics) {
    luckyStar->UseSkill();
    
    EXPECT_TRUE(luckyStar->IsInCooldown());
    EXPECT_EQ(luckyStar->GetCooldown(), 1);
    
    // 模拟冷却更新
    luckyStar->UpdateCooldown();
    
    EXPECT_EQ(luckyStar->GetCooldown(), 0);
    EXPECT_FALSE(luckyStar->IsInCooldown());
}

TEST_F(SkillTest, SkillReset) {
    luckyStar->UseSkill();
    collector->UseSkill();
    
    // 重置后应该恢复
    luckyStar->ResetForNewRound();
    collector->ResetForNewRound();
    
    EXPECT_FALSE(luckyStar->IsInCooldown());
    EXPECT_EQ(luckyStar->GetCooldown(), 0);
    EXPECT_EQ(collector->GetUsesRemaining(), 1); // Collector重置后恢复使用次数
}

TEST_F(SkillTest, DefenderMechanics) {
    // Defender 应该能够尝试防御
    EXPECT_TRUE(defender->TryDefend());
    EXPECT_TRUE(defender->IsInCooldown());
    
    // 在冷却中不能再次防御
    EXPECT_FALSE(defender->TryDefend());
}

TEST_F(SkillTest, CharacterFactory) {
    // 测试角色工厂
    auto lucky = UNO::Game::CharacterFactory::CreateCharacter(UNO::Game::CharacterType::LUCKY_STAR);
    auto collectorChar = UNO::Game::CharacterFactory::CreateCharacter(UNO::Game::CharacterType::COLLECTOR);
    auto thiefChar = UNO::Game::CharacterFactory::CreateCharacter(UNO::Game::CharacterType::THIEF);
    auto defenderChar = UNO::Game::CharacterFactory::CreateCharacter(UNO::Game::CharacterType::DEFENDER);
    
    EXPECT_NE(lucky, nullptr);
    EXPECT_NE(collectorChar, nullptr);
    EXPECT_NE(thiefChar, nullptr);
    EXPECT_NE(defenderChar, nullptr);
    
    EXPECT_EQ(lucky->GetType(), UNO::Game::CharacterType::LUCKY_STAR);
    EXPECT_EQ(collectorChar->GetType(), UNO::Game::CharacterType::COLLECTOR);
}

TEST_F(SkillTest, RandomCharacterGeneration) {
    // 测试随机角色生成
    auto charType = UNO::Game::CharacterFactory::GetRandomCharacter();
    
    // 应该返回有效的角色类型
    EXPECT_TRUE(charType == UNO::Game::CharacterType::LUCKY_STAR ||
                charType == UNO::Game::CharacterType::COLLECTOR ||
                charType == UNO::Game::CharacterType::THIEF ||
                charType == UNO::Game::CharacterType::DEFENDER);
}

TEST_F(SkillTest, CharacterNameLookup) {
    EXPECT_EQ(UNO::Game::CharacterFactory::GetCharacterName(UNO::Game::CharacterType::LUCKY_STAR), "Lucky Star");
    EXPECT_EQ(UNO::Game::CharacterFactory::GetCharacterName(UNO::Game::CharacterType::COLLECTOR), "Collector");
    EXPECT_EQ(UNO::Game::CharacterFactory::GetCharacterName(UNO::Game::CharacterType::THIEF), "Thief");
    EXPECT_EQ(UNO::Game::CharacterFactory::GetCharacterName(UNO::Game::CharacterType::DEFENDER), "Defender");
    EXPECT_EQ(UNO::Game::CharacterFactory::GetCharacterName(UNO::Game::CharacterType::NONE), "None");
}
