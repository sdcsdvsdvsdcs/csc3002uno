#pragma once

#include "../game/cards.h"
#include "../game/stat.h"

namespace UNO { namespace Network {

using namespace Game;

enum class MsgType : uint8_t {
    JOIN_GAME,
    JOIN_GAME_RSP,
    GAME_START,
    ACTION,
    DRAW_RSP,
    GAME_END,
    // 新增：角色系统消息类型
    SKILL_USE,
    SKILL_RSP,
    // 新增：特殊效果消息类型
    SPECIAL_EFFECT,
    // 新增：游戏状态更新消息
    GAME_STATE_UPDATE
};

struct Msg {
    MsgType mType;
    int mLen;  // **not** include the mType and mLen itself
};

struct JoinGameMsg : public Msg {
    char mUsername[];
};

struct JoinGameRspMsg : public Msg {
    int mPlayerNum;
    // including player himself
    char mUsernames[];
};

struct GameStartMsg : public Msg {
    Card mInitHandCards[7];
    Card mFlippedCard;  // indicating the first card that should be played
    int mFirstPlayer;  // the index of the first player to play a card
    // usernames of all players, not including player himself, ' ' as delimiter
    // and the order is from left side of the player to right side
    char mUsernames[];
};

enum class ActionType : uint8_t {
    DRAW,
    SKIP,
    PLAY
};
std::ostream& operator<<(std::ostream& os, const ActionType& type);

struct ActionMsg : public Msg {
    ActionType mActionType;
    int mPlayerIndex;
};

struct DrawMsg : public ActionMsg {
    int mNumber;  // the number of cards to draw
};

struct SkipMsg : public ActionMsg {
};

struct PlayMsg : public ActionMsg {
    Card mCard;
    CardColor mNextColor;  // valid only if mCard is black
};

struct DrawRspMsg : public Msg {
    int mNumber;
    Card mCards[];
};

struct GameEndMsg : public Msg {
    int mWinner;
};

// 新增：技能使用消息
struct SkillUseMsg : public Msg {
    int mPlayerIndex;
    int mSkillType;  // CharacterType 枚举值
    int mTargetPlayer;  // 对于需要目标的技能（如Thief）
    int mCardType;      // 对于需要指定卡牌类型的技能
};

// 新增：技能响应消息
struct SkillRspMsg : public Msg {
    int mPlayerIndex;
    bool mSuccess;
    int mAffectedCardsCount;
    Card mAffectedCards[];  // 技能影响的卡牌
};

// 新增：特殊效果消息（用于Package和Flash卡）
struct SpecialEffectMsg : public Msg {
    int mPlayerIndex;
    int mEffectType;    // CardText 枚举值（PACKAGE 或 FLASH）
    int mTargetColor;   // 效果目标颜色
    int mAffectedPlayersCount;
    int mAffectedPlayers[]; // 受影响的玩家索引
};

// 新增：游戏状态更新消息
struct GameStateUpdateMsg : public Msg {
    int mCurrentPlayer;
    int mCurrentPhase;  // GameStat::TurnPhase 枚举值
    bool mSpecialEffectActive;
    Card mLastPlayedCard;
    int mCardsNumToDraw;
};
}}
