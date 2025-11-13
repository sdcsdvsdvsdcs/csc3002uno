#include <ctime>
#include <algorithm>
#include <iostream>

#include "game_board.h"

namespace UNO { namespace Game {

GameBoard::GameBoard(std::shared_ptr<Network::IServer> serverSp)
    : mServer(serverSp), 
    mDiscardPile(std::make_unique<DiscardPile>()),
    mDeck(std::make_unique<Deck>(*mDiscardPile))
{
    mServer->RegisterReceiveJoinGameInfoCallback(
        [this](int index, const JoinGameInfo &info) {
            ReceiveUsername(index, info.mUsername);
        }
    );
    mServer->RegisterAllPlayersJoinedCallback([this] { StartGame(); });
}

void GameBoard::Start()
{
    mServer->Run();
}

std::shared_ptr<Network::IServer> GameBoard::CreateServer(const std::string &port)
{
    return std::make_shared<Network::Server>(port);
}

void GameBoard::ResetGame()
{
    mServer->Reset();
    mPlayerStats.clear();
    mIsFlashEffectActive = false;
    mFlashCardsPlayed = 0;
    mPlayersAffectedByFlash.clear();
    mSkillUsedThisTurn.clear();
    mIsPackageEffectActive = false;  // 新增：重置Package效果状态
    mPackagePlayerIndex = -1;
}

void GameBoard::ReceiveUsername(int index, const std::string &username)
{
    std::cout << "receive, index: " << index << ", username: " << username << std::endl;
    mPlayerStats.emplace_back(username, 7);
    std::vector<std::string> tmpUsernames;
    std::for_each(mPlayerStats.begin(), mPlayerStats.end(),
        [&tmpUsernames](const PlayerStat &stat) {
            tmpUsernames.push_back(stat.GetUsername());
        }
    );
    Common::Util::Deliver<JoinGameRspInfo>(mServer, index, Common::Common::mPlayerNum, tmpUsernames);
    for (int i = 0; i < index; i++) {
        Common::Util::Deliver<JoinGameInfo>(mServer, i, username);
    }
}

void GameBoard::StartGame()
{
#ifdef ENABLE_LOG
    spdlog::info("Game Starts.");
#endif
    mDeck->Init();
    std::vector<std::array<Card, 7>> initHandCards = 
        mDeck->DealInitHandCards(Common::Common::mPlayerNum);

    // Assign random characters to each player
    for (auto& playerStat : mPlayerStats) {
        auto characterType = CharacterFactory::GetRandomCharacter();
        auto character = CharacterFactory::CreateCharacter(characterType);
        playerStat.AssignCharacter(std::move(character));
        std::cout << "Player " << playerStat.GetUsername() 
                  << " assigned character: " << playerStat.GetCharacterName() << std::endl;
    }

    // flip a card
    Card flippedCard;
    while (true) {
        flippedCard = mDeck->Draw();
        if (flippedCard.mColor == CardColor::BLACK) {
            // if the flipped card is a wild card, put it to under the deck and flip a new one
            mDeck->PutToBottom(flippedCard);
        }
        else {
            if (CardSet::DrawTexts.count(flippedCard.mText)) {
                // last played card will become EMPTY if the flipped card is `Draw` card
                flippedCard.mText = CardText::EMPTY;
            }
            break;
        }
    }

    // choose the first player randomly
    std::srand(std::time(nullptr));
    int firstPlayer = std::rand() % Common::Common::mPlayerNum;

    std::vector<std::string> tmpUsernames;
    std::for_each(mPlayerStats.begin(), mPlayerStats.end(),
        [&tmpUsernames](const PlayerStat &stat) {
            tmpUsernames.push_back(stat.GetUsername());
        }
    );
    for (int player = 0; player < Common::Common::mPlayerNum; player++) {
        Common::Util::Deliver<GameStartInfo>(mServer, player, initHandCards[player], flippedCard,
            Common::Util::WrapWithPlayerNum(firstPlayer - player), tmpUsernames);

        std::rotate(tmpUsernames.begin(), tmpUsernames.begin() + 1, tmpUsernames.end());
    }

    mGameStat.reset(new GameStat(firstPlayer, flippedCard));
    
    // Initialize skill usage tracking
    for (int i = 0; i < Common::Common::mPlayerNum; i++) {
        mSkillUsedThisTurn[i] = false;
    }

    GameLoop();
}

void GameBoard::GameLoop()
{
    while (!mGameStat->DoesGameEnd()) {
        try {
            int currentPlayer = mGameStat->GetCurrentPlayer();
            
            // 新增：检查特殊效果状态
            if (mGameStat->IsSpecialEffectActive()) {
                std::cout << "Special effect active, handling special phase" << std::endl;
                // 处理特殊效果阶段（如Flash卡效果）
            }
            
            // Start of turn phase
            mGameStat->SetCurrentPhase(GameStat::TurnPhase::START);
            std::cout << "Player " << currentPlayer << "'s turn - START phase" << std::endl;
            
            // Reset skill usage for this turn
            mSkillUsedThisTurn[currentPlayer] = false;

            // Skill usage phase
            mGameStat->SetCurrentPhase(GameStat::TurnPhase::SKILL);
            std::cout << "Player " << currentPlayer << "'s turn - SKILL phase" << std::endl;
            HandleSkillPhase();

            // Card play phase
            mGameStat->SetCurrentPhase(GameStat::TurnPhase::CARD_PLAY);
            std::cout << "Player " << currentPlayer << "'s turn - CARD_PLAY phase" << std::endl;
            
            auto actionInfo = Common::Util::Receive<ActionInfo>(mServer, currentPlayer);
            switch (actionInfo->mActionType) {
                case ActionType::DRAW:
                    HandleDraw(Common::Util::DynamicCast<DrawInfo>(actionInfo));
                    break;
                case ActionType::SKIP:
                    HandleSkip(Common::Util::DynamicCast<SkipInfo>(actionInfo));
                    break;
                case ActionType::PLAY:
                    HandlePlay(Common::Util::DynamicCast<PlayInfo>(actionInfo));
                    break;
                default:
                    assert(0);
            }

            // End of turn phase
            mGameStat->SetCurrentPhase(GameStat::TurnPhase::END);
            std::cout << "Player " << currentPlayer << "'s turn - END phase" << std::endl;
            HandleTurnEnd();

        }
        catch (const std::exception &e) {
            /// TODO: handle the condition that someone has disconnected
            std::cout << "someone has disconnected, shutdown server" << std::endl;
            std::exit(-1);
        }
    }
    ResetGame();
}

void GameBoard::HandleSkillPhase()
{
    int currentPlayer = mGameStat->GetCurrentPlayer();
    PlayerStat& currentStat = mPlayerStats[currentPlayer];
    
    if (currentStat.HasCharacter() && currentStat.CanUseSkill() && !mSkillUsedThisTurn[currentPlayer]) {
        // In a real implementation, we would send a message to the client
        // asking if they want to use their skill, and receive their decision
        std::cout << "Player " << currentPlayer << " can use skill: " 
                  << currentStat.GetCharacterName() << std::endl;
        
        // For demonstration, we'll auto-use skills in certain conditions
        // In a real game, this would be player's choice
        if (currentStat.GetCharacterType() == CharacterType::COLLECTOR && 
            !mDiscardPile->GetPile().empty()) {
            // Auto-use Collector skill if discard pile is not empty
            ProcessCollectorSkill(currentPlayer);
            mSkillUsedThisTurn[currentPlayer] = true;
        }
        // 新增：其他角色的技能触发条件
        else if (currentStat.GetCharacterType() == CharacterType::LUCKY_STAR) {
            // Lucky Star 技能在抽牌阶段处理
            std::cout << "Lucky Star skill available for draw phase" << std::endl;
        }
    }
}

void GameBoard::HandleTurnEnd()
{
    int currentPlayer = mGameStat->GetCurrentPlayer();
    
    // Update character cooldowns
    for (auto& playerStat : mPlayerStats) {
        playerStat.UpdateCharacterCooldown();
    }
    
    // Reset Flash effect if it was active
    if (mIsFlashEffectActive) {
        mIsFlashEffectActive = false;
        mFlashCardsPlayed = 0;
        mPlayersAffectedByFlash.clear();
        mGameStat->SetSpecialEffectActive(false);  // 新增：更新游戏状态
    }
    
    // 新增：重置Package效果状态
    if (mIsPackageEffectActive) {
        mIsPackageEffectActive = false;
        mPackagePlayerIndex = -1;
    }
    
    std::cout << "Turn ended for player " << currentPlayer << std::endl;
}

void GameBoard::HandleDraw(const std::unique_ptr<DrawInfo> &info)
{
    std::cout << *info << std::endl;

    // Check for Lucky Star skill during draw phase
    int currentPlayer = mGameStat->GetCurrentPlayer();
    PlayerStat& currentStat = mPlayerStats[currentPlayer];
    
    if (currentStat.HasCharacter() && 
        currentStat.GetCharacterType() == CharacterType::LUCKY_STAR &&
        currentStat.CanUseSkill() && !mSkillUsedThisTurn[currentPlayer]) {
        ProcessLuckyStarSkill(currentPlayer);
        mSkillUsedThisTurn[currentPlayer] = true;
    } else {
        // Normal draw
        std::vector<Card> cardsToDraw = mDeck->Draw(info->mNumber);
        Common::Util::Deliver<DrawRspInfo>(mServer, currentPlayer, info->mNumber, cardsToDraw);
    }

    // Broadcast to other players
    Broadcast<DrawInfo>(*info);

    // update stat
    mPlayerStats[currentPlayer].UpdateAfterDraw(info->mNumber);
    mGameStat->UpdateAfterDraw();
}

void GameBoard::HandleSkip(const std::unique_ptr<SkipInfo> &info)
{
    std::cout << *info << std::endl;

    // broadcast to other players
    Broadcast<SkipInfo>(*info);

    // update stat
    mPlayerStats[mGameStat->GetCurrentPlayer()].UpdateAfterSkip();
    mGameStat->UpdateAfterSkip();
}

void GameBoard::HandlePlay(const std::unique_ptr<PlayInfo> &info)
{
    std::cout << *info << std::endl;
    
    // 新增：验证Wild Draw Four出牌条件
    if (info->mCard.mText == CardText::DRAW_FOUR && info->mCard.mColor == CardColor::BLACK) {
        if (!CanPlayWildDrawFour(mGameStat->GetCurrentPlayer())) {
            std::cout << "Invalid Wild Draw Four play - player has matching color cards" << std::endl;
            // 在实际实现中，这里应该拒绝出牌并让玩家重新选择
            return;
        }
    }
    
    mDiscardPile->Add(info->mCard);
    
    // Handle special card effects
    HandleSpecialCardEffects(info->mCard);

    if (info->mCard.mColor == CardColor::BLACK) {
        // change the color to the specified next color to show in UI
        info->mCard.mColor = info->mNextColor;
    }

    // broadcast to other players
    Broadcast<PlayInfo>(*info);

    // update stat
    PlayerStat &stat = mPlayerStats[mGameStat->GetCurrentPlayer()];
    stat.UpdateAfterPlay(info->mCard);
    if (stat.GetRemainingHandCardsNum() == 0) {
        Win();
    }
    mGameStat->UpdateAfterPlay(info->mCard);
}

void GameBoard::HandleSpecialCardEffects(const Card& card)
{
    int currentPlayer = mGameStat->GetCurrentPlayer();
    
    switch (card.mText) {
        case CardText::PACKAGE:
            std::cout << "Package Card played by player " << currentPlayer << std::endl;
            // 设置Package效果状态
            mIsPackageEffectActive = true;
            mPackagePlayerIndex = currentPlayer;
            mPackageTargetColor = card.mColor; // 使用卡牌颜色作为默认目标颜色
            mGameStat->SetSpecialEffectActive(true);
            // In real implementation, ask player to choose a color
            // For now, auto-choose the card's color
            HandlePackageCardEffect(currentPlayer, card.mColor);
            break;
            
        case CardText::FLASH:
            std::cout << "Flash Card played by player " << currentPlayer << std::endl;
            mGameStat->SetSpecialEffectActive(true);
            // In real implementation, ask player to choose a color
            // For now, auto-choose the card's color
            HandleFlashCardEffect(currentPlayer, card.mColor);
            break;
            
        default:
            break;
    }
}

void GameBoard::HandlePackageCardEffect(int playerIndex, CardColor chosenColor)
{
    std::cout << "Package Card effect: Player " << playerIndex 
              << " can discard all " << chosenColor << " number cards" << std::endl;
    
    // In a real implementation, we would:
    // 1. Send a message to the client to choose which color to discard
    // 2. Receive their choice
    // 3. Remove all number cards of that color from their hand
    // 4. Add those cards to the discard pile
    
    // For now, just log the effect
    PlayerStat& playerStat = mPlayerStats[playerIndex];
    std::cout << "Player " << playerIndex << " would discard all " 
              << chosenColor << " number cards from their hand" << std::endl;
    std::cout << "Player has " << playerStat.GetHandCardCount() << " cards in hand" << std::endl;
}

void GameBoard::HandleFlashCardEffect(int playerIndex, CardColor chosenColor)
{
    std::cout << "Flash Card effect activated! Color: " << chosenColor << std::endl;
    
    mIsFlashEffectActive = true;
    mFlashEffectColor = chosenColor;
    mFlashCardsPlayed = 0;
    mPlayersAffectedByFlash.clear();
    mGameStat->SetSpecialEffectActive(true);
    
    int currentPlayer = playerIndex;
    
    // Process Flash effect for other players in sequence
    for (int i = 1; i < Common::Common::mPlayerNum; i++) {
        int targetPlayer = (currentPlayer + i) % Common::Common::mPlayerNum;
        
        // Check if target player has the specified color card
        // In real implementation, we would check their actual hand
        bool hasColorCard = false; // This would be determined by actual hand checking
        
        if (!hasColorCard) {
            // Player must draw cards equal to number of Flash cards already played
            int cardsToDraw = mFlashCardsPlayed;
            if (cardsToDraw > 0) {
                std::vector<Card> drawnCards = mDeck->Draw(cardsToDraw);
                std::cout << "Player " << targetPlayer << " draws " << cardsToDraw 
                          << " cards due to Flash effect" << std::endl;
                mPlayersAffectedByFlash.push_back(targetPlayer);
                
                // Send draw response to affected player
                Common::Util::Deliver<DrawRspInfo>(mServer, targetPlayer, cardsToDraw, drawnCards);
                // 更新受影响玩家的状态
                mPlayerStats[targetPlayer].UpdateAfterDraw(cardsToDraw);
            }
        } else {
            mFlashCardsPlayed++;
        }
    }
    
    std::cout << "Flash effect completed. " << mFlashCardsPlayed 
              << " cards played, " << mPlayersAffectedByFlash.size() 
              << " players affected" << std::endl;
}

// 以下技能处理方法保持不变（为了完整性保留）
void GameBoard::HandleCharacterSkill(int playerIndex, CharacterType skillType, int targetPlayer, CardText cardType)
{
    switch (skillType) {
        case CharacterType::LUCKY_STAR:
            ProcessLuckyStarSkill(playerIndex);
            break;
        case CharacterType::COLLECTOR:
            ProcessCollectorSkill(playerIndex);
            break;
        case CharacterType::THIEF:
            if (targetPlayer != -1) {
                ProcessThiefSkill(playerIndex, targetPlayer, cardType);
            }
            break;
        default:
            break;
    }
}

void GameBoard::ProcessLuckyStarSkill(int playerIndex)
{
    std::cout << "Lucky Star skill used by player " << playerIndex << std::endl;
    
    // View top 3 cards of the deck
    std::vector<Card> topCards;
    for (int i = 0; i < 3 && !mDeck->Empty(); i++) {
        topCards.push_back(mDeck->Draw());
    }
    
    // In real implementation, send these cards to the client to choose one
    // For now, auto-choose the first card
    Card chosenCard = topCards.empty() ? Card() : topCards[0];
    
    // Return the other two cards to the deck in original order
    for (int i = topCards.size() - 1; i >= 0; i--) {
        if (i != 0) { // Skip the chosen card
            mDeck->PutToBottom(topCards[i]);
        }
    }
    
    // Give the chosen card to the player
    if (!topCards.empty()) {
        std::vector<Card> chosenCards = {chosenCard};
        Common::Util::Deliver<DrawRspInfo>(mServer, playerIndex, 1, chosenCards);
        mPlayerStats[playerIndex].UpdateAfterDraw(1);
    }
    
    // Mark skill as used
    mPlayerStats[playerIndex].UseSkill();
}

void GameBoard::ProcessCollectorSkill(int playerIndex)
{
    std::cout << "Collector skill used by player " << playerIndex << std::endl;
    
    auto discardPile = mDiscardPile->GetPile();
    if (discardPile.empty()) {
        std::cout << "Discard pile is empty, skill fails" << std::endl;
        return;
    }
    
    // In real implementation, send discard pile to client to choose a card
    // For now, auto-choose the top card (excluding the very top if it's the current play)
    Card chosenCard = discardPile.size() > 1 ? discardPile[1] : discardPile[0];
    
    std::cout << "Player " << playerIndex << " collects card: " << chosenCard << std::endl;
    
    // Give the card to the player (in real implementation, remove from discard pile)
    std::vector<Card> collectedCards = {chosenCard};
    Common::Util::Deliver<DrawRspInfo>(mServer, playerIndex, 1, collectedCards);
    mPlayerStats[playerIndex].UpdateAfterDraw(1);
    
    // Mark skill as used
    mPlayerStats[playerIndex].UseSkill();
}

void GameBoard::ProcessThiefSkill(int playerIndex, int targetPlayer, CardText cardType)
{
    std::cout << "Thief skill used by player " << playerIndex 
              << " on player " << targetPlayer << " for card type: " << static_cast<int>(cardType) << std::endl;
    
    // Check if target has Defender and try to defend
    if (ProcessDefenderSkill(targetPlayer)) {
        std::cout << "Defender skill activated! Thief skill blocked." << std::endl;
        return;
    }
    
    // In real implementation:
    // 1. Check if target player has the specified card type
    // 2. If yes, randomly steal one card of that type
    // 3. Give one card from thief to target player
    
    std::cout << "Thief skill executed successfully" << std::endl;
    
    // Mark skill as used
    mPlayerStats[playerIndex].UseSkill();
}

bool GameBoard::ProcessDefenderSkill(int targetPlayer)
{
    PlayerStat& targetStat = mPlayerStats[targetPlayer];
    if (targetStat.HasCharacter() && 
        targetStat.GetCharacterType() == CharacterType::DEFENDER) {
        
        auto defender = dynamic_cast<Defender*>(targetStat.GetCharacter());
        if (defender && defender->TryDefend()) {
            targetStat.UseSkill(); // Mark skill as used for cooldown
            return true;
        }
    }
    return false;
}

void GameBoard::Win()
{
    mGameStat->GameEnds();
#ifdef ENABLE_LOG
    spdlog::info("Game Ends.");
#endif
    
    int winnerIndex = mGameStat->GetCurrentPlayer();
    std::cout << "Player " << winnerIndex << " (" << mPlayerStats[winnerIndex].GetUsername() 
              << ") wins the game!" << std::endl;
    std::cout << "Character: " << mPlayerStats[winnerIndex].GetCharacterName() << std::endl;
}

void GameBoard::BroadcastGameStateUpdate()
{
    // In real implementation, send game state update to all players
    // including current phase, active effects, etc.
}

bool GameBoard::CanPlayCard(int playerIndex, const Card& card) const
{
    Card lastPlayedCard = mGameStat->GetLastPlayedCard();
    bool isUno = (mPlayerStats[playerIndex].GetRemainingHandCardsNum() == 1);
    return card.CanBePlayedAfter(lastPlayedCard, isUno);
}

std::vector<Card> GameBoard::GetPlayableCards(int playerIndex) const
{
    std::vector<Card> playableCards;
    // In real implementation, this would check the player's actual hand
    // against the current game state and rules
    return playableCards;
}

// 新增：Wild Draw Four出牌条件验证
bool GameBoard::CanPlayWildDrawFour(int playerIndex) const
{
    // 在实际实现中，这里应该检查玩家手牌中是否有匹配当前颜色的牌
    // 如果没有匹配的牌，才能出Wild Draw Four
    
    CardColor currentColor = mGameStat->GetLastPlayedCard().mColor;
    
    // 这里应该是实际的玩家手牌检查逻辑
    // 目前返回true作为占位符
    return true;
}

}}
