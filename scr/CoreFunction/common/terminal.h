#pragma once

#if defined(__unix__) || defined(__APPLE__)
#include <termios.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#include <conio.h>
#endif

namespace UNO { namespace Common {

class Terminal {
public:
#if defined(__unix__) || defined(__APPLE__)
    /**
     * Save the old terminal mode.
     */
    Terminal();

    /**
     * Recover the old terminal mode. RAII.
     */
    ~Terminal();

    /**
     * Change the terminal mode so that input will auto flush 
     * (i.e. no longer need a '\n')
     */
    void SetModeAutoFlush();

    /**
     * Change the terminal mode so that input will not echo in the console.
     */
    void SetModeNoEcho();

    /**
     * Recover the old terminal explicitly.
     */
    void Recover();
#endif

    /**
     * Clear those chars that have been inputted but not consumed yet in the input buffer.
     */
    static void ClearStdInBuffer();

    /**
     * 新增：设置终端标题
     */
    static void SetTerminalTitle(const std::string& title);

    /**
     * 新增：获取终端尺寸
     */
    static std::pair<int, int> GetTerminalSize();

    /**
     * 新增：移动光标到指定位置
     */
    static void MoveCursorTo(int x, int y);

    /**
     * 新增：清屏
     */
    static void ClearScreen();

    /**
     * 新增：显示/隐藏光标
     */
    static void ShowCursor(bool show);

    /**
     * 新增：设置文本颜色
     */
    static void SetTextColor(const std::string& color);

    /**
     * 新增：重置文本属性
     */
    static void ResetTextAttributes();

#if defined(__unix__) || defined(__APPLE__)
private:
    void GetNewAttr();

    void ApplyNewAttr();

private:
    struct termios mNewAttr;
    struct termios mOldAttr;
#endif
};
}}
