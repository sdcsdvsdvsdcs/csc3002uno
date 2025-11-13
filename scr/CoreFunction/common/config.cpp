#include "config.h"

namespace UNO { namespace Common {

// 定义静态常量
const std::string Config::CMD_OPT_SHORT_LISTEN = "l";
const std::string Config::CMD_OPT_LONG_LISTEN = "listen";
const std::string Config::CMD_OPT_BOTH_LISTEN = CMD_OPT_SHORT_LISTEN + ", " + CMD_OPT_LONG_LISTEN;
const std::string Config::CMD_OPT_SHORT_CONNECT = "c";
const std::string Config::CMD_OPT_LONG_CONNECT = "connect";
const std::string Config::CMD_OPT_BOTH_CONNECT = CMD_OPT_SHORT_CONNECT + ", " + CMD_OPT_LONG_CONNECT;
const std::string Config::CMD_OPT_SHORT_USERNAME = "u";
const std::string Config::CMD_OPT_LONG_USERNAME = "username";
const std::string Config::CMD_OPT_BOTH_USERNAME = CMD_OPT_SHORT_USERNAME + ", " + CMD_OPT_LONG_USERNAME;
const std::string Config::CMD_OPT_SHORT_PLAYERS = "n";
const std::string Config::CMD_OPT_LONG_PLAYERS = "players";
const std::string Config::CMD_OPT_BOTH_PLAYERS = CMD_OPT_SHORT_PLAYERS + ", " + CMD_OPT_LONG_PLAYERS;
const std::string Config::CMD_OPT_SHORT_CFGFILE = "f";
const std::string Config::CMD_OPT_LONG_CFGFILE = "file";
const std::string Config::CMD_OPT_BOTH_CFGFILE = CMD_OPT_SHORT_CFGFILE + ", " + CMD_OPT_LONG_CFGFILE;
const std::string Config::CMD_OPT_LONG_LOGFILE = "log";
const std::string Config::CMD_OPT_SHORT_VERSION = "v";
const std::string Config::CMD_OPT_LONG_VERSION = "version";
const std::string Config::CMD_OPT_BOTH_VERSION = CMD_OPT_SHORT_VERSION + ", " + CMD_OPT_LONG_VERSION;
const std::string Config::CMD_OPT_SHORT_HELP = "h";
const std::string Config::CMD_OPT_LONG_HELP = "help";
const std::string Config::CMD_OPT_BOTH_HELP = CMD_OPT_SHORT_HELP + ", " + CMD_OPT_LONG_HELP;

// 新增：游戏模式选项
const std::string Config::CMD_OPT_SHORT_MODE = "m";
const std::string Config::CMD_OPT_LONG_MODE = "mode";
const std::string Config::CMD_OPT_BOTH_MODE = CMD_OPT_SHORT_MODE + ", " + CMD_OPT_LONG_MODE;
const std::string Config::CMD_OPT_LONG_NO_CHARACTERS = "no-characters";

const std::string Config::FILE_OPT_SERVER = "server";
const std::string Config::FILE_OPT_CLIENT = "client";
const std::string Config::FILE_OPT_LISTEN = "listenOn";
const std::string Config::FILE_OPT_CONNECT = "connectTo";
const std::string Config::FILE_OPT_USERNAME = "username";
const std::string Config::FILE_OPT_PLAYERS = "playerNum";
const std::string Config::FILE_OPT_RED = "red";
const std::string Config::FILE_OPT_YELLOW = "yellow";
const std::string Config::FILE_OPT_GREEN = "green";
const std::string Config::FILE_OPT_BLUE = "blue";
const std::string Config::FILE_OPT_WILD = "wild";

// 新增：配置文件选项
const std::string Config::FILE_OPT_GAME_MODE = "gameMode";
const std::string Config::FILE_OPT_ENABLE_CHARACTERS = "enableCharacters";
const std::string Config::FILE_OPT_MAX_SKILL_USES = "maxSkillUses";

// define static common variables here
int Common::mPlayerNum;
int Common::mTimeoutPerTurn;
int Common::mHandCardsNumPerRow;
bool Common::mEnableCharacterSystem;
int Common::mMaxSkillUsesPerGame;
std::string Common::mRedEscape;
std::string Common::mYellowEscape;
std::string Common::mGreenEscape;
std::string Common::mBlueEscape;
std::string Common::mWildEscape;
Common::GameMode Common::mGameMode;

const std::map<std::string, std::string> Common::mEscapeMap = {
    {"red",          "\033[31m"},
    {"yellow",       "\033[33m"},
    {"green",        "\033[32m"},
    {"blue",         "\033[34m"},
    {"cyan",         "\033[36m"},
    {"magenta",      "\033[35m"},
    {"brightRed",    "\033[91m"},
    {"brightYellow", "\033[93m"},
    {"brightGreen",  "\033[92m"},
    {"brightBlue",   "\033[94m"},
    {"brightCyan",   "\033[96m"},
    {"brightMagenta","\033[95m"}
};

Config::Config(int argc, const char **argv)
{
    mOptions = std::make_unique<cxxopts::Options>("uno", "UNO - uno card game with character system");
    mOptions->add_options()
        (CMD_OPT_BOTH_LISTEN, "the port number that server will listen on", cxxopts::value<std::string>())
        (CMD_OPT_BOTH_CONNECT, "the endpoint that client (player) will connect to", cxxopts::value<std::string>())
        (CMD_OPT_BOTH_USERNAME, "the username of the player", cxxopts::value<std::string>())
        (CMD_OPT_BOTH_PLAYERS, "the number of players", cxxopts::value<int>())
        (CMD_OPT_BOTH_CFGFILE, "the path of config file", cxxopts::value<std::string>())
        (CMD_OPT_LONG_LOGFILE, "the path of log file", cxxopts::value<std::string>())
        (CMD_OPT_BOTH_MODE, "game mode: classic, characters, custom", cxxopts::value<std::string>())
        (CMD_OPT_LONG_NO_CHARACTERS, "disable character system", cxxopts::value<bool>())
        (CMD_OPT_BOTH_VERSION, "show version of application", cxxopts::value<bool>())
        (CMD_OPT_BOTH_HELP, "show help info", cxxopts::value<bool>());
    
    try {
        mCmdlineOpts = std::make_unique<cxxopts::ParseResult>(mOptions->parse(argc, argv));
    }
    catch (std::exception &e) {
        std::cout << mOptions->help() << std::endl;
        std::cout << e.what() << std::endl;
        std::exit(-1);
    }
    std::string configFile;
    if (mCmdlineOpts->count(CMD_OPT_LONG_CFGFILE)) {
        auto configFile = (*mCmdlineOpts)[CMD_OPT_LONG_CFGFILE].as<std::string>();
        auto rootNode = YAML::LoadFile(configFile);
        if (rootNode[FILE_OPT_SERVER].IsDefined()) {
            mServerNode = std::make_unique<YAML::Node>(rootNode[FILE_OPT_SERVER]);
        }
        if (rootNode[FILE_OPT_CLIENT].IsDefined()) {
            mClientNode = std::make_unique<YAML::Node>(rootNode[FILE_OPT_CLIENT]);
        }
    }
}

std::unique_ptr<GameConfigInfo> Config::Parse()
{
    HandleImmediateConfig();
    // options from command line takes precedence over ones from config file
    ParseFileOpts();
    try {
        ParseCmdlineOpts();
    }
    catch (std::exception &e) {
        std::cout << mOptions->help() << std::endl;
        std::cout << e.what() << std::endl;
        std::exit(-1);
    }

    // handle common config here
    SetUpCommonConfig();

    // the main function will handle game config
    return std::move(mGameConfigInfo);
}

void Config::HandleImmediateConfig()
{
    if (mCmdlineOpts->count(CMD_OPT_LONG_HELP)) {
        std::cout << mOptions->help() << std::endl;
        std::exit(0);
    }
    if (mCmdlineOpts->count(CMD_OPT_LONG_VERSION)) {
        std::cout << "uno version 2.0 with character system" << std::endl;
        std::exit(0);
    }
}

void Config::ParseFileOpts()
{
    // parse server node
    if (mServerNode && mCmdlineOpts->count(CMD_OPT_LONG_LISTEN)) {
        if ((*mServerNode)[FILE_OPT_PLAYERS].IsDefined()) {
            mCommonConfigInfo->mPlayerNum = (*mServerNode)[FILE_OPT_PLAYERS].as<int>();
        }
        if ((*mServerNode)[FILE_OPT_GAME_MODE].IsDefined()) {
            mCommonConfigInfo->mGameMode = (*mServerNode)[FILE_OPT_GAME_MODE].as<std::string>();
        }
        if ((*mServerNode)[FILE_OPT_ENABLE_CHARACTERS].IsDefined()) {
            mCommonConfigInfo->mEnableCharacters = (*mServerNode)[FILE_OPT_ENABLE_CHARACTERS].as<bool>();
        }
        if ((*mServerNode)[FILE_OPT_MAX_SKILL_USES].IsDefined()) {
            mCommonConfigInfo->mMaxSkillUses = (*mServerNode)[FILE_OPT_MAX_SKILL_USES].as<int>();
        }
    }

    // parse client node
    if (mClientNode && mCmdlineOpts->count(CMD_OPT_LONG_CONNECT)) {
        if ((*mClientNode)[FILE_OPT_USERNAME].IsDefined()) {
            mGameConfigInfo->mUsername = (*mClientNode)[FILE_OPT_USERNAME].as<std::string>();
        }
        if ((*mClientNode)[FILE_OPT_RED].IsDefined()) {
            mCommonConfigInfo->mRedEscape = (*mClientNode)[FILE_OPT_RED].as<std::string>();
        }
        if ((*mClientNode)[FILE_OPT_YELLOW].IsDefined()) {
            mCommonConfigInfo->mYellowEscape = (*mClientNode)[FILE_OPT_YELLOW].as<std::string>();
        }
        if ((*mClientNode)[FILE_OPT_GREEN].IsDefined()) {
            mCommonConfigInfo->mGreenEscape = (*mClientNode)[FILE_OPT_GREEN].as<std::string>();
        }
        if ((*mClientNode)[FILE_OPT_BLUE].IsDefined()) {
            mCommonConfigInfo->mBlueEscape = (*mClientNode)[FILE_OPT_BLUE].as<std::string>();
        }
        if ((*mClientNode)[FILE_OPT_WILD].IsDefined()) {
            mCommonConfigInfo->mWildEscape = (*mClientNode)[FILE_OPT_WILD].as<std::string>();
        }
        if ((*mClientNode)[FILE_OPT_GAME_MODE].IsDefined()) {
            mCommonConfigInfo->mGameMode = (*mClientNode)[FILE_OPT_GAME_MODE].as<std::string>();
        }
    }
}

void Config::ParseCmdlineOpts()
{
    // check options
    if (mCmdlineOpts->count(CMD_OPT_LONG_LISTEN) && mCmdlineOpts->count(CMD_OPT_LONG_CONNECT)) {
        throw std::runtime_error("cannot specify both -l and -c options at the same time");
    }
    if (!mCmdlineOpts->count(CMD_OPT_LONG_LISTEN) && !mCmdlineOpts->count(CMD_OPT_LONG_CONNECT)) {
        throw std::runtime_error("must specify either -l or -c option");
    }
    if (mCmdlineOpts->count(CMD_OPT_LONG_CONNECT) && !mCmdlineOpts->count(CMD_OPT_LONG_USERNAME)
        && (!mClientNode || !(*mClientNode)[FILE_OPT_USERNAME].IsDefined())) {
        throw std::runtime_error("must specify -u option if -c option is specified");
    }
    if (mCmdlineOpts->count(CMD_OPT_LONG_CONNECT) && mCmdlineOpts->count(CMD_OPT_LONG_PLAYERS)) {
        throw std::runtime_error("only server side can specify -n option");
    }

    // -l
    if (mCmdlineOpts->count(CMD_OPT_LONG_LISTEN)) {
        mGameConfigInfo->mIsServer = true;
        mGameConfigInfo->mPort = (*mCmdlineOpts)[CMD_OPT_LONG_LISTEN].as<std::string>();
    }

    // -c
    if (mCmdlineOpts->count(CMD_OPT_LONG_CONNECT)) {
        mGameConfigInfo->mIsServer = false;
        std::string endpoint = (*mCmdlineOpts)[CMD_OPT_LONG_CONNECT].as<std::string>();
        int pos = endpoint.find(":");
        mGameConfigInfo->mHost = endpoint.substr(0, pos);
        mGameConfigInfo->mPort = endpoint.substr(pos + 1);
    }

    // -u
    if (mCmdlineOpts->count(CMD_OPT_LONG_USERNAME)) {
        mGameConfigInfo->mUsername = (*mCmdlineOpts)[CMD_OPT_LONG_USERNAME].as<std::string>();
    }

    // -n
    if (mCmdlineOpts->count(CMD_OPT_LONG_PLAYERS)) {
        mCommonConfigInfo->mPlayerNum = (*mCmdlineOpts)[CMD_OPT_LONG_PLAYERS].as<int>();
    }

    // --log
    if (mCmdlineOpts->count(CMD_OPT_LONG_LOGFILE)) {
        mGameConfigInfo->mLogPath = (*mCmdlineOpts)[CMD_OPT_LONG_LOGFILE].as<std::string>();
    }

    // -m / --mode
    if (mCmdlineOpts->count(CMD_OPT_LONG_MODE)) {
        mCommonConfigInfo->mGameMode = (*mCmdlineOpts)[CMD_OPT_LONG_MODE].as<std::string>();
    }

    // --no-characters
    if (mCmdlineOpts->count(CMD_OPT_LONG_NO_CHARACTERS)) {
        mCommonConfigInfo->mEnableCharacters = false;
    }
}

void Config::SetUpCommonConfig()
{
    Common::mPlayerNum = mCommonConfigInfo->mPlayerNum.value_or(3);
    Common::mTimeoutPerTurn = 15;
    Common::mHandCardsNumPerRow = 8;
    
    // 设置角色系统
    Common::mEnableCharacterSystem = mCommonConfigInfo->mEnableCharacters.value_or(true);
    Common::mMaxSkillUsesPerGame = mCommonConfigInfo->mMaxSkillUses.value_or(3);
    
    // 设置游戏模式
    if (mCommonConfigInfo->mGameMode) {
        std::string mode = mCommonConfigInfo->mGameMode.value();
        if (mode == "classic") {
            Common::mGameMode = Common::GameMode::CLASSIC;
            Common::mEnableCharacterSystem = false;
        } else if (mode == "characters") {
            Common::mGameMode = Common::GameMode::WITH_CHARACTERS;
            Common::mEnableCharacterSystem = true;
        } else if (mode == "custom") {
            Common::mGameMode = Common::GameMode::CUSTOM;
        }
    } else {
        Common::mGameMode = Common::mEnableCharacterSystem ? 
            Common::GameMode::WITH_CHARACTERS : Common::GameMode::CLASSIC;
    }
    
    // 设置颜色转义序列
    auto redIter = Common::mEscapeMap.find(mCommonConfigInfo->mRedEscape.value_or("red"));
    Common::mRedEscape = (redIter == Common::mEscapeMap.end()) ? 
        Common::mEscapeMap.at("red") : redIter->second;

    auto yellowIter = Common::mEscapeMap.find(mCommonConfigInfo->mYellowEscape.value_or("yellow"));
    Common::mYellowEscape = (yellowIter == Common::mEscapeMap.end()) ? 
        Common::mEscapeMap.at("yellow") : yellowIter->second;

    auto greenIter = Common::mEscapeMap.find(mCommonConfigInfo->mGreenEscape.value_or("green"));
    Common::mGreenEscape = (greenIter == Common::mEscapeMap.end()) ? 
        Common::mEscapeMap.at("green") : greenIter->second;

    auto blueIter = Common::mEscapeMap.find(mCommonConfigInfo->mBlueEscape.value_or("blue"));
    Common::mBlueEscape = (blueIter == Common::mEscapeMap.end()) ? 
        Common::mEscapeMap.at("blue") : blueIter->second;

    auto wildIter = Common::mEscapeMap.find(mCommonConfigInfo->mWildEscape.value_or("brightMagenta"));
    Common::mWildEscape = (wildIter == Common::mEscapeMap.end()) ? 
        Common::mEscapeMap.at("brightMagenta") : wildIter->second;
}
}}
