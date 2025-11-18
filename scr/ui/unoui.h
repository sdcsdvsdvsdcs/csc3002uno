#ifndef UNOUI_H
#define UNOUI_H

#include <vector>
#include <string>
#include "colour_render.h" 

// Card structure
struct Card {
    std::string color;
    std::string type;
    int number;
    
    Card(std::string c = "RED", std::string t = "NUMBER", int n = 1) 
        : color(c), type(t), number(n) {}
};

class UnoUI {
private:
    // Game state
    std::vector<Card> playerHand;
    Card currentCard;
    std::vector<std::string> playerNames;
    std::vector<int> playerCardCounts;
    int currentPlayerIndex;
    int selectedCardIndex;
    
    // UI state
    bool gameRunning;
    std::string message;
    bool needsRedraw;
    RenderCard cardRenderer; 

    //Character system
    std::vector<std::string> playerCharacters;
    std::vector<bool> skillAvailable;
    std::vector<int> skillUsesRemaining;
    std::vector<bool> skillCooldown;
    bool defenderRevealed;

public:
    UnoUI();
    void drawGameScreen();
    void handleInput();
    void drawCard();
    bool isGameRunning() const;
    void useSkill();
    void resetSkillCooldowns(); // For round management

private:
    void drawTopPanel();
    void drawMiddlePanel();
    void drawBottomPanel();
    void drawPlayerHand();
    void clearScreen();
    void setCursorPosition(int x, int y);
    void drawBox(int x, int y, int width, int height, const std::string& title = "");
    void drawCenteredText(int y, const std::string& text);
    void drawText(int x, int y, const std::string& text);
    int getConsoleWidth();
    int getConsoleHeight();
    void drawCharacterInfo();
    void drawSkillStatus();
};

#endif