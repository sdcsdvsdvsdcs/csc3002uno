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
    
private:
    const std::string mUsername;
    std::unique_ptr<Character> mCharacter;  // 角色指针
    
    // 其他现有字段...
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

    TurnPhase GetCurrentPhase() const { return mCurrentPhase; }
    void SetCurrentPhase(TurnPhase phase) { mCurrentPhase = phase; }
    void AdvancePhase();

private:
    TurnPhase mCurrentPhase{TurnPhase::START};
};
