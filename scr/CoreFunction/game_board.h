#pragma once

#include <memory>
#include <random>
#include <deque>
#include <cstdlib>
#include <map>

#include "stat.h"
#include "cards.h"
#include "../network/server.h"

namespace UNO { namespace Game {

using namespace Network;

class GameBoard {
public:
    explicit GameBoard(std::shared_ptr<Network::IServer> serverSp);

    void Start();

    static std::shared_ptr<Network::IServer> CreateServer(const std::string &port);

private:
    /**
     * Callback of receiving a \c JoinGameInfo from a player.
     *   \param index: the index of the player
     *   \param username: the player's username
     */
    void ReceiveUsername(int index, const std::string &username);

    /**
     * Have received all players' info, start game.
     */
    void StartGame();

    /**
     * Main game loop, wait for \c ActionInfo from players and act accordingly.
     */
    void GameLoop();
    
    /**
     * Handle a \c DrawInfo from player.
     */
    void HandleDraw(const std::unique_ptr<DrawInfo> &info);
    
    /**
     * Handle a \c SkipInfo from player.
     */
    void HandleSkip(const std::unique_ptr<SkipInfo> &info);
    
    /**
     * Handle a \c PlayInfo from player.
     */
    void HandlePlay(const std::unique_ptr<PlayInfo> &info);

    /**
     * Handle skill usage phase.
     */
    void HandleSkillPhase();

    /**
     * Handle turn end phase.
     */
    void HandleTurnEnd();

    /**
     * Handle special card effects.
     */
    void HandleSpecialCardEffects(const Card& card);

    /**
     * Handle Package Card effect.
     */
    void HandlePackageCardEffect(int playerIndex, CardColor chosenColor);

    /**
     * Handle Flash Card effect.
     */
    void HandleFlashCardEffect(int playerIndex, CardColor chosenColor);

    /**
     * Handle character skills.
     */
    void HandleCharacterSkill(int playerIndex, CharacterType skillType, int targetPlayer = -1, CardText cardType = CardText::EMPTY);

    /**
     * Process Lucky Star skill.
     */
    void ProcessLuckyStarSkill(int playerIndex);

    /**
     * Process Collector skill.
     */
    void ProcessCollectorSkill(int playerIndex);

    /**
     * Process Thief skill.
     */
    void ProcessThiefSkill(int playerIndex, int targetPlayer, CardText cardType);

    /**
     * Process Defender skill.
     */
    bool ProcessDefenderSkill(int targetPlayer);

    /**
     * Someone has won, end game.
     */
    void Win();

    /**
     * Reset the game state and prepare for restart.
     */
    void ResetGame();

    /**
     * Broadcast info to players other than the current one.
     */
    template <typename ActionInfoT>
    void Broadcast(ActionInfoT &info) {
        int currentPlayer = mGameStat->GetCurrentPlayer();
        for (int i = 0; i < Common::Common::mPlayerNum; i++) {
            if (i != currentPlayer) {
                info.mPlayerIndex = Common::Util::WrapWithPlayerNum(currentPlayer - i);
                mServer->DeliverInfo(&typeid(ActionInfoT), i, info);
            }
        }
    }

    /**
     * Send game state update to all players.
     */
    void BroadcastGameStateUpdate();

    /**
     * Check if a player can play a card.
     */
    bool CanPlayCard(int playerIndex, const Card& card) const;

    /**
     * Get playable cards for a player.
     */
    std::vector<Card> GetPlayableCards(int playerIndex) const;

    /**
     * Check if Wild Draw Four card can be played by player.
     */
    bool CanPlayWildDrawFour(int playerIndex) const;

public:
    // for tests
    const std::unique_ptr<DiscardPile> &GetDiscardPile() const { return mDiscardPile; }

    const std::unique_ptr<Deck> &GetDeck() const { return mDeck; }

    const std::unique_ptr<GameStat> &GetGameStat() const { return mGameStat; }

    const std::vector<PlayerStat> &GetPlayerStats() const { return mPlayerStats; }

private:
    std::shared_ptr<Network::IServer> mServer;

    // state of game board
    std::unique_ptr<DiscardPile> mDiscardPile;
    std::unique_ptr<Deck> mDeck;
    std::unique_ptr<GameStat> mGameStat;

    // state of all players
    std::vector<PlayerStat> mPlayerStats;

    // Game state variables for special effects
    bool mIsFlashEffectActive{false};
    CardColor mFlashEffectColor{CardColor::RED};
    int mFlashCardsPlayed{0};
    std::vector<int> mPlayersAffectedByFlash;

    // Skill usage tracking
    std::map<int, bool> mSkillUsedThisTurn;
    
    // 新增：Package Card效果状态
    bool mIsPackageEffectActive{false};
    int mPackagePlayerIndex{-1};
    CardColor mPackageTargetColor{CardColor::RED};
};
}}
