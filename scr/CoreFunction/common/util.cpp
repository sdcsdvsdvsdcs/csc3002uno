#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

#if defined(__unix__) || defined(__APPLE__)
#include <poll.h>
#include <sys/ioctl.h>
#elif defined(_WIN32)
#include <conio.h>
#include <windows.h>
#endif

#include "util.h"
#include "../game/stat.h"

namespace UNO { namespace Common {

int Util::Wrap(int numToWrap, int range)
{
    int ret = numToWrap % range;
    if (ret < 0) {
        ret += range;
    }
    return ret;
}

int Util::WrapWithPlayerNum(int numToWrap)
{
    return Wrap(numToWrap, Common::mPlayerNum);
}

int Util::GetSegmentNum(int handcardNum) { 
    return (handcardNum - 1) / Common::mHandCardsNumPerRow + 1; 
}

int Util::GetSegmentIndex(int handcardIndex) { 
    return handcardIndex / Common::mHandCardsNumPerRow; 
}

int Util::GetIndexInSegment(int handcardIndex) { 
    return handcardIndex % Common::mHandCardsNumPerRow; 
}

char Util::GetCharWithTimeout(int milliseconds, bool autoFlush)
{
#if defined(__unix__) || defined(__APPLE__)
    std::unique_ptr<Terminal> terminal;
    if (autoFlush) {
        terminal.reset(new Terminal());
        terminal->SetModeAutoFlush();
    }

    struct pollfd pfd = { STDIN_FILENO, POLLIN, 0 };
    int ret = poll(&pfd, 1, milliseconds);

    if (ret == 0) {
        throw std::runtime_error("timeout");
    }
    else if (ret == 1) {
        char c = getchar();
        return c;
    }
#elif defined(_WIN32)
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    auto ret = WaitForSingleObject(GetStdHandle(STD_INPUT_HANDLE), milliseconds);

    if (ret == WAIT_TIMEOUT) {
        throw std::runtime_error("timeout");
    }
    else if (ret == WAIT_OBJECT_0) {
        char c = _getch();
        return c;
    }
#endif
    return 0;
}

void Util::HideTerminalCursor()
{
    Terminal::ShowCursor(false);
}

void Util::ShowTerminalCursor()
{
    Terminal::ShowCursor(true);
}

std::string Util::GetColoredText(const std::string& text, const std::string& color)
{
#if defined(__unix__) || defined(__APPLE__)
    static std::map<std::string, std::string> colorMap = {
        {"red", "\033[31m"}, {"green", "\033[32m"}, {"yellow", "\033[33m"},
        {"blue", "\033[34m"}, {"magenta", "\033[35m"}, {"cyan", "\033[36m"},
        {"white", "\033[37m"}, {"bright_red", "\033[91m"}, {"bright_green", "\033[92m"},
        {"bright_yellow", "\033[93m"}, {"bright_blue", "\033[94m"}, {"bright_magenta", "\033[95m"},
        {"bright_cyan", "\033[96m"}, {"reset", "\033[0m"}
    };
    
    auto it = colorMap.find(color);
    if (it != colorMap.end()) {
        return it->second + text + colorMap["reset"];
    }
#elif defined(_WIN32)
    // Windows下返回原始文本，颜色由Terminal类处理
#endif
    return text;
}

std::string Util::FormatTime(int seconds)
{
    int minutes = seconds / 60;
    int secs = seconds % 60;
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << minutes << ":" 
       << std::setw(2) << std::setfill('0') << secs;
    return ss.str();
}

bool Util::IsNumber(const std::string& str)
{
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

std::vector<std::string> Util::SplitString(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string Util::TrimString(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    
    if (start == std::string::npos) {
        return "";
    }
    
    return str.substr(start, end - start + 1);
}

std::string Util::GetCharacterSkillDescription(CharacterType characterType)
{
    switch (characterType) {
        case CharacterType::LUCKY_STAR:
            return "Lucky Star: View top 3 deck cards and choose one to draw";
        case CharacterType::COLLECTOR:
            return "Collector: Take one card from discard pile";
        case CharacterType::THIEF:
            return "Thief: Steal a card from another player";
        case CharacterType::DEFENDER:
            return "Defender: Automatically defend against character skills";
        default:
            return "No character skill";
    }
}

bool Util::CanUseSkillThisTurn(int playerIndex, const std::vector<bool>& skillUsedThisTurn)
{
    if (playerIndex < 0 || playerIndex >= skillUsedThisTurn.size()) {
        return false;
    }
    return !skillUsedThisTurn[playerIndex];
}

int Util::CalculateSkillCooldown(CharacterType characterType)
{
    switch (characterType) {
        case CharacterType::LUCKY_STAR:
            return 1; // 冷却1回合
        case CharacterType::COLLECTOR:
            return 1;
        case CharacterType::THIEF:
            return 1;
        case CharacterType::DEFENDER:
            return 1;
        default:
            return 0;
    }
}

bool Util::IsSpecialCard(const Card& card)
{
    // 检查是否为功能卡或特殊卡
    static std::set<CardText> specialTexts = {
        CardText::SKIP, CardText::REVERSE, CardText::DRAW_TWO,
        CardText::WILD, CardText::DRAW_FOUR, CardText::PACKAGE, CardText::FLASH
    };
    
    return specialTexts.count(card.mText) > 0;
}

bool Util::CanChainCards(const std::vector<Card>& cards)
{
    if (cards.size() < 2) {
        return true;
    }
    
    // 检查所有卡牌是否可以连锁（相同数字或类型）
    CardText firstText = cards[0].mText;
    for (size_t i = 1; i < cards.size(); ++i) {
        if (cards[i].mText != firstText) {
            return false;
        }
    }
    
    return true;
}

int Util::CalculateDrawPenalty(const Card& card, int currentDrawCount)
{
    switch (card.mText) {
        case CardText::DRAW_TWO:
            return currentDrawCount + 2;
        case CardText::DRAW_FOUR:
            return currentDrawCount + 4;
        default:
            return currentDrawCount;
    }
}

bool Util::ValidateGameState(int currentPlayer, int playerCount)
{
    return currentPlayer >= 0 && currentPlayer < playerCount;
}

bool Util::ValidateCardPlay(const Card& card, const Card& lastCard, bool isUno)
{
    // 基础验证逻辑
    if (card.mColor == CardColor::BLACK) {
        return true; // 万能牌总是可以出
    }
    
    // 颜色匹配
    if (card.mColor == lastCard.mColor) {
        return true;
    }
    
    // 数字/类型匹配
    if (card.mText == lastCard.mText) {
        return true;
    }
    
    // UNO状态下不能出功能卡作为最后一张牌
    if (isUno && IsSpecialCard(card)) {
        return false;
    }
    
    return false;
}

}}
