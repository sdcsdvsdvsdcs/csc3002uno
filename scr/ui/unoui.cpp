#include "unoui.h"
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <algorithm>
#include <random> 

UnoUI::UnoUI() : selectedCardIndex(0), gameRunning(true), currentPlayerIndex(0), 
                  needsRedraw(true), defenderRevealed(false) {
    currentCard = {"RED", "NUMBER", 5};
    playerHand = {
        {"RED", "NUMBER", 1},
        {"GREEN", "ACTION", 12},
        {"BLUE", "ACTION", 11},
        {"YELLOW", "ACTION", 13},
        {"WILD", "WILD", 0},
        {"RED", "NUMBER", 9}
    };
    playerNames = {"You", "CPU 1", "CPU 2", "CPU 3"};
    playerCardCounts = {static_cast<int>(playerHand.size()), 4, 7, 2};

    // Initialize characters with proper skill tracking
    playerCharacters = {"Lucky Star", "Collector", "Thief", "Defender"};
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(playerCharacters.begin(), playerCharacters.end(), g);

    // Initialize skill states
    skillAvailable.resize(4, true);
    skillCooldown.resize(4, false);
    skillUsesRemaining.resize(4, 0);

    // Set initial skill uses
    for (size_t i = 0; i < playerCharacters.size(); i++) {
        if (playerCharacters[i] == "Lucky Star") {
            skillUsesRemaining[i] = 3;
        } else {
            skillUsesRemaining[i] = 1; 
        }
    }

    message = "Your turn! Use A/D to navigate, ENTER to play, W to draw";

}

void UnoUI::clearScreen() {
    system("cls");
}

void UnoUI::setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void UnoUI::drawText(int x, int y, const std::string& text) {
    setCursorPosition(x, y);
    std::cout << text;
}

void UnoUI::drawCenteredText(int y, const std::string& text) {
    int width = getConsoleWidth();
    int x = (width - text.length()) / 2;
    drawText(std::max(0, x), y, text);
}

int UnoUI::getConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

int UnoUI::getConsoleHeight() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void UnoUI::drawTopPanel() {
    drawCenteredText(1, "UNO GAME");
    drawCenteredText(2, "====================");
    
    // Draw character info
    drawCharacterInfo();
    drawSkillStatus();
    
    std::cout << std::endl;
    
    drawText(5, 10, "Other Players:");
    std::cout << std::endl;
    
    drawText(5, 11, "CPU 1: 4 cards");
    drawText(25, 11, "CPU 2: 7 cards"); 
    drawText(45, 11, "CPU 3: 2 cards");
    std::cout << std::endl;
    
    drawCenteredText(13, "====================");
    std::cout << std::endl;
}

void UnoUI::drawCharacterInfo() {
    int width = getConsoleWidth();
    
    drawCenteredText(3, "CHARACTERS");
    drawCenteredText(4, "===========");
    
    // Display character assignments with skill status
    for (int i = 0; i < 4; i++) {
        int x = 5 + (i * 30);
        setCursorPosition(x, 6);
        
        std::string charInfo = (i == 0 ? "You: " : "CPU " + std::to_string(i) + ": ");
        charInfo += playerCharacters[i];
        
        // Add skill status indicator
        if (i == 0) { // Only show detailed status for human player
            if (!skillAvailable[0]) {
                charInfo += " (Used)";
            } else if (skillCooldown[0]) {
                charInfo += " (Cooldown)";
            } else {
                charInfo += " (Ready)";
            }
        }
        
        // Defender reveal status
        if (playerCharacters[i] == "Defender" && defenderRevealed) {
            charInfo += " [REVEALED]";
        }
        
        std::cout << charInfo;
    }
}

void UnoUI::drawSkillStatus() {
    if (currentPlayerIndex == 0 && skillAvailable[0] && !skillCooldown[0]) {
        std::string yourCharacter = playerCharacters[0];
        std::string skillInfo;
        
        if (yourCharacter == "Lucky Star") {
            skillInfo = "Lucky Star Skill (" + std::to_string(skillUsesRemaining[0]) + 
                       "/3 uses): Draw 3 cards, pick 1 (Cooldown: 1 round)";
        }
        else if (yourCharacter == "Collector") {
            skillInfo = "Collector Skill (1/1 use): Steal from discard pile (Cooldown: 1 round)";
        }
        else if (yourCharacter == "Thief") {
            skillInfo = "Thief Skill (1/1 use): Swap cards with opponent (Cooldown: 1 round)";
        }
        else if (yourCharacter == "Defender") {
            skillInfo = "Defender Skill: Passive defense (Auto-activates)";
        }
        
        drawCenteredText(8, skillInfo);
    }
}

