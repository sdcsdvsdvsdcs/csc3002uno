#pragma once

#include <string>
#include <map>

namespace UNO { namespace Common {

class Common {
public:
    static int mPlayerNum;
    static int mTimeoutPerTurn;
    static int mHandCardsNumPerRow;
    
    // 新增：角色系统相关配置
    static bool mEnableCharacterSystem;
    static int mMaxSkillUsesPerGame;
    
    // color escape
    static std::string mRedEscape;
    static std::string mYellowEscape;
    static std::string mGreenEscape;
    static std::string mBlueEscape;
    static std::string mWildEscape;
    
    const static std::map<std::string, std::string> mEscapeMap;
    
    // 新增：游戏模式
    enum class GameMode {
        CLASSIC,
        WITH_CHARACTERS,
        CUSTOM
    };
    static GameMode mGameMode;
};
}}
