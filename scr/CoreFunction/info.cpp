#include "info.h"

namespace UNO { namespace Game {

using namespace Network;

void JoinGameInfo::Serialize(uint8_t *buffer) const
{
    JoinGameMsg *msg = reinterpret_cast<JoinGameMsg *>(buffer);
    msg->mType = MsgType::JOIN_GAME;
    msg->mLen = mUsername.size();
    std::strcpy(msg->mUsername, mUsername.c_str());
}

std::unique_ptr<JoinGameInfo> JoinGameInfo::Deserialize(const uint8_t *buffer)
{
    const JoinGameMsg *msg = reinterpret_cast<const JoinGameMsg *>(buffer);
    std::unique_ptr<JoinGameInfo> info = std::make_unique<JoinGameInfo>();
    info->mUsername = msg->mUsername;
    return info;
}

void JoinGameRspInfo::Serialize(uint8_t *buffer) const
{
    JoinGameRspMsg *msg = reinterpret_cast<JoinGameRspMsg *>(buffer);
    msg->mType = MsgType::JOIN_GAME_RSP;

    std::string usernames{};
    std::for_each(mUsernames.begin(), mUsernames.end(),
        [&usernames](const std::string &username) {
            usernames.append(username).push_back(' ');
        }
    );
    msg->mLen = sizeof(int) + usernames.size();

    msg->mPlayerNum = mPlayerNum;
    std::strcpy(msg->mUsernames, usernames.c_str());
}

std::unique_ptr<JoinGameRspInfo> JoinGameRspInfo::Deserialize(const uint8_t *buffer)
{
    const JoinGameRspMsg *msg = reinterpret_cast<const JoinGameRspMsg *>(buffer);
    std::unique_ptr<JoinGameRspInfo> info = std::make_unique<JoinGameRspInfo>();
    info->mPlayerNum = msg->mPlayerNum;
    std::string usernames(msg->mUsernames);
    while (!usernames.empty()) {
        int pos = usernames.find(' ');
        info->mUsernames.emplace_back(usernames, 0, pos);
        usernames.erase(0, pos + 1);
    }
    return info;
}

void GameStartInfo::Serialize(uint8_t *buffer) const
{
    GameStartMsg *msg = reinterpret_cast<GameStartMsg *>(buffer);
    msg->mType = MsgType::GAME_START;

    std::string usernames{};
    std::for_each(mUsernames.begin(), mUsernames.end(),
        [&usernames](const std::string &username) {
            usernames.append(username).push_back(' ');
        }
    );

    // 新增：序列化角色类型信息
    std::string characterTypes{};
    for (auto charType : mCharacterTypes) {
        characterTypes.append(std::to_string(static_cast<int>(charType))).push_back(' ');
    }

    msg->mLen = sizeof(Card) * 8 + sizeof(int) + usernames.size() + characterTypes.size();

    std::copy(mInitHandCards.begin(), mInitHandCards.end(), msg->mInitHandCards);
    msg->mFlippedCard = mFlippedCard;
    msg->mFirstPlayer = mFirstPlayer;
    
    // 合并用户名和角色类型信息
    std::string combinedInfo = usernames + "|" + characterTypes;
    std::strcpy(msg->mUsernames, combinedInfo.c_str());
}

std::unique_ptr<GameStartInfo> GameStartInfo::Deserialize(const uint8_t *buffer)
{
    const GameStartMsg *msg = reinterpret_cast<const GameStartMsg *>(buffer);
    std::unique_ptr<GameStartInfo> info = std::make_unique<GameStartInfo>();

    std::copy(std::begin(msg->mInitHandCards), std::end(msg->mInitHandCards),
            info->mInitHandCards.begin());
    info->mFlippedCard = msg->mFlippedCard;
    info->mFirstPlayer = msg->mFirstPlayer;
    
    std::string combinedInfo(msg->mUsernames);
    // 分离用户名和角色类型信息
    size_t separatorPos = combinedInfo.find('|');
    std::string usernames = combinedInfo.substr(0, separatorPos);
    std::string characterTypes = combinedInfo.substr(separatorPos + 1);
    
    // 解析用户名
    while (!usernames.empty()) {
        int pos = usernames.find(' ');
        if (pos != std::string::npos) {
            info->mUsernames.emplace_back(usernames, 0, pos);
            usernames.erase(0, pos + 1);
        } else {
            break;
        }
    }
    
    // 解析角色类型
    while (!characterTypes.empty()) {
        int pos = characterTypes.find(' ');
        if (pos != std::string::npos) {
            int charTypeValue = std::stoi(characterTypes.substr(0, pos));
            info->mCharacterTypes.push_back(static_cast<CharacterType>(charTypeValue));
            characterTypes.erase(0, pos + 1);
        } else {
            break;
        }
    }

    return info;
}

void ActionInfo::Serialize(uint8_t *buffer) const
{
    ActionMsg *msg = reinterpret_cast<ActionMsg *>(buffer);
    msg->mType = MsgType::ACTION;
    msg->mLen = sizeof(ActionMsg) - sizeof(Msg);
    msg->mActionType = mActionType;
    msg->mPlayerIndex = mPlayerIndex;
}

std::unique_ptr<ActionInfo> ActionInfo::Deserialize(const uint8_t *buffer)
{
    const ActionMsg *msg = reinterpret_cast<const ActionMsg *>(buffer);
    // info here is polymorphic
    std::unique_ptr<ActionInfo> info;
    switch (msg->mActionType) {
        case ActionType::DRAW:
            info.reset(dynamic_cast<ActionInfo *>(DrawInfo::Deserialize(buffer).release()));
            break;
        case ActionType::SKIP:
            info.reset(dynamic_cast<ActionInfo *>(SkipInfo::Deserialize(buffer).release()));
            break;
        case ActionType::PLAY:
            info.reset(dynamic_cast<ActionInfo *>(PlayInfo::Deserialize(buffer).release()));
            break;
        default:
            assert(0);
    }
    info->mActionType = msg->mActionType;
    info->mPlayerIndex = msg->mPlayerIndex;
    return info;
}

void DrawInfo::Serialize(uint8_t *buffer) const
{
    ActionInfo::Serialize(buffer);
    DrawMsg *msg = reinterpret_cast<DrawMsg *>(buffer);
    msg->mLen = sizeof(DrawMsg) - sizeof(Msg);
    msg->mNumber = mNumber;
}

std::unique_ptr<DrawInfo> DrawInfo::Deserialize(const uint8_t *buffer)
{
    const DrawMsg *msg = reinterpret_cast<const DrawMsg *>(buffer);
    std::unique_ptr<DrawInfo> info = std::make_unique<DrawInfo>();
    info->mNumber = msg->mNumber;
    return info;
}

void SkipInfo::Serialize(uint8_t *buffer) const
{
    ActionInfo::Serialize(buffer);
    SkipMsg *msg = reinterpret_cast<SkipMsg *>(buffer);
    msg->mLen = sizeof(SkipMsg) - sizeof(Msg);
}

std::unique_ptr<SkipInfo> SkipInfo::Deserialize(const uint8_t *buffer)
{
    const SkipMsg *msg = reinterpret_cast<const SkipMsg *>(buffer);
    std::unique_ptr<SkipInfo> info = std::make_unique<SkipInfo>();
    return info;
}

void PlayInfo::Serialize(uint8_t *buffer) const
{
    ActionInfo::Serialize(buffer);
    PlayMsg *msg = reinterpret_cast<PlayMsg *>(buffer);
    msg->mLen = sizeof(PlayMsg) - sizeof(Msg);
    msg->mCard = mCard;
    msg->mNextColor = mNextColor;
}

std::unique_ptr<PlayInfo> PlayInfo::Deserialize(const uint8_t *buffer)
{
    const PlayMsg *msg = reinterpret_cast<const PlayMsg *>(buffer);
    std::unique_ptr<PlayInfo> info = std::make_unique<PlayInfo>();
    info->mCard = msg->mCard;
    info->mNextColor = msg->mNextColor;
    return info;
}

void DrawRspInfo::Serialize(uint8_t *buffer) const
{
    DrawRspMsg *msg = reinterpret_cast<DrawRspMsg *>(buffer);
    msg->mType = MsgType::DRAW_RSP;

    msg->mLen = sizeof(int) + mNumber * sizeof(Card);
    msg->mNumber = mNumber;

    std::copy(mCards.begin(), mCards.end(), msg->mCards);
}

std::unique_ptr<DrawRspInfo> DrawRspInfo::Deserialize(const uint8_t *buffer)
{
    const DrawRspMsg *msg = reinterpret_cast<const DrawRspMsg *>(buffer);
    std::unique_ptr<DrawRspInfo> info = std::make_unique<DrawRspInfo>();
    info->mNumber = msg->mNumber;
    info->mCards.resize(info->mNumber);
    std::copy(msg->mCards, msg->mCards + msg->mNumber, info->mCards.begin());
    return info;
}

void GameEndInfo::Serialize(uint8_t *buffer) const
{
    GameEndMsg *msg = reinterpret_cast<GameEndMsg *>(buffer);
    msg->mType = MsgType::GAME_END;
    msg->mLen = sizeof(int);
    msg->mWinner = mWinner;
}

std::unique_ptr<GameEndInfo> GameEndInfo::Deserialize(const uint8_t *buffer)
{
    const GameEndMsg *msg = reinterpret_cast<const GameEndMsg *>(buffer);
    std::unique_ptr<GameEndInfo> info = std::make_unique<GameEndInfo>();
    info->mWinner = msg->mWinner;
    return info;
}

// 新增：SkillUseInfo 序列化/反序列化实现
void SkillUseInfo::Serialize(uint8_t *buffer) const
{
    SkillUseMsg *msg = reinterpret_cast<SkillUseMsg *>(buffer);
    msg->mType = MsgType::SKILL_USE;
    msg->mLen = sizeof(SkillUseMsg) - sizeof(Msg);
    msg->mPlayerIndex = mPlayerIndex;
    msg->mSkillType = static_cast<int>(mSkillType);
    msg->mTargetPlayer = mTargetPlayer;
    msg->mCardType = static_cast<int>(mCardType);
}

std::unique_ptr<SkillUseInfo> SkillUseInfo::Deserialize(const uint8_t *buffer)
{
    const SkillUseMsg *msg = reinterpret_cast<const SkillUseMsg *>(buffer);
    std::unique_ptr<SkillUseInfo> info = std::make_unique<SkillUseInfo>();
    info->mPlayerIndex = msg->mPlayerIndex;
    info->mSkillType = static_cast<CharacterType>(msg->mSkillType);
    info->mTargetPlayer = msg->mTargetPlayer;
    info->mCardType = static_cast<CardText>(msg->mCardType);
    return info;
}

// 新增：SkillRspInfo 序列化/反序列化实现
void SkillRspInfo::Serialize(uint8_t *buffer) const
{
    SkillRspMsg *msg = reinterpret_cast<SkillRspMsg *>(buffer);
    msg->mType = MsgType::SKILL_RSP;
    msg->mLen = sizeof(SkillRspMsg) - sizeof(Msg) + mAffectedCards.size() * sizeof(Card);
    msg->mPlayerIndex = mPlayerIndex;
    msg->mSuccess = mSuccess;
    msg->mAffectedCardsCount = mAffectedCards.size();
    std::copy(mAffectedCards.begin(), mAffectedCards.end(), msg->mAffectedCards);
}

std::unique_ptr<SkillRspInfo> SkillRspInfo::Deserialize(const uint8_t *buffer)
{
    const SkillRspMsg *msg = reinterpret_cast<const SkillRspMsg *>(buffer);
    std::unique_ptr<SkillRspInfo> info = std::make_unique<SkillRspInfo>();
    info->mPlayerIndex = msg->mPlayerIndex;
    info->mSuccess = msg->mSuccess;
    info->mAffectedCards.resize(msg->mAffectedCardsCount);
    std::copy(msg->mAffectedCards, msg->mAffectedCards + msg->mAffectedCardsCount, 
              info->mAffectedCards.begin());
    return info;
}

// 新增：SpecialEffectInfo 序列化/反序列化实现
void SpecialEffectInfo::Serialize(uint8_t *buffer) const
{
    SpecialEffectMsg *msg = reinterpret_cast<SpecialEffectMsg *>(buffer);
    msg->mType = MsgType::SPECIAL_EFFECT;
    msg->mLen = sizeof(SpecialEffectMsg) - sizeof(Msg) + mAffectedPlayers.size() * sizeof(int);
    msg->mPlayerIndex = mPlayerIndex;
    msg->mEffectType = static_cast<int>(mEffectType);
    msg->mTargetColor = static_cast<int>(mTargetColor);
    msg->mAffectedPlayersCount = mAffectedPlayers.size();
    std::copy(mAffectedPlayers.begin(), mAffectedPlayers.end(), msg->mAffectedPlayers);
}

std::unique_ptr<SpecialEffectInfo> SpecialEffectInfo::Deserialize(const uint8_t *buffer)
{
    const SpecialEffectMsg *msg = reinterpret_cast<const SpecialEffectMsg *>(buffer);
    std::unique_ptr<SpecialEffectInfo> info = std::make_unique<SpecialEffectInfo>();
    info->mPlayerIndex = msg->mPlayerIndex;
    info->mEffectType = static_cast<CardText>(msg->mEffectType);
    info->mTargetColor = static_cast<CardColor>(msg->mTargetColor);
    info->mAffectedPlayers.resize(msg->mAffectedPlayersCount);
    std::copy(msg->mAffectedPlayers, msg->mAffectedPlayers + msg->mAffectedPlayersCount,
              info->mAffectedPlayers.begin());
    return info;
}

// 新增：GameStateUpdateInfo 序列化/反序列化实现
void GameStateUpdateInfo::Serialize(uint8_t *buffer) const
{
    GameStateUpdateMsg *msg = reinterpret_cast<GameStateUpdateMsg *>(buffer);
    msg->mType = MsgType::GAME_STATE_UPDATE;
    msg->mLen = sizeof(GameStateUpdateMsg) - sizeof(Msg);
    msg->mCurrentPlayer = mCurrentPlayer;
    msg->mCurrentPhase = static_cast<int>(mCurrentPhase);
    msg->mSpecialEffectActive = mSpecialEffectActive;
    msg->mLastPlayedCard = mLastPlayedCard;
    msg->mCardsNumToDraw = mCardsNumToDraw;
}

std::unique_ptr<GameStateUpdateInfo> GameStateUpdateInfo::Deserialize(const uint8_t *buffer)
{
    const GameStateUpdateMsg *msg = reinterpret_cast<const GameStateUpdateMsg *>(buffer);
    std::unique_ptr<GameStateUpdateInfo> info = std::make_unique<GameStateUpdateInfo>();
    info->mCurrentPlayer = msg->mCurrentPlayer;
    info->mCurrentPhase = static_cast<GameStat::TurnPhase>(msg->mCurrentPhase);
    info->mSpecialEffectActive = msg->mSpecialEffectActive;
    info->mLastPlayedCard = msg->mLastPlayedCard;
    info->mCardsNumToDraw = msg->mCardsNumToDraw;
    return info;
}

// 操作符重载实现保持不变
bool JoinGameInfo::operator==(const JoinGameInfo &info) const
{
    return mUsername == info.mUsername;
}

bool JoinGameRspInfo::operator==(const JoinGameRspInfo &info) const
{
    return (mPlayerNum == info.mPlayerNum) && (mUsernames == info.mUsernames);
}

bool GameStartInfo::operator==(const GameStartInfo &info) const
{
    return (mInitHandCards == info.mInitHandCards) && (mFlippedCard == info.mFlippedCard)
        && (mFirstPlayer == info.mFirstPlayer) && (mUsernames == info.mUsernames)
        && (mCharacterTypes == info.mCharacterTypes);  // 新增：角色类型比较
}

bool ActionInfo::operator==(const ActionInfo &info) const
{
    return (mActionType == info.mActionType) && (mPlayerIndex == info.mPlayerIndex);
}

bool DrawInfo::operator==(const DrawInfo &info) const
{
    return (mNumber == info.mNumber) 
        && (dynamic_cast<const ActionInfo &>(*this) == dynamic_cast<const ActionInfo &>(info));
}

bool SkipInfo::operator==(const SkipInfo &info) const
{
    return dynamic_cast<const ActionInfo &>(*this) == dynamic_cast<const ActionInfo &>(info);
}

bool PlayInfo::operator==(const PlayInfo &info) const
{
    return (mCard == info.mCard) && (mNextColor == info.mNextColor)
        && (dynamic_cast<const ActionInfo &>(*this) == dynamic_cast<const ActionInfo &>(info));
}

bool DrawRspInfo::operator==(const DrawRspInfo &info) const
{
    return (mNumber == info.mNumber) && (mCards == info.mCards);
}

bool GameEndInfo::operator==(const GameEndInfo &info) const
{
    return mWinner == info.mWinner;
}

// 新增：SkillUseInfo 操作符重载
bool SkillUseInfo::operator==(const SkillUseInfo &info) const
{
    return (mPlayerIndex == info.mPlayerIndex) && (mSkillType == info.mSkillType)
        && (mTargetPlayer == info.mTargetPlayer) && (mCardType == info.mCardType);
}

// 新增：SkillRspInfo 操作符重载
bool SkillRspInfo::operator==(const SkillRspInfo &info) const
{
    return (mPlayerIndex == info.mPlayerIndex) && (mSuccess == info.mSuccess)
        && (mAffectedCards == info.mAffectedCards);
}

// 新增：SpecialEffectInfo 操作符重载
bool SpecialEffectInfo::operator==(const SpecialEffectInfo &info) const
{
    return (mPlayerIndex == info.mPlayerIndex) && (mEffectType == info.mEffectType)
        && (mTargetColor == info.mTargetColor) && (mAffectedPlayers == info.mAffectedPlayers);
}

// 新增：GameStateUpdateInfo 操作符重载
bool GameStateUpdateInfo::operator==(const GameStateUpdateInfo &info) const
{
    return (mCurrentPlayer == info.mCurrentPlayer) && (mCurrentPhase == info.mCurrentPhase)
        && (mSpecialEffectActive == info.mSpecialEffectActive) 
        && (mLastPlayedCard == info.mLastPlayedCard) && (mCardsNumToDraw == info.mCardsNumToDraw);
}

// 输出操作符实现保持不变
std::ostream& operator<<(std::ostream& os, const JoinGameInfo& info)
{
    os << "JoinGameInfo Received: " << std::endl;
    os << "\t mUsername: " << info.mUsername << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const JoinGameRspInfo& info)
{
    os << "JoinGameRspInfo Received: " << std::endl;
    os << "\t mPlayerNum: " << info.mPlayerNum << std::endl;
    os << "\t mUsernames: [";
    assert(!info.mUsernames.empty());
    for (int i = 0; i < info.mUsernames.size() - 1; i++) {
        os << info.mUsernames[i] << ", ";
    }
    os << info.mUsernames.back() << "]" << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const GameStartInfo& info)
{
    os << "GameStartInfo Received: " << std::endl;
    os << "\t mInitHandCards: [";
    for (int i = 0; i < 6; i++) {
        os << info.mInitHandCards[i] << ", ";
    }
    os << info.mInitHandCards[6] << "]" << std::endl;

    os << "\t mFlippedCard: " << info.mFlippedCard << std::endl;
    os << "\t mFirstPlayer: " << info.mFirstPlayer << std::endl;

    os << "\t mUsernames: [";
    assert(!info.mUsernames.empty());
    for (int i = 0; i < info.mUsernames.size() - 1; i++) {
        os << info.mUsernames[i] << ", ";
    }
    os << info.mUsernames.back() << "]" << std::endl;
    
    // 新增：输出角色类型信息
    os << "\t mCharacterTypes: [";
    if (!info.mCharacterTypes.empty()) {
        for (int i = 0; i < info.mCharacterTypes.size() - 1; i++) {
            os << static_cast<int>(info.mCharacterTypes[i]) << ", ";
        }
        os << static_cast<int>(info.mCharacterTypes.back()) << "]" << std::endl;
    } else {
        os << "]" << std::endl;
    }
    
    return os;
}

std::ostream& operator<<(std::ostream& os, const ActionInfo& info)
{
    os << "\t mActionType: " << info.mActionType << std::endl;
    os << "\t mPlayerIndex: " << info.mPlayerIndex << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const DrawInfo& info)
{
    os << "DrawInfo Received: " << std::endl;
    os << dynamic_cast<const ActionInfo &>(info);
    os << "\t mNumber: " << info.mNumber << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const SkipInfo& info)
{
    os << "SkipInfo Received: " << std::endl;
    os << dynamic_cast<const ActionInfo &>(info);
    return os;
}

std::ostream& operator<<(std::ostream& os, const PlayInfo& info)
{
    os << "PlayInfo Received: " << std::endl;
    os << dynamic_cast<const ActionInfo &>(info);
    os << "\t mCard: " << info.mCard << std::endl;
    os << "\t mNextColor: " << info.mNextColor << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const DrawRspInfo& info)
{
    os << "DrawRspInfo Received: " << std::endl;
    os << "\t mNumber: " << info.mNumber << std::endl;
    os << "\t mCards: [";
    assert(!info.mCards.empty());
    for (int i = 0; i < info.mCards.size() - 1; i++) {
        os << info.mCards[i] << ", ";
    }
    os << info.mCards.back() << "]" << std::endl;
    return os;
}

// 新增：SkillUseInfo 输出操作符
std::ostream& operator<<(std::ostream& os, const SkillUseInfo& info)
{
    os << "SkillUseInfo Received: " << std::endl;
    os << "\t mPlayerIndex: " << info.mPlayerIndex << std::endl;
    os << "\t mSkillType: " << static_cast<int>(info.mSkillType) << std::endl;
    os << "\t mTargetPlayer: " << info.mTargetPlayer << std::endl;
    os << "\t mCardType: " << static_cast<int>(info.mCardType) << std::endl;
    return os;
}

// 新增：SkillRspInfo 输出操作符
std::ostream& operator<<(std::ostream& os, const SkillRspInfo& info)
{
    os << "SkillRspInfo Received: " << std::endl;
    os << "\t mPlayerIndex: " << info.mPlayerIndex << std::endl;
    os << "\t mSuccess: " << info.mSuccess << std::endl;
    os << "\t mAffectedCards: [";
    if (!info.mAffectedCards.empty()) {
        for (int i = 0; i < info.mAffectedCards.size() - 1; i++) {
            os << info.mAffectedCards[i] << ", ";
        }
        os << info.mAffectedCards.back() << "]" << std::endl;
    } else {
        os << "]" << std::endl;
    }
    return os;
}

// 新增：SpecialEffectInfo 输出操作符
std::ostream& operator<<(std::ostream& os, const SpecialEffectInfo& info)
{
    os << "SpecialEffectInfo Received: " << std::endl;
    os << "\t mPlayerIndex: " << info.mPlayerIndex << std::endl;
    os << "\t mEffectType: " << static_cast<int>(info.mEffectType) << std::endl;
    os << "\t mTargetColor: " << static_cast<int>(info.mTargetColor) << std::endl;
    os << "\t mAffectedPlayers: [";
    if (!info.mAffectedPlayers.empty()) {
        for (int i = 0; i < info.mAffectedPlayers.size() - 1; i++) {
            os << info.mAffectedPlayers[i] << ", ";
        }
        os << info.mAffectedPlayers.back() << "]" << std::endl;
    } else {
        os << "]" << std::endl;
    }
    return os;
}

// 新增：GameStateUpdateInfo 输出操作符
std::ostream& operator<<(std::ostream& os, const GameStateUpdateInfo& info)
{
    os << "GameStateUpdateInfo Received: " << std::endl;
    os << "\t mCurrentPlayer: " << info.mCurrentPlayer << std::endl;
    os << "\t mCurrentPhase: " << static_cast<int>(info.mCurrentPhase) << std::endl;
    os << "\t mSpecialEffectActive: " << info.mSpecialEffectActive << std::endl;
    os << "\t mLastPlayedCard: " << info.mLastPlayedCard << std::endl;
    os << "\t mCardsNumToDraw: " << info.mCardsNumToDraw << std::endl;
    return os;
}

}}
