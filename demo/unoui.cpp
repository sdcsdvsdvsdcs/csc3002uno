#include "unoui.h"
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <algorithm>
#include <random>

// Helper function to convert CardColor to string
std::string getColorString(CardColor color) {
    switch(color) {
        case CardColor::RED: return "RED";
        case CardColor::BLUE: return "BLUE";
        case CardColor::GREEN: return "GREEN";
        case CardColor::YELLOW: return "YELLOW";
        default: return "WILD";
    }
}

UnoUI::UnoUI() : selectedCardIndex(0), gameRunning(true), needsRedraw(true) {
    // Initialize backend with players
    gameEngine.addPlayer("You");
    gameEngine.addPlayer("CPU 1");
    gameEngine.addPlayer("CPU 2"); 
    gameEngine.addPlayer("CPU 3");
    gameEngine.startGame();
    
    message = "Your turn! Use A/D to navigate, ENTER to play, W to draw, S for skill";
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

void UnoUI::drawCharacterInfo() {
    int width = getConsoleWidth();
    
    drawCenteredText(3, "CHARACTERS");
    drawCenteredText(4, "===========");
    
    // Get characters from backend
    std::vector<std::string> characters = gameEngine.getPlayerCharacters();
    
    for (int i = 0; i < 4 && i < characters.size(); i++) {
        int x = 5 + (i * 30);
        setCursorPosition(x, 6);
        
        std::string charInfo = (i == 0 ? "You: " : "CPU " + std::to_string(i) + ": ");
        charInfo += characters[i];
        
        // Add skill status for human player
        if (i == 0 && gameEngine.canUseSkill(0)) {
            charInfo += " (Ready)";
        } else if (i == 0) {
            charInfo += " (Used)";
        }
        
        std::cout << charInfo;
    }
}

void UnoUI::drawSkillStatus() {
    if (gameEngine.getCurrentPlayerIndex() == 0 && gameEngine.canUseSkill(0)) {
        std::vector<std::string> characters = gameEngine.getPlayerCharacters();
        std::string yourCharacter = characters[0];
        std::string skillInfo;
        
        if (yourCharacter == "Lucky Star") {
            skillInfo = "Lucky Star Skill: Draw 3 cards, pick 1 (Cooldown: 1 round)";
        }
        else if (yourCharacter == "Collector") {
            skillInfo = "Collector Skill: Steal from discard pile (Cooldown: 1 round)";
        }
        else if (yourCharacter == "Thief") {
            skillInfo = "Thief Skill: Swap cards with opponent (Cooldown: 1 round)";
        }
        else if (yourCharacter == "Defender") {
            skillInfo = "Defender Skill: Passive defense (Auto-activates)";
        }
        
        drawCenteredText(8, skillInfo);
    }
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
    
    // Get card counts from backend
    std::vector<int> cardCounts = gameEngine.getPlayerCardCounts();
    drawText(5, 11, "CPU 1: " + std::to_string(cardCounts[1]) + " cards");
    drawText(25, 11, "CPU 2: " + std::to_string(cardCounts[2]) + " cards"); 
    drawText(45, 11, "CPU 3: " + std::to_string(cardCounts[3]) + " cards");
    std::cout << std::endl;
    
    drawCenteredText(13, "====================");
    std::cout << std::endl;
}

void UnoUI::drawMiddlePanel() {
    int width = getConsoleWidth();
    
    drawCenteredText(9, "CURRENT CARD");
    drawCenteredText(10, "============");
    std::cout << std::endl;
    
    // Get current card from backend
    Card currentCard = gameEngine.getCurrentCard();
    setCursorPosition(width/2 - 15, 12);
    
    // Show current play color if the card is wild
    if (currentCard.getColorString() == "WILD" || 
        currentCard.getColorString() == "WILD_DRAW4" || 
        currentCard.getColorString() == "FLASH") {
        std::cout << "Current Card: " << currentCard.getColorString() 
                  << " (Color: " << getColorString(gameEngine.getCurrentPlayColor()) << ")";
    } else {
        std::cout << "Current Card: " << currentCard.getColorString() << " " << currentCard.getValueString();
    }
    std::cout << std::endl;
    
    // Render the card using your renderer
    setCursorPosition(width/2 - 15, 13);
    
    if (currentCard.getColorString() != "UNKNOWN") {
        cardRenderer.render_card(
            currentCard.getColorString(),
            currentCard.getCardType(),
            currentCard.getCardNumber()
        );
    } else {
        std::cout << "No card played yet";
    }
    
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
    
    // Get hand from backend
    std::vector<Card> playerHand = gameEngine.getPlayerHand(0);
    
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
        Card card = playerHand[i];
        
        if (i == selectedCardIndex) {
            setCursorPosition(cardX + 5, 55);
            std::cout << ">>> SELECTED <<<" << std::endl;
        } else {
            setCursorPosition(cardX + 5, 55);
            std::cout << "                " << std::endl;
        }
        
        setCursorPosition(cardX, 56);
        std::cout << "Card " << (i + 1) << ": " << card.getColorString() << " " << card.getValueString();
        
        // Render the card
        setCursorPosition(cardX, 57);
        if (card.getColorString() != "UNKNOWN") {
            cardRenderer.render_card(
                card.getColorString(),
                card.getCardType(),
                card.getCardNumber()
            );
        } else {
            std::cout << "Invalid Card";
        }
        
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
    drawCenteredText(94, "CONTROLS:");
    
    // Only show skill button if skill is available
    if (gameEngine.getCurrentPlayerIndex() == 0 && gameEngine.canUseSkill(0)) {
        drawCenteredText(95, "A=Left  D=Right  W=Draw  S=Skill  M=Multiple");
    } else {
        drawCenteredText(95, "A=Left  D=Right  W=Draw  M=Multiple");
    }
    
    drawCenteredText(96, "ENTER=Play Single  Q=Quit");
    drawCenteredText(97, "================================");
}

void UnoUI::drawGameScreen() {
    if (needsRedraw) {
        clearScreen();
        
        drawTopPanel();
        drawMiddlePanel(); 
        drawPlayerHand();
        drawBottomPanel();
        
        setCursorPosition(0, 100);
        needsRedraw = false;
    }
}

CardColor UnoUI::chooseColor() {
    int width = getConsoleWidth();
    int choice = 0;
    bool colorChosen = false;
    std::vector<std::string> colors = {"RED", "BLUE", "GREEN", "YELLOW"};
    std::vector<std::string> colorCodes = {"\033[91m", "\033[94m", "\033[92m", "\033[93m"};
    const std::string RESET = "\033[0m";
    
    while (!colorChosen) {
        clearScreen();
        drawTopPanel();
        
        drawCenteredText(15, "CHOOSE A COLOR");
        drawCenteredText(16, "==============");
        std::cout << std::endl;
        
        for (int i = 0; i < 4; i++) {
            int x = width/2 - 20 + (i * 10);
            setCursorPosition(x, 18);
            if (i == choice) {
                std::cout << ">>> " << colorCodes[i] << colors[i] << RESET << " <<<";
            } else {
                std::cout << "    " << colorCodes[i] << colors[i] << RESET << "    ";
            }
        }
        
        std::cout << std::endl << std::endl;
        drawCenteredText(20, "Use A/D to choose, ENTER to confirm");
        
        char ch = _getch();
        switch (ch) {
            case 'a':
            case 'A':
                if (choice > 0) choice--;
                break;
            case 'd':
            case 'D':
                if (choice < 3) choice++;
                break;
            case 13: // Enter
                colorChosen = true;
                break;
        }
    }
    
    // Convert choice to CardColor
    switch (choice) {
        case 0: return CardColor::RED;
        case 1: return CardColor::BLUE;
        case 2: return CardColor::GREEN;
        case 3: return CardColor::YELLOW;
        default: return CardColor::RED;
    }
}

void UnoUI::handleMultipleCardPlay() {
    std::vector<Card> hand = gameEngine.getPlayerHand(0);
    if (hand.empty()) {
        message = "No cards in hand!";
        needsRedraw = true;
        return;
    }
    
    Card firstCard = hand[selectedCardIndex];
    std::vector<Card> cardsToPlay = {firstCard};
    
    // Find all cards with same value
    for (size_t i = 0; i < hand.size(); i++) {
        if (i != selectedCardIndex && hand[i].value == firstCard.value) {
            cardsToPlay.push_back(hand[i]);
        }
    }
    
    if (cardsToPlay.size() > 1) {
        if (gameEngine.canPlayMultipleCards(0, cardsToPlay)) {
            if (firstCard.isWildCard()) {
                // For wild cards in multiple play, choose color
                CardColor chosenColor = chooseColor();
                if (gameEngine.playMultipleCards(0, cardsToPlay, chosenColor)) {
                    message = "Played " + std::to_string(cardsToPlay.size()) + " " + 
                             firstCard.getValueString() + " cards! (Chose: " + getColorString(chosenColor) + ")";
                    gameEngine.nextTurn();
                    
                    // Reset selection
                    std::vector<Card> newHand = gameEngine.getPlayerHand(0);
                    if (selectedCardIndex >= newHand.size() && !newHand.empty()) {
                        selectedCardIndex = newHand.size() - 1;
                    }
                } else {
                    message = "Failed to play multiple cards!";
                }
            } else {
                // Normal multiple card play
                if (gameEngine.playMultipleCards(0, cardsToPlay)) {
                    message = "Played " + std::to_string(cardsToPlay.size()) + " " + 
                             firstCard.getValueString() + " cards!";
                    gameEngine.nextTurn();
                    
                    // Reset selection
                    std::vector<Card> newHand = gameEngine.getPlayerHand(0);
                    if (selectedCardIndex >= newHand.size() && !newHand.empty()) {
                        selectedCardIndex = newHand.size() - 1;
                    }
                } else {
                    message = "Failed to play multiple cards!";
                }
            }
        } else {
            message = "Cannot play multiple cards! Check if first card matches current card.";
        }
    } else {
        message = "No other cards with same value to play multiple!";
    }
    needsRedraw = true;
}

void UnoUI::handleInput() {
    if (_kbhit()) {
        char ch = _getch();
        
        // Only process input if it's the human player's turn
        if (gameEngine.getCurrentPlayerIndex() != 0) {
            // It's CPU's turn, let them play automatically
            return;
        }
        
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
                if (selectedCardIndex < gameEngine.getPlayerHand(0).size() - 1) {
                    selectedCardIndex++;
                    message = "Selected Card " + std::to_string(selectedCardIndex + 1);
                    needsRedraw = true;
                }
                break;
                
            case 'w':
            case 'W':
                {
                    Card drawnCard = gameEngine.drawCard(0);
                    message = "Drew: " + drawnCard.getColorString() + " " + drawnCard.getValueString();
                    gameEngine.nextTurn();
                    needsRedraw = true;
                }
                break;
                
            case 's':
            case 'S':
                if (gameEngine.canUseSkill(0)) {
                    gameEngine.useSkill(0);
                    message = "Used skill!";
                    needsRedraw = true;
                } else {
                    message = "Cannot use skill now!";
                    needsRedraw = true;
                }
                break;
                
            case 'm':
            case 'M': // Multiple card play
                if (gameEngine.getCurrentPlayerIndex() == 0) {
                    handleMultipleCardPlay();
                } else {
                    message = "Not your turn!";
                    needsRedraw = true;
                }
                break;
                
            case 13: // Enter - single card play
                {
                    std::vector<Card> hand = gameEngine.getPlayerHand(0);
                    if (!hand.empty() && selectedCardIndex < hand.size()) {
                        Card selectedCard = hand[selectedCardIndex];
                        if (gameEngine.canPlayCard(0, selectedCard)) {
                            if (selectedCard.isWildCard()) {
                                // For wild cards, let player choose color
                                CardColor chosenColor = chooseColor();
                                if (gameEngine.playCard(0, selectedCard, chosenColor)) {
                                    message = "Played: " + selectedCard.getColorString() + 
                                             " (Chose: " + getColorString(chosenColor) + ")";
                                    gameEngine.nextTurn();
                                    
                                    // Reset selection if hand size changed
                                    if (selectedCardIndex >= gameEngine.getPlayerHand(0).size() && 
                                        !gameEngine.getPlayerHand(0).empty()) {
                                        selectedCardIndex = gameEngine.getPlayerHand(0).size() - 1;
                                    }
                                } else {
                                    message = "Failed to play card!";
                                }
                            } else {
                                // For normal cards, play without color choice
                                if (gameEngine.playCard(0, selectedCard)) {
                                    message = "Played: " + selectedCard.getColorString() + " " + selectedCard.getValueString();
                                    gameEngine.nextTurn();
                                    
                                    // Reset selection if hand size changed
                                    if (selectedCardIndex >= gameEngine.getPlayerHand(0).size() && 
                                        !gameEngine.getPlayerHand(0).empty()) {
                                        selectedCardIndex = gameEngine.getPlayerHand(0).size() - 1;
                                    }
                                } else {
                                    message = "Failed to play card!";
                                }
                            }
                        } else {
                            message = "Cannot play that card! Current: " + 
                                     gameEngine.getCurrentCard().getColorString() + " " + 
                                     gameEngine.getCurrentCard().getValueString();
                        }
                        needsRedraw = true;
                    } else {
                        message = "No cards to play!";
                        needsRedraw = true;
                    }
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
    
    // Handle CPU turns automatically (even without key press)
    if (gameEngine.getCurrentPlayerIndex() != 0) {
        // Small delay to make CPU turns visible
        Sleep(1000);
        gameEngine.playCPUTurn();
        gameEngine.nextTurn();
        needsRedraw = true;
    }
}

bool UnoUI::isGameRunning() const {
    return gameRunning;
}