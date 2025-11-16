#include "msg.h"

namespace UNO { namespace Network {

std::ostream& operator<<(std::ostream& os, const ActionType& type)
{
    std::string typeStr;
    switch (type) {
        case ActionType::DRAW: typeStr = "DRAW"; break;
        case ActionType::SKIP: typeStr = "SKIP"; break;
        case ActionType::PLAY: typeStr = "PLAY"; break;
        default: assert(0);
    }

    os << typeStr;
    return os;
}

// 新增：MsgType输出操作符
std::ostream& operator<<(std::ostream& os, const MsgType& type)
{
    std::string typeStr;
    switch (type) {
        case MsgType::JOIN_GAME: typeStr = "JOIN_GAME"; break;
        case MsgType::JOIN_GAME_RSP: typeStr = "JOIN_GAME_RSP"; break;
        case MsgType::GAME_START: typeStr = "GAME_START"; break;
        case MsgType::ACTION: typeStr = "ACTION"; break;
        case MsgType::DRAW_RSP: typeStr = "DRAW_RSP"; break;
        case MsgType::GAME_END: typeStr = "GAME_END"; break;
        case MsgType::SKILL_USE: typeStr = "SKILL_USE"; break;
        case MsgType::SKILL_RSP: typeStr = "SKILL_RSP"; break;
        case MsgType::SPECIAL_EFFECT: typeStr = "SPECIAL_EFFECT"; break;
        case MsgType::GAME_STATE_UPDATE: typeStr = "GAME_STATE_UPDATE"; break;
        default: assert(0);
    }

    os << typeStr;
    return os;
}

// 新增：CharacterType输出操作符
std::ostream& operator<<(std::ostream& os, const CharacterType& type)
{
    std::string typeStr;
    switch (type) {
        case CharacterType::LUCKY_STAR: typeStr = "LUCKY_STAR"; break;
        case CharacterType::COLLECTOR: typeStr = "COLLECTOR"; break;
        case CharacterType::THIEF: typeStr = "THIEF"; break;
        case CharacterType::DEFENDER: typeStr = "DEFENDER"; break;
        case CharacterType::NONE: typeStr = "NONE"; break;
        default: assert(0);
    }

    os << typeStr;
    return os;
}

// 新增：TurnPhase输出操作符
std::ostream& operator<<(std::ostream& os, const GameStat::TurnPhase& phase)
{
    std::string phaseStr;
    switch (phase) {
        case GameStat::TurnPhase::START: phaseStr = "START"; break;
        case GameStat::TurnPhase::SKILL: phaseStr = "SKILL"; break;
        case GameStat::TurnPhase::CARD_PLAY: phaseStr = "CARD_PLAY"; break;
        case GameStat::TurnPhase::END: phaseStr = "END"; break;
        default: assert(0);
    }

    os << phaseStr;
    return os;
}
}}
