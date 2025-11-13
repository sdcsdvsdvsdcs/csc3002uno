#pragma once

#include <string>
#include <memory>
#include "info.h"
#include "../common/util.h"

namespace UNO { namespace Game {

using namespace Network;

// 角色类型枚举
enum class CharacterType {
    LUCKY_STAR,
    COLLECTOR,
    THIEF,
    DEFENDER,
    NONE
};

// 前向声明
class Character;

class GameStat {
public:
    /// 回合阶段
    enum class TurnPhase {
        START,       // 回合开始阶段
        SKILL,       // 技能使用阶段  
        CARD_PLAY,   // 出牌阶段
        END          // 回合结束阶段
    };

    /// constructor for \c Player
    GameStat(const GameStartInfo &info);

    /// constructor for \c GameBoard
    GameStat(int firstPlayer, Card flippedCard);
    
    void NextPlayer();

    void UpdateAfterDraw();

    void UpdateAfterSkip();

    void UpdateAfterPlay(Card card);

    void Tick();

    void AdvancePhase();

    bool IsMyTurn() const { return mCurrentPlayer == 0; }

    bool IsSkipped() const { return mLastPlayedCard.mText == CardText::SKIP; }

    int GetCurrentPlayer() const { return mCurrentPlayer; }

    bool IsInClockwise() const { return mIsInClockwise; }

    bool DoesGameEnd() const { return mGameEnds; }

    int GetTimeElapsed() const { return mTimeElapsed; }

    Card GetLastPlayedCard() const { return mLastPlayedCard; }

    int GetCardsNumToDraw() const { return mCardsNumToDraw; }

    TurnPhase GetCurrentPhase() const { return mCurrentPhase; }

    void SetCurrentPhase(TurnPhase phase) { mCurrentPhase = phase; }

    void GameEnds() { mGameEnds = true; mCurrentPlayer = -1; }

    void Reverse() { mIsInClockwise = !mIsInClockwise; }

    // 新增：检查是否处于特殊效果状态（用于Flash卡等）
    bool IsSpecialEffectActive() const { return mSpecialEffectActive; }
    void SetSpecialEffectActive(bool active) { mSpecialEffectActive = active; }

    // for tests
    void SetCurrentPlayer(int currentPlayer) { mCurrentPlayer = currentPlayer; }
    
    void SetIsInClockwise(bool isInClockwise) { mIsInClockwise = isInClockwise; }

    void SetLastPlayedCard(Card lastPlayedCard) { mLastPlayedCard = lastPlayedCard; }

    void SetCardsNumToDraw(int cardsNumToDraw) { mCardsNumToDraw = cardsNumToDraw; }

private:
    int mCurrentPlayer;
    bool mIsInClockwise;
    bool mGameEnds{false};
    int mTimeElapsed{0};
    TurnPhase mCurrentPhase{TurnPhase::START};
    bool mSpecialEffectActive{false}; // 新增：标记特殊效果状态

    // currently the two fields below are not used by GameStat of GameBoard
    Card mLastPlayedCard{};
    int mCardsNumToDraw{1};  // +2 and +4 can accumulate
};

class PlayerStat {
public:
    PlayerStat() {}
    explicit PlayerStat(const std::string& username, int remainingHandCardsNum);

    void UpdateAfterDraw(int number, int indexOfNewlyDrawn = -1);
    
    void UpdateAfterSkip();
    
    void UpdateAfterPlay(Card card);

    // 角色相关方法
    void AssignCharacter(std::unique_ptr<Character> character);
    Character* GetCharacter() const { return mCharacter.get(); }
    bool HasCharacter() const { return mCharacter != nullptr; }
    CharacterType GetCharacterType() const;
    std::string GetCharacterName() const;

    // 技能管理
    bool CanUseSkill() const;
    void UseSkill();
    void UpdateCharacterCooldown();
    void ResetCharacterForNewRound();

    // 新增：获取玩家手牌信息（用于PackageCard等效果）
    int GetHandCardCount() const { return mRemainingHandCardsNum; }

    std::string GetUsername() const { return mUsername; }

    int GetRemainingHandCardsNum() const { return mRemainingHandCardsNum;  }

    bool DoPlayInLastRound() const { return mDoPlayInLastRound; }

    Card GetLastPlayedCard() const { return mLastPlayedCard; }

    bool HasChanceToPlayAfterDraw() const { return mHasChanceToPlayAfterDraw; }

    int GetIndexOfNewlyDrawn() const { return mIndexOfNewlyDrawn; }

    // for test
    void SetLastPlayedCard(Card lastPlayedCard) { mLastPlayedCard = lastPlayedCard; }

private:
    const std::string mUsername;
    std::unique_ptr<Character> mCharacter;  // 角色指针

    int mRemainingHandCardsNum;
    bool mDoPlayInLastRound{false};
    Card mLastPlayedCard{};
    bool mHasChanceToPlayAfterDraw{false};
    int mIndexOfNewlyDrawn{-1};

    friend std::ostream& operator<<(std::ostream& os, const PlayerStat& stat);
};

// 角色基类
class Character {
public:
    Character(CharacterType type, const std::string& name);
    virtual ~Character() = default;

    CharacterType GetType() const { return mType; }
    std::string GetName() const { return mName; }
    int GetUsesRemaining() const { return mUsesRemaining; }
    bool IsInCooldown() const { return mIsInCooldown; }
    int GetCooldown() const { return mCooldown; }
    
    // 技能使用限制
    virtual bool CanUseSkill() const = 0;
    virtual void UseSkill() = 0;
    virtual void ResetForNewRound() = 0;
    virtual void UpdateCooldown() = 0;

    // 新增：技能效果应用方法
    virtual void ApplySkillEffect(GameStat& gameStat, PlayerStat& playerStat) {}

protected:
    CharacterType mType;
    std::string mName;
    int mUsesRemaining{0};
    int mCooldown{0};
    bool mIsInCooldown{false};
};

// Lucky Star 角色
class LuckyStar : public Character {
public:
    LuckyStar();
    bool CanUseSkill() const override;
    void UseSkill() override;
    void ResetForNewRound() override;
    void UpdateCooldown() override;
    void ApplySkillEffect(GameStat& gameStat, PlayerStat& playerStat) override;

private:
    static const int MAX_USES = 3;
};

// Collector 角色
class Collector : public Character {
public:
    Collector();
    bool CanUseSkill() const override;
    void UseSkill() override;
    void ResetForNewRound() override;
    void UpdateCooldown() override;
    void ApplySkillEffect(GameStat& gameStat, PlayerStat& playerStat) override;
};

// Thief 角色
class Thief : public Character {
public:
    Thief();
    bool CanUseSkill() const override;
    void UseSkill() override;
    void ResetForNewRound() override;
    void UpdateCooldown() override;
    void ApplySkillEffect(GameStat& gameStat, PlayerStat& playerStat) override;
};

// Defender 角色
class Defender : public Character {
public:
    Defender();
    bool CanUseSkill() const override;
    void UseSkill() override;
    void ResetForNewRound() override;
    void UpdateCooldown() override;
    
    // 防御者特有方法
    bool TryDefend();
};

// 角色工厂
class CharacterFactory {
public:
    static std::unique_ptr<Character> CreateCharacter(CharacterType type);
    static CharacterType GetRandomCharacter();
    static std::string GetCharacterName(CharacterType type);
};

}}