void UnoUI::useSkill() {
    if (currentPlayerIndex != 0) {
        message = "Not your turn!";
        needsRedraw = true;
        return;
    }
    
    std::string yourCharacter = playerCharacters[0];
    
    if (!skillAvailable[0]) {
        message = "Skill unavailable!";
        needsRedraw = true;
        return;
    }
    
    if (skillCooldown[0]) {
        message = "Skill in cooldown! Available next round.";
        needsRedraw = true;
        return;
    }
    
    if (skillUsesRemaining[0] <= 0) {
        message = "No skill uses remaining!";
        needsRedraw = true;
        return;
    }
    
    // Character-specific skill execution
    if (yourCharacter == "Lucky Star") {
        message = "Lucky Star: Viewing top 3 cards... (Backend: implement card selection)";
        skillUsesRemaining[0]--;
        if (skillUsesRemaining[0] <= 0) {
            skillAvailable[0] = false;
            message += " FINAL USE! Skill permanently disabled.";
        } else {
            skillCooldown[0] = true;
            message += " Cooldown activated for next round.";
        }
    }
    else if (yourCharacter == "Collector") {
        message = "Collector: Viewing discard pile... (Backend: implement card steal from discard)";
        skillUsesRemaining[0] = 0;
        skillCooldown[0] = true;
        skillAvailable[0] = false;
    }
    else if (yourCharacter == "Thief") {
        message = "Thief: Selecting target... (Backend: implement player selection and card swap)";
        skillUsesRemaining[0] = 0;
        skillCooldown[0] = true;
        skillAvailable[0] = false;
    }
    else if (yourCharacter == "Defender") {
        message = "Defender: Passive skill - activates automatically when attacked!";
        // Defender doesn't actively use skill
        return;
    }
    
    needsRedraw = true;
}

void UnoUI::resetSkillCooldowns() {
    // Called at the start of each round
    for (size_t i = 0; i < skillCooldown.size(); i++) {
        if (skillCooldown[i] && skillUsesRemaining[i] > 0) {
            skillCooldown[i] = false;
            if (playerCharacters[i] != "Lucky Star") {
                skillAvailable[i] = true;
            }
        }
    }
}

void UnoUI::drawMiddlePanel() {
    int width = getConsoleWidth();
    
    drawCenteredText(9, "CURRENT CARD");
    drawCenteredText(10, "============");
    std::cout << std::endl;
    
    // Make space for the card
    setCursorPosition(width/2 - 15, 12);
    
    // Render the current card with proper spacing
    setCursorPosition(width/2 - 15, 13);
    cardRenderer.render_card(currentCard.color, currentCard.type, currentCard.number);
    
    // Message below the card with proper spacing
    std::cout << std::endl;
    drawCenteredText(48, message);
    std::cout << std::endl;
    
    drawCenteredText(50, "====================");
    std::cout << std::endl;
}

void UnoUI::drawPlayerHand() {
    int width = getConsoleWidth();
    
    // Calculate center position for both lines using the longer string
    std::string handTitle = "YOUR HAND";
    
    // Both strings are the same length (9 characters), so they should center the same
    int titleX = (width - handTitle.length()) / 2;
    
    // Draw both lines centered
    setCursorPosition(titleX, 52);
    std::cout << handTitle;
    
    std::cout << std::endl;
    
    // Rest of the function remains the same...
    int maxCardsToShow = std::min(3, static_cast<int>(playerHand.size()));
    int startIndex = 0;
    
    if (playerHand.size() > maxCardsToShow) {
        startIndex = std::max(0, selectedCardIndex - 1);
        if (startIndex + maxCardsToShow > playerHand.size()) {
            startIndex = playerHand.size() - maxCardsToShow;
        }
    }
    
    int cardSpacing = width / (maxCardsToShow + 1);
    
    for (int i = startIndex; i < startIndex + maxCardsToShow && i < playerHand.size(); i++) {
        int cardX = cardSpacing * (i - startIndex + 1) - 15;
        
        // Selection indicator above the chunk - WITH NEWLINE
        if (i == selectedCardIndex) {
            setCursorPosition(cardX + 5, 55);
            std::cout << ">>> SELECTED <<<" << std::endl;
        } else {
            setCursorPosition(cardX + 5, 55);
            std::cout << "                " << std::endl;
        }
        
        // Card name on the next line
        setCursorPosition(cardX, 56);
        std::string cardName = "Card " + std::to_string(i + 1) + ": " + playerHand[i].color + " ";
        if (playerHand[i].type == "NUMBER") {
            cardName += std::to_string(playerHand[i].number);
        } else if (playerHand[i].type == "ACTION") {
            if (playerHand[i].number == 11) cardName += "REVERSE";
            else if (playerHand[i].number == 12) cardName += "SKIP";
            else if (playerHand[i].number == 13) cardName += "DRAW 2";
            else cardName += "PACKAGE";
        } else {
            cardName += "WILD";
        }
        std::cout << cardName;
        
        // Card art on the next line
        setCursorPosition(cardX, 57);
        cardRenderer.render_card(playerHand[i].color, playerHand[i].type, playerHand[i].number);
        
        // Empty line after chunk
        setCursorPosition(cardX, 92);
        std::cout << std::endl;
    }
    
    // Navigation hints
    if (startIndex > 0) {
        setCursorPosition(10, 95);
        std::cout << "<-- [A] Previous Cards";
    } else {
        setCursorPosition(10, 95);
        std::cout << "                      ";
    }
    
    if (startIndex + maxCardsToShow < playerHand.size()) {
        setCursorPosition(width - 30, 95);
        std::cout << "[D] Next Cards -->";
    } else {
        setCursorPosition(width - 30, 95);
        std::cout << "                  ";
    }
    
    std::cout << std::endl;
}

