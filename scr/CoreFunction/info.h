#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <cstring>
#include <algorithm>
#include <memory>

#include "../network/msg.h"
#include "stat.h"  // 新增：包含角色系统头文件

namespace UNO { namespace Game {

using namespace Network;

struct Info {
    // enable polymorphism
    virtual ~Info() {}
};

struct JoinGameInfo : public Info {
    std::string mUsername;

    JoinGameInfo() {}
    JoinGameInfo(const std::string &username) : mUsername(username) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<JoinGameInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const JoinGameInfo &info) const;
    friend std::ostream &operator<<(std::ostream &os, const JoinGameInfo &info);
};

struct JoinGameRspInfo : public Info {
    int mPlayerNum;
    std::vector<std::string> mUsernames;

    JoinGameRspInfo() {}
    JoinGameRspInfo(int playerNum, const std::vector<std::string> &usernames)
        : mPlayerNum(playerNum), mUsernames(usernames) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<JoinGameRspInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const JoinGameRspInfo &info) const;
    friend std::ostream &operator<<(std::ostream &os, const JoinGameRspInfo &info);
};

struct GameStartInfo : public Info {
    std::array<Card, 7> mInitHandCards;
    Card mFlippedCard;
    int mFirstPlayer;
    std::vector<std::string> mUsernames;
    std::vector<CharacterType> mCharacterTypes;  // 新增：角色类型信息

    GameStartInfo() {}
    GameStartInfo(const std::array<Card, 7> &initHandCards,
        Card flippedCard, int firstPlayer,
        const std::vector<std::string> &usernames,
        const std::vector<CharacterType> &characterTypes = {})  // 新增：角色类型参数
        : mInitHandCards(initHandCards), mFlippedCard(flippedCard),
        mFirstPlayer(firstPlayer), mUsernames(usernames), mCharacterTypes(characterTypes) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<GameStartInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const GameStartInfo &info) const;
    friend std::ostream& operator<<(std::ostream& os, const GameStartInfo& info);
};

struct ActionInfo : public Info {
    ActionType mActionType;
    int mPlayerIndex{-1};

    ActionInfo() {}
    ActionInfo(ActionType actionType) : mActionType(actionType) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<ActionInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const ActionInfo &info) const;
    friend std::ostream& operator<<(std::ostream& os, const ActionInfo& info);

    // enable polymorphism
    virtual ~ActionInfo() {}
};

struct DrawInfo : public ActionInfo {
    int mNumber;

    DrawInfo() : ActionInfo(ActionType::DRAW) {}
    DrawInfo(int number) : ActionInfo(ActionType::DRAW), mNumber(number) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<DrawInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const DrawInfo &info) const;
    friend std::ostream& operator<<(std::ostream& os, const DrawInfo& info);
};

struct SkipInfo : public ActionInfo {
    SkipInfo() : ActionInfo(ActionType::SKIP) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<SkipInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const SkipInfo &info) const;
    friend std::ostream& operator<<(std::ostream& os, const SkipInfo& info);
};

struct PlayInfo : public ActionInfo {
    Card mCard;
    CardColor mNextColor;

    PlayInfo() : ActionInfo(ActionType::PLAY) {}
    PlayInfo(Card card) : PlayInfo(card, card.mColor) {}
    PlayInfo(Card card, CardColor nextColor)
        : ActionInfo(ActionType::PLAY), mCard(card), mNextColor(nextColor) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<PlayInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const PlayInfo &info) const;
    friend std::ostream& operator<<(std::ostream& os, const PlayInfo& info);
};

struct DrawRspInfo : public Info {
    int mNumber;
    std::vector<Card> mCards;

    DrawRspInfo() {}
    DrawRspInfo(int number, const std::vector<Card> &cards) 
        : mNumber(number), mCards(cards) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<DrawRspInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const DrawRspInfo &info) const;
    friend std::ostream& operator<<(std::ostream& os, const DrawRspInfo& info);
};

struct GameEndInfo : public Info {
    int mWinner;

    GameEndInfo() {}
    GameEndInfo(int winner) : mWinner(winner) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<GameEndInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const GameEndInfo &info) const;
};

// 新增：技能使用信息
struct SkillUseInfo : public Info {
    int mPlayerIndex;
    CharacterType mSkillType;
    int mTargetPlayer;  // 对于需要目标的技能（如Thief）
    CardText mCardType; // 对于需要指定卡牌类型的技能

    SkillUseInfo() {}
    SkillUseInfo(int playerIndex, CharacterType skillType, 
                 int targetPlayer = -1, CardText cardType = CardText::EMPTY)
        : mPlayerIndex(playerIndex), mSkillType(skillType),
          mTargetPlayer(targetPlayer), mCardType(cardType) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<SkillUseInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const SkillUseInfo &info) const;
    friend std::ostream& operator<<(std::ostream& os, const SkillUseInfo& info);
};

// 新增：技能响应信息
struct SkillRspInfo : public Info {
    int mPlayerIndex;
    bool mSuccess;
    std::vector<Card> mAffectedCards;  // 技能影响的卡牌

    SkillRspInfo() {}
    SkillRspInfo(int playerIndex, bool success, const std::vector<Card> &affectedCards = {})
        : mPlayerIndex(playerIndex), mSuccess(success), mAffectedCards(affectedCards) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<SkillRspInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const SkillRspInfo &info) const;
    friend std::ostream& operator<<(std::ostream& os, const SkillRspInfo& info);
};

// 新增：特殊效果信息（用于Package和Flash卡）
struct SpecialEffectInfo : public Info {
    int mPlayerIndex;
    CardText mEffectType;  // PACKAGE 或 FLASH
    CardColor mTargetColor; // 效果目标颜色
    std::vector<int> mAffectedPlayers; // 受影响的玩家

    SpecialEffectInfo() {}
    SpecialEffectInfo(int playerIndex, CardText effectType, 
                      CardColor targetColor = CardColor::RED,
                      const std::vector<int> &affectedPlayers = {})
        : mPlayerIndex(playerIndex), mEffectType(effectType),
          mTargetColor(targetColor), mAffectedPlayers(affectedPlayers) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<SpecialEffectInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const SpecialEffectInfo &info) const;
    friend std::ostream& operator<<(std::ostream& os, const SpecialEffectInfo& info);
};

// 新增：游戏状态更新信息
struct GameStateUpdateInfo : public Info {
    int mCurrentPlayer;
    GameStat::TurnPhase mCurrentPhase;
    bool mSpecialEffectActive;
    Card mLastPlayedCard;
    int mCardsNumToDraw;

    GameStateUpdateInfo() {}
    GameStateUpdateInfo(int currentPlayer, GameStat::TurnPhase currentPhase,
                        bool specialEffectActive, Card lastPlayedCard, int cardsNumToDraw)
        : mCurrentPlayer(currentPlayer), mCurrentPhase(currentPhase),
          mSpecialEffectActive(specialEffectActive), mLastPlayedCard(lastPlayedCard),
          mCardsNumToDraw(cardsNumToDraw) {}

    void Serialize(uint8_t *buffer) const;
    static std::unique_ptr<GameStateUpdateInfo> Deserialize(const uint8_t *buffer);

    bool operator==(const GameStateUpdateInfo &info) const;
    friend std::ostream& operator<<(std::ostream& os, const GameStateUpdateInfo& info);
};

}}
