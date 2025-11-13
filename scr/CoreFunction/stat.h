// 在 PlayerStat 类中添加角色相关字段
class PlayerStat {
public:
    PlayerStat() {}
    explicit PlayerStat(const std::string& username, int remainingHandCardsNum);

    // 添加角色相关方法
    void AssignCharacter(std::unique_ptr<Character> character);
    Character* GetCharacter() const { return mCharacter.get(); }
    bool HasCharacter() const { return mCharacter != nullptr; }

    // 其他现有方法保持不变...
    void UpdateAfterDraw(int number, int indexOfNewlyDrawn = -1);
    
    void UpdateAfterSkip();
    
    void UpdateAfterPlay(Card card);

    std::string GetUsername() const { return mUsername; }

    int GetRemainingHandCardsNum() const { return mRemainingHandCardsNum;  }

    bool DoPlayInLastRound() const { return mDoPlayInLastRound; }

    Card GetLastPlayedCard() const { return mLastPlayedCard; }

    bool HasChanceToPlayAfterDraw() const { return mHasChanceToPlayAfterDraw; }

    int GetIndexOfNewlyDrawn() const { return mIndexOfNewlyDrawn; }

    // for test
    void SetLastPlayedCard(Card lastPlayedCard) { mLastPlayedCard = lastPlayedCard; }
    
private:
    const std::string mUsername;
    std::unique_ptr<Character> mCharacter;  // 角色指针
    
    // 其他现有字段...
    int mRemainingHandCardsNum;
    bool mDoPlayInLastRound{false};
    Card mLastPlayedCard{};
    bool mHasChanceToPlayAfterDraw{false};
    int mIndexOfNewlyDrawn{-1};

    friend std::ostream& operator<<(std::ostream& os, const PlayerStat& stat);

};

// 在 GameStat 中添加回合阶段管理
class GameStat {
public:
    enum class TurnPhase {
        START,       // 回合开始阶段
        SKILL,       // 技能使用阶段  
        CARD_PLAY,   // 出牌阶段
        END          // 回合结束阶段
    };

    /// constructor for \c Player
    GameStat(const GameStartInfo &info);

    /// constructor for \c GameBoard
    GameStat(int firstPlayer, Card flippedCard);
    
    void NextPlayer();

    void UpdateAfterDraw();

    void UpdateAfterSkip();

    void UpdateAfterPlay(Card card);

    void Tick();

    bool IsMyTurn() const { return mCurrentPlayer == 0; }

    bool IsSkipped() const { return mLastPlayedCard.mText == CardText::SKIP; }

    int GetCurrentPlayer() const { return mCurrentPlayer; }

    bool IsInClockwise() const { return mIsInClockwise; }

    bool DoesGameEnd() const { return mGameEnds; }

    int GetTimeElapsed() const { return mTimeElapsed; }

    Card GetLastPlayedCard() const { return mLastPlayedCard; }

    int GetCardsNumToDraw() const { return mCardsNumToDraw; }

    void GameEnds() { mGameEnds = true; mCurrentPlayer = -1; }

    void Reverse() { mIsInClockwise = !mIsInClockwise; }

    // for tests
    void SetCurrentPlayer(int currentPlayer) { mCurrentPlayer = currentPlayer; }
    
    void SetIsInClockwise(bool isInClockwise) { mIsInClockwise = isInClockwise; }

    void SetLastPlayedCard(Card lastPlayedCard) { mLastPlayedCard = lastPlayedCard; }

    void SetCardsNumToDraw(int cardsNumToDraw) { mCardsNumToDraw = cardsNumToDraw; }


    TurnPhase GetCurrentPhase() const { return mCurrentPhase; }
    void SetCurrentPhase(TurnPhase phase) { mCurrentPhase = phase; }
    void AdvancePhase();

private:
    TurnPhase mCurrentPhase{TurnPhase::START};

    int mCurrentPlayer;
    bool mIsInClockwise;
    bool mGameEnds{false};
    int mTimeElapsed{0};

    // currently the two fields below are not used by GameStat of GameBoard
    Card mLastPlayedCard{};
    int mCardsNumToDraw{1};  // +2 and +4 can accumulate
};