void UnoUI::drawBottomPanel() {
    std::cout << std::endl;
    drawCenteredText(94, "CONTROLS: ");
    
    // Only show skill button if skill is available
    if (currentPlayerIndex == 0 && skillAvailable[0] && !skillCooldown[0] && 
        skillUsesRemaining[0] > 0 && playerCharacters[0] != "Defender") {
        drawCenteredText(95, "A=Left D=Right W=Draw S=Skill ");
    } else {
        drawCenteredText(95, "A=Left D=Right W=Draw ");
    }
    
    drawCenteredText(96, "ENTER=Play Q=Quit");
    drawCenteredText(97, "============================");
}

void UnoUI::drawGameScreen() {
    if (needsRedraw) {
        clearScreen();
        
        drawTopPanel();
        drawMiddlePanel(); 
        drawPlayerHand();
        drawBottomPanel();
        
        // Make sure cursor is at bottom
        setCursorPosition(0, 100);
        
        needsRedraw = false;
    }
}

void UnoUI::drawCard() {
    // Simulate drawing a random card
    static std::vector<Card> drawPile = {
        {"RED", "NUMBER", 7},
        {"BLUE", "NUMBER", 2},
        {"GREEN", "ACTION", 12}, // SKIP
        {"YELLOW", "ACTION", 13}, // DRAW 2
        {"WILD", "WILD", 0},
        {"RED", "NUMBER", 4},
        {"BLUE", "ACTION", 11} // REVERSE
    };
    
    static int drawCounter = 0;
    
    if (drawCounter < drawPile.size()) {
        Card newCard = drawPile[drawCounter];
        playerHand.push_back(newCard);
        drawCounter++;
        
        std::string cardName;
        if (newCard.type == "NUMBER") {
            cardName = std::to_string(newCard.number);
        } else if (newCard.type == "ACTION") {
            if (newCard.number == 11) cardName = "REVERSE";
            else if (newCard.number == 12) cardName = "SKIP";
            else cardName = "DRAW 2";
        } else {
            cardName = "WILD";
        }
        
        message = "Drew: " + newCard.color + " " + cardName;
    } else {
        message = "Draw pile is empty!";
    }
    
    needsRedraw = true;
}

void UnoUI::handleInput() {
    if (_kbhit()) {
        char ch = _getch();
        
        // Simple keyboard controls (no arrow keys)
        switch (ch) {
            case 'a':
            case 'A':
                if (selectedCardIndex > 0) {
                    selectedCardIndex--;
                    message = "Selected Card " + std::to_string(selectedCardIndex + 1);
                    needsRedraw = true;
                }
                break;
                
            case 'd':
            case 'D':
                if (selectedCardIndex < playerHand.size() - 1) {
                    selectedCardIndex++;
                    message = "Selected Card " + std::to_string(selectedCardIndex + 1);
                    needsRedraw = true;
                }
                break;
                
            case 'w':
            case 'W': 
                drawCard();
                break;
                
            case 13: // Enter key - Play card
                if (!playerHand.empty()) {
                    Card played = playerHand[selectedCardIndex];
                    
                    std::string cardName;
                    if (played.type == "NUMBER") {
                        cardName = std::to_string(played.number);
                    } else if (played.type == "ACTION") {
                        if (played.number == 11) cardName = "REVERSE";
                        else if (played.number == 12) cardName = "SKIP";
                        else cardName = "DRAW 2";
                    } else {
                        cardName = "WILD";
                    }
                    
                    message = "Playing " + played.color + " " + cardName + "...";
                    
                    playerHand.erase(playerHand.begin() + selectedCardIndex);
                    currentCard = played;
                    
                    if (selectedCardIndex >= playerHand.size() && !playerHand.empty()) {
                        selectedCardIndex = playerHand.size() - 1;
                    }
                    
                    needsRedraw = true;
                } else {
                    message = "No cards to play! Press W to draw a card.";
                    needsRedraw = true;
                }
                break;
                
            case 'q':
            case 'Q':
                gameRunning = false;
                message = "Thanks for playing!";
                needsRedraw = true;
                break;
        }
    }
}

bool UnoUI::isGameRunning() const {
    return gameRunning;
}