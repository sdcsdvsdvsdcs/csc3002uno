#include "player.h"
#include <algorithm>

Player::Player(int id, const std::string& playerName) 
    : playerId(id), name(playerName), character(CharacterType::LUCKY_STAR),
      skillAvailable(true), skillCooldown(false), skillUsesRemaining(0),
      defenderRevealed(false) {}

void Player::addCard(const Card& card) {
    hand.push_back(card);
}

bool Player::removeCard(const Card& card) {
    auto it = std::find(hand.begin(), hand.end(), card);
    if (it != hand.end()) {
        hand.erase(it);
        return true;
    }
    return false;
}

bool Player::hasCard(const Card& card) const {
    return std::find(hand.begin(), hand.end(), card) != hand.end();
}

void Player::setCharacter(CharacterType charType) {
    character = charType;
    skillAvailable = true;
    skillCooldown = false;
    defenderRevealed = false;
    
    switch(charType) {
        case CharacterType::LUCKY_STAR:
            skillUsesRemaining = 3;
            break;
        case CharacterType::COLLECTOR:
        case CharacterType::THIEF:
        case CharacterType::DEFENDER:
            skillUsesRemaining = 1;  // All characters get 1 skill use
            break;
        default:
            skillUsesRemaining = 0;
            break;
    }
}

std::string Player::getCharacterString() const {
    switch(character) {
        case CharacterType::LUCKY_STAR: return "Lucky Star";
        case CharacterType::COLLECTOR: return "Collector";
        case CharacterType::THIEF: return "Thief";
        case CharacterType::DEFENDER: return "Defender";
        default: return "Unknown";
    }
}

void Player::useSkill() {
    if (skillUsesRemaining > 0) {
        skillUsesRemaining--;
        skillCooldown = true;
        
        if (skillUsesRemaining <= 0) {
            skillAvailable = false;
        }
    }
}

void Player::resetSkillCooldown() {
    if (skillCooldown && skillUsesRemaining > 0) {
        skillCooldown = false;
        if (character != CharacterType::LUCKY_STAR) {
            skillAvailable = true;
        }
    }
}

std::vector<Card> Player::getCardsByColor(CardColor color) const {
    std::vector<Card> result;
    for (const auto& card : hand) {
        if (card.color == color) {
            result.push_back(card);
        }
    }
    return result;
}

bool Player::hasColorCard(CardColor color) const {
    for (const auto& card : hand) {
        if (card.color == color) {
            return true;
        }
    }
    return false;
}