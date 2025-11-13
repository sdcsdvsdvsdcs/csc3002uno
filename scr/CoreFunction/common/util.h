#pragma once

#include <memory>
#include <cassert>
#include <vector>
#include <map>
#ifdef ENABLE_LOG
#include <spdlog/spdlog.h>
#endif

#include "common.h"
#include "terminal.h"

namespace UNO { 

namespace Network {
    
class IServer;

class IClient;

}

namespace Common {

class Util {
public:
    static int Wrap(int numToWrap, int range);

    static int WrapWithPlayerNum(int numToWrap);

    /**
     * Consume a char with a timeout, if it's exceeded, an exception will be thrown
     *   \param milliseconds: timeout in milliseconds
     *   \param autoFlush: if true, the inputted char will get consumed regardless of '\n'
     *   \return the consumed char
     */
    static char GetCharWithTimeout(int milliseconds, bool autoFlush);

    /**
     * Get the number of segment, given the number of cards in hand.
     */
    static int GetSegmentNum(int handcardNum);

    /**
     * Get the index of segment that a card in hand belongs to.
     */
    static int GetSegmentIndex(int handcardIndex);

    /**
     * Get the index in a segment, given the index of the card in hand.
     */
    static int GetIndexInSegment(int handcardIndex);

    /**
     * Hide the terminal cursor for better player experience, 
     * especially when clean screen without using cls
     */
    static void HideTerminalCursor();

    /**
     * 新增：显示终端光标
     */
    static void ShowTerminalCursor();

    /**
     * 新增：获取带颜色的字符串
     */
    static std::string GetColoredText(const std::string& text, const std::string& color);

    /**
     * 新增：格式化时间显示
     */
    static std::string FormatTime(int seconds);

    /**
     * 新增：检查字符串是否为数字
     */
    static bool IsNumber(const std::string& str);

    /**
     * 新增：字符串分割
     */
    static std::vector<std::string> SplitString(const std::string& str, char delimiter);

    /**
     * 新增：字符串修剪
     */
    static std::string TrimString(const std::string& str);

    /**
     * 新增：角色技能相关工具函数
     */
    static std::string GetCharacterSkillDescription(CharacterType characterType);
    static bool CanUseSkillThisTurn(int playerIndex, const std::vector<bool>& skillUsedThisTurn);
    static int CalculateSkillCooldown(CharacterType characterType);

    /**
     * 新增：卡牌效果相关工具函数
     */
    static bool IsSpecialCard(const Card& card);
    static bool CanChainCards(const std::vector<Card>& cards);
    static int CalculateDrawPenalty(const Card& card, int currentDrawCount);

    /**
     * 新增：游戏状态验证
     */
    static bool ValidateGameState(int currentPlayer, int playerCount);
    static bool ValidateCardPlay(const Card& card, const Card& lastCard, bool isUno);

    /**
     * Dynamically cast a unique_ptr to one with another type.
     */
    template<typename DstInfoT, typename SrcInfoUp>
    static std::unique_ptr<DstInfoT> DynamicCast(SrcInfoUp &&srcInfo) {
        return std::unique_ptr<DstInfoT>(dynamic_cast<DstInfoT *>(srcInfo.release()));
    }

    /**
     * Helper of ReceiveInfo for brief code.
     */
    template<typename InfoT, typename Peer, typename... Args>
    static std::unique_ptr<InfoT> Receive(std::shared_ptr<Peer> peer, Args... args) {
        if constexpr (std::is_same_v<Peer, Network::IServer>) {
            static_assert(sizeof...(args) == 1);
            return ReceiveHelper<InfoT>(peer, args...);
        }
        else if constexpr (std::is_same_v<Peer, Network::IClient>) {
            static_assert(sizeof...(args) == 0);
            return DynamicCast<InfoT>(peer->ReceiveInfo(&typeid(InfoT)));
        }
        else {
            assert(0);
        }
    }

    /**
     * Helper of DelieveInfo for brief code.
     */
    template<typename InfoT, typename Peer, typename... Args>
    static void Deliver(std::shared_ptr<Peer> peer, Args... args) {
        if constexpr (std::is_same_v<Peer, Network::IServer>) {
            DeliverHelper<InfoT>(peer, args...);
        }
        else if constexpr (std::is_same_v<Peer, Network::IClient>) {
            peer->DeliverInfo(&typeid(InfoT), InfoT{args...});
        }
        else {
            assert(0);
        }
    }

private:
    template<typename InfoT, typename Peer>
    static std::unique_ptr<InfoT> ReceiveHelper(std::shared_ptr<Peer> server, int index) {
        return DynamicCast<InfoT>(server->ReceiveInfo(&typeid(InfoT), index));
    }

    template<typename InfoT, typename Peer, typename... Args>
    static void DeliverHelper(std::shared_ptr<Peer> server, int index, Args... args) {
        server->DeliverInfo(&typeid(InfoT), index, InfoT{args...});
    }
};
}}
