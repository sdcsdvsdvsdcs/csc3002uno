#include "unoui.h"
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <algorithm>

UnoUI::UnoUI() : selectedCardIndex(0), gameRunning(true), currentPlayerIndex(0), needsRedraw(true) {
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
    drawCenteredText(94, "CONTROLS:  ");
    drawCenteredText(95, "A = Move Left    D = Move Right    W = Draw Card  ");
    drawCenteredText(96, "ENTER = Play Card    Q = Quit Game");
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