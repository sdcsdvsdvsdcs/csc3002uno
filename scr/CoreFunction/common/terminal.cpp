#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#endif
#include <iostream>
#include "terminal.h"

namespace UNO { namespace Common {

#if defined(__unix__) || defined(__APPLE__)
Terminal::Terminal()
{
    /// XXX: what if throwing an exception
    // save the old attr
    tcgetattr(STDIN_FILENO, &mOldAttr);
}

void Terminal::GetNewAttr()
{
    tcgetattr(STDIN_FILENO, &mNewAttr);
}

void Terminal::SetModeAutoFlush()
{
    GetNewAttr();
    mNewAttr.c_lflag &= ~ICANON;
    mNewAttr.c_lflag &= ~ECHO;
    ApplyNewAttr();
}

void Terminal::SetModeNoEcho()
{
    GetNewAttr();
    mNewAttr.c_lflag &= ~ECHO;
    ApplyNewAttr();
}

void Terminal::ApplyNewAttr()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &mNewAttr);
}

void Terminal::Recover()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &mOldAttr);
}

Terminal::~Terminal()
{
    Recover();
}
#endif

void Terminal::ClearStdInBuffer()
{
#if defined(__unix__) || defined(__APPLE__)
    tcflush(STDIN_FILENO, TCIFLUSH);
#elif defined(_WIN32)
    while (true) {
        auto ret = _kbhit();
        if (ret != 0) {
            _getch();
        }
        else {
            break;
        }
    }
#endif
}

void Terminal::SetTerminalTitle(const std::string& title)
{
#if defined(__unix__) || defined(__APPLE__)
    std::cout << "\033]0;" << title << "\007";
#elif defined(_WIN32)
    SetConsoleTitleA(title.c_str());
#endif
}

std::pair<int, int> Terminal::GetTerminalSize()
{
#if defined(__unix__) || defined(__APPLE__)
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    return {size.ws_col, size.ws_row};
#elif defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return {csbi.srWindow.Right - csbi.srWindow.Left + 1, 
            csbi.srWindow.Bottom - csbi.srWindow.Top + 1};
#endif
    return {80, 24}; // 默认值
}

void Terminal::MoveCursorTo(int x, int y)
{
#if defined(__unix__) || defined(__APPLE__)
    std::cout << "\033[" << y << ";" << x << "H";
#elif defined(_WIN32)
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#endif
}

void Terminal::ClearScreen()
{
#if defined(__unix__) || defined(__APPLE__)
    std::cout << "\033[2J";
#elif defined(_WIN32)
    system("cls");
#endif
}

void Terminal::ShowCursor(bool show)
{
#if defined(__unix__) || defined(__APPLE__)
    std::cout << (show ? "\033[?25h" : "\033[?25l");
#elif defined(_WIN32)
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cinfo;
    cinfo.bVisible = show;
    cinfo.dwSize = 1;
    SetConsoleCursorInfo(hOutput, &cinfo);
#endif
}

void Terminal::SetTextColor(const std::string& color)
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
        std::cout << it->second;
    }
#elif defined(_WIN32)
    static std::map<std::string, WORD> colorMap = {
        {"red", FOREGROUND_RED | FOREGROUND_INTENSITY},
        {"green", FOREGROUND_GREEN | FOREGROUND_INTENSITY},
        {"yellow", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY},
        {"blue", FOREGROUND_BLUE | FOREGROUND_INTENSITY},
        {"magenta", FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY},
        {"cyan", FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY},
        {"white", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY},
        {"reset", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE}
    };
    
    auto it = colorMap.find(color);
    if (it != colorMap.end()) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), it->second);
    }
#endif
}

void Terminal::ResetTextAttributes()
{
#if defined(__unix__) || defined(__APPLE__)
    std::cout << "\033[0m";
#elif defined(_WIN32)
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                           FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
}

}}
