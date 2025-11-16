#include "unoui.h"
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <algorithm>

UnoUI::UnoUI() : selectedCardIndex(0), gameRunning(true), currentPlayerIndex(0), needsRedraw(true) {
    // Demo data
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
    message = "Your turn! Select a card and press ENTER to play";
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
    // Simple header with better spacing
    drawCenteredText(1, "UNO GAME");
    drawCenteredText(2, "====================");
    std::cout << std::endl;
    
    drawText(5, 4, "Other Players:");
    std::cout << std::endl;
    
    drawText(5, 5, "CPU 1: 4 cards");
    drawText(25, 5, "CPU 2: 7 cards"); 
    drawText(45, 5, "CPU 3: 2 cards");
    std::cout << std::endl;
    
    drawCenteredText(7, "====================");
    std::cout << std::endl;
}

void UnoUI::drawMiddlePanel() {
    int width = getConsoleWidth();
    
    drawCenteredText(9, "CURRENT CARD");
    drawCenteredText(10, "============");
    std::cout << std::endl;
    
    // Make space for the card
    setCursorPosition(width/2 - 15, 12);
    std::cout << "Current Card:" << std::endl;
    
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
    
    drawCenteredText(52, "YOUR HAND");
    drawCenteredText(53, "=========");
    std::cout << std::endl;
    
    // Show cards with navigation
    int maxCardsToShow = std::min(3, static_cast<int>(playerHand.size()));
    int startIndex = 0;
    
    if (playerHand.size() > maxCardsToShow) {
        startIndex = std::max(0, selectedCardIndex - 1);
        if (startIndex + maxCardsToShow > playerHand.size()) {
            startIndex = playerHand.size() - maxCardsToShow;
        }
    }
    
    int endIndex = startIndex + maxCardsToShow;
    
    // Calculate positions for cards
    int cardSpacing = width / (maxCardsToShow + 1);
    
    for (int i = startIndex; i < endIndex && i < playerHand.size(); i++) {
        int cardX = cardSpacing * (i - startIndex + 1) - 15;
        
        // Selection indicator
        if (i == selectedCardIndex) {
            setCursorPosition(cardX + 5, 55);
            std::cout << ">>> SELECTED <<<";
        } else {
            setCursorPosition(cardX + 8, 55);
            std::cout << "             "; // Clear previous selection
        }
        
        // Render the card
        setCursorPosition(cardX, 56);
        cardRenderer.render_card(playerHand[i].color, playerHand[i].type, playerHand[i].number);
        
        // Card label below
        setCursorPosition(cardX + 5, 91);
        std::cout << "Card " << (i + 1);
        
        // Show card info
        setCursorPosition(cardX, 92);
        std::string cardInfo = playerHand[i].color + " ";
        if (playerHand[i].type == "NUMBER") {
            cardInfo += std::to_string(playerHand[i].number);
        } else if (playerHand[i].type == "ACTION") {
            if (playerHand[i].number == 11) cardInfo += "REVERSE";
            else if (playerHand[i].number == 12) cardInfo += "SKIP";
            else cardInfo += "DRAW 2";
        } else {
            cardInfo += "WILD";
        }
        std::cout << cardInfo;
    }
    
    // Scroll indicators if needed
    if (startIndex > 0) {
        setCursorPosition(10, 75);
        std::cout << "<-- [A] Previous";
    } else {
        setCursorPosition(10, 75);
        std::cout << "               ";
    }
    
    if (endIndex < playerHand.size()) {
        setCursorPosition(width - 25, 75);
        std::cout << "[D] Next -->";
    } else {
        setCursorPosition(width - 25, 75);
        std::cout << "            ";
    }
    
    std::cout << std::endl;
}

void UnoUI::drawBottomPanel() {
    std::cout << std::endl;
    drawCenteredText(94, "CONTROLS:");
    drawCenteredText(95, "A = Move Left    D = Move Right");
    drawCenteredText(96, "ENTER = Play Card    Q = Quit Game");
    drawCenteredText(97, "========================================");
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

void UnoUI::handleInput() {
    if (_kbhit()) {
        char ch = _getch();
        
        // Debug: print what key was pressed
        // std::cout << "Key pressed: " << (int)ch << std::endl;
        
        switch (ch) {
            case 'a':
            case 'A':
                if (selectedCardIndex > 0) {
                    selectedCardIndex--;
                    message = "Selected Card " + std::to_string(selectedCardIndex + 1);
                    needsRedraw = true;
                    std::cout << "Moved left to card " << (selectedCardIndex + 1) << std::endl;
                }
                break;
                
            case 'd':
            case 'D':
                if (selectedCardIndex < playerHand.size() - 1) {
                    selectedCardIndex++;
                    message = "Selected Card " + std::to_string(selectedCardIndex + 1);
                    needsRedraw = true;
                    std::cout << "Moved right to card " << (selectedCardIndex + 1) << std::endl;
                }
                break;
                
            case 13: // Enter key
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
                    
                    // Remove card and update current card
                    playerHand.erase(playerHand.begin() + selectedCardIndex);
                    currentCard = played;
                    
                    // Adjust selection if needed
                    if (selectedCardIndex >= playerHand.size() && !playerHand.empty()) {
                        selectedCardIndex = playerHand.size() - 1;
                    }
                    
                    needsRedraw = true;
                    std::cout << "Played card!" << std::endl;
                }
                break;
                
            case 'q':
            case 'Q':
                gameRunning = false;
                message = "Thanks for playing!";
                needsRedraw = true;
                break;
                
            default:
                // Ignore other keys
                break;
        }
    }
}

bool UnoUI::isGameRunning() const {
    return gameRunning;
}