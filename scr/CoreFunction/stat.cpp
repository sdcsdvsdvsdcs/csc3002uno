#include "stat.h"
#include <random>

namespace UNO { namespace Game {

// GameStat 实现
GameStat::GameStat(const GameStartInfo &info)
    : mCurrentPlayer(info.mFirstPlayer), 
    mIsInClockwise(info.mFlippedCard.mText != CardText::REVERSE),
    mLastPlayedCard(info.mFlippedCard) {}

GameStat::GameStat(int firstPlayer, Card flippedCard)
    : mCurrentPlayer(firstPlayer),
    mIsInClockwise(flippedCard.mText != CardText::REVERSE) {}

void GameStat::NextPlayer()
{
    mCurrentPlayer = mIsInClockwise ? 
        Common::Util::WrapWithPlayerNum(mCurrentPlayer + 1) :
        Common::Util::WrapWithPlayerNum(mCurrentPlayer - 1);
    mTimeElapsed = 0;
    mCurrentPhase = TurnPhase::START; // 新回合开始
}

void GameStat::UpdateAfterDraw()
{
    if (CardSet::DrawTexts.count(mLastPlayedCard.mText)) {
        // last played card will become EMPTY after the draw penalty is consumed
        mLastPlayedCard.mText = CardText::EMPTY;
    }
    // the number of cards to draw falls back to 1
    mCardsNumToDraw = 1;

    // no need to invoke NextPlayer() here 
    // because a draw action is always followed by a skip or play action
}

void GameStat::UpdateAfterSkip()
{
    if (mLastPlayedCard.mText == CardText::SKIP) {
        // last played card will become EMPTY after the skip penalty is consumed
        mLastPlayedCard.mText = CardText::EMPTY;
    }
    NextPlayer();
}

void GameStat::UpdateAfterPlay(Card card)
{
    if (card.mText == CardText::WILD) {
        // if just a common wild card (not +4), don't affect the number text
        mLastPlayedCard.mColor = card.mColor;
    }
    else {
        mLastPlayedCard = card;
    }
    
    if (card.mText == CardText::REVERSE) {
        mIsInClockwise = !mIsInClockwise;
    }
    if (card.mText == CardText::DRAW_TWO) {
        // in the normal state, mCardsNumToDraw is equal to 1
        // once a player plays a `Draw` card, the effect is gonna accumulate
        mCardsNumToDraw = (mCardsNumToDraw == 1) ? 2 : (mCardsNumToDraw + 2);
    }
    if (card.mText == CardText::DRAW_FOUR) {
        mCardsNumToDraw = (mCardsNumToDraw == 1) ? 4 : (mCardsNumToDraw + 4);
    }

    if (!mGameEnds) {
        NextPlayer();
    }
}

void GameStat::Tick()
{
    mTimeElapsed++;
}

void GameStat::AdvancePhase()
{
    switch (mCurrentPhase) {
        case TurnPhase::START:
            mCurrentPhase = TurnPhase::SKILL;
            break;
        case TurnPhase::SKILL:
            mCurrentPhase = TurnPhase::CARD_PLAY;
            break;
        case TurnPhase::CARD_PLAY:
            mCurrentPhase = TurnPhase::END;
            break;
        case TurnPhase::END:
            mCurrentPhase = TurnPhase::START;
            break;
    }
}

// PlayerStat 实现
PlayerStat::PlayerStat(const std::string &username, int remainingHandCardsNum)
    : mUsername(username), mRemainingHandCardsNum(remainingHandCardsNum) {}

void PlayerStat::UpdateAfterDraw(int number, int indexOfNewlyDrawn)
{
    mRemainingHandCardsNum += number;
    mDoPlayInLastRound = false;
    // only common draw (rather than draw penalty due to +2 / +4) 
    // has the chance to play the card just drawn immediately
    mHasChanceToPlayAfterDraw = (number == 1);
    mIndexOfNewlyDrawn = indexOfNewlyDrawn;
}

void PlayerStat::UpdateAfterSkip()
{
    mDoPlayInLastRound = false;
    mHasChanceToPlayAfterDraw = false;
}

void PlayerStat::UpdateAfterPlay(Card card)
{
    mRemainingHandCardsNum--;
    mDoPlayInLastRound = true;
    mLastPlayedCard = card;
    mHasChanceToPlayAfterDraw = false;
}

void PlayerStat::AssignCharacter(std::unique_ptr<Character> character)
{
    mCharacter = std::move(character);
}

CharacterType PlayerStat::GetCharacterType() const
{
    return mCharacter ? mCharacter->GetType() : CharacterType::NONE;
}

std::string PlayerStat::GetCharacterName() const
{
    return mCharacter ? mCharacter->GetName() : "None";
}

bool PlayerStat::CanUseSkill() const
{
    return mCharacter && mCharacter->CanUseSkill();
}

void PlayerStat::UseSkill()
{
    if (mCharacter) {
        mCharacter->UseSkill();
    }
}

void PlayerStat::UpdateCharacterCooldown()
{
    if (mCharacter) {
        mCharacter->UpdateCooldown();
    }
}

void PlayerStat::ResetCharacterForNewRound()
{
    if (mCharacter) {
        mCharacter->ResetForNewRound();
    }
}

std::ostream& operator<<(std::ostream& os, const PlayerStat& stat)
{
    os << "\t   { " << stat.mUsername << ", " << stat.mRemainingHandCardsNum;
    if (stat.mDoPlayInLastRound) {
        os << ", " << stat.mLastPlayedCard;
    }
    if (stat.HasCharacter()) {
        os << ", Character: " << stat.GetCharacterName();
    }
    os << " }";
    return os;
}

// Character 基类实现
Character::Character(CharacterType type, const std::string& name)
    : mType(type), mName(name) {}

// Lucky Star 实现
LuckyStar::LuckyStar() 
    : Character(CharacterType::LUCKY_STAR, "Lucky Star") 
{
    mUsesRemaining = MAX_USES;
    mCooldown = 0;
    mIsInCooldown = false;
}

bool LuckyStar::CanUseSkill() const {
    return mUsesRemaining > 0 && !mIsInCooldown;
}

void LuckyStar::UseSkill() {
    if (CanUseSkill()) {
        mUsesRemaining--;
        mIsInCooldown = true;
        mCooldown = 1; // 下一回合冷却
    }
}

void LuckyStar::ResetForNewRound() {
    // Lucky Star 的技能次数在整个游戏中有限制
    // 不重置使用次数，只重置冷却
    mIsInCooldown = false;
    mCooldown = 0;
}

void LuckyStar::UpdateCooldown() {
    if (mIsInCooldown && mCooldown > 0) {
        mCooldown--;
        if (mCooldown == 0) {
            mIsInCooldown = false;
        }
    }
}

// Collector 实现
Collector::Collector()
    : Character(CharacterType::COLLECTOR, "Collector")
{
    mUsesRemaining = 1;
    mCooldown = 0;
    mIsInCooldown = false;
}

bool Collector::CanUseSkill() const {
    return mUsesRemaining > 0 && !mIsInCooldown;
}

void Collector::UseSkill() {
    if (CanUseSkill()) {
        mUsesRemaining = 0; // 每回合只能用一次
        mIsInCooldown = true;
        mCooldown = 1; // 下一回合冷却
    }
}

void Collector::ResetForNewRound() {
    mUsesRemaining = 1;
    mIsInCooldown = false;
    mCooldown = 0;
}

void Collector::UpdateCooldown() {
    if (mIsInCooldown && mCooldown > 0) {
        mCooldown--;
        if (mCooldown == 0) {
            mIsInCooldown = false;
        }
    }
}

// Thief 实现
Thief::Thief()
    : Character(CharacterType::THIEF, "Thief")
{
    mUsesRemaining = 1;
    mCooldown = 0;
    mIsInCooldown = false;
}

bool Thief::CanUseSkill() const {
    return mUsesRemaining > 0 && !mIsInCooldown;
}

void Thief::UseSkill() {
    if (CanUseSkill()) {
        mUsesRemaining = 0;
        mIsInCooldown = true;
        mCooldown = 1;
    }
}

void Thief::ResetForNewRound() {
    mUsesRemaining = 1;
    mIsInCooldown = false;
    mCooldown = 0;
}

void Thief::UpdateCooldown() {
    if (mIsInCooldown && mCooldown > 0) {
        mCooldown--;
        if (mCooldown == 0) {
            mIsInCooldown = false;
        }
    }
}

// Defender 实现
Defender::Defender()
    : Character(CharacterType::DEFENDER, "Defender")
{
    mUsesRemaining = 1;
    mCooldown = 0;
    mIsInCooldown = false;
}

bool Defender::CanUseSkill() const {
    // Defender 的技能是被动的，不能主动使用
    return false;
}

void Defender::UseSkill() {
    // 被动技能，不主动使用
}

void Defender::ResetForNewRound() {
    mUsesRemaining = 1;
    mIsInCooldown = false;
    mCooldown = 0;
}

void Defender::UpdateCooldown() {
    if (mIsInCooldown && mCooldown > 0) {
        mCooldown--;
        if (mCooldown == 0) {
            mIsInCooldown = false;
        }
    }
}

bool Defender::TryDefend() {
    if (mUsesRemaining > 0 && !mIsInCooldown) {
        mUsesRemaining = 0;
        mIsInCooldown = true;
        mCooldown = 1;
        return true;
    }
    return false;
}

// CharacterFactory 实现
std::unique_ptr<Character> CharacterFactory::CreateCharacter(CharacterType type) {
    switch (type) {
        case CharacterType::LUCKY_STAR:
            return std::make_unique<LuckyStar>();
        case CharacterType::COLLECTOR:
            return std::make_unique<Collector>();
        case CharacterType::THIEF:
            return std::make_unique<Thief>();
        case CharacterType::DEFENDER:
            return std::make_unique<Defender>();
        default:
            return nullptr;
    }
}

CharacterType CharacterFactory::GetRandomCharacter() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 3);
    
    return static_cast<CharacterType>(dis(gen));
}

std::string CharacterFactory::GetCharacterName(CharacterType type) {
    switch (type) {
        case CharacterType::LUCKY_STAR:
            return "Lucky Star";
        case CharacterType::COLLECTOR:
            return "Collector";
        case CharacterType::THIEF:
            return "Thief";
        case CharacterType::DEFENDER:
            return "Defender";
        default:
            return "None";
    }
}

}}
