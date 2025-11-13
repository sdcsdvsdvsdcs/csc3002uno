void PlayerStat::AssignCharacter(std::unique_ptr<Character> character) {
    mCharacter = std::move(character);
}

void GameStat::AdvancePhase() {
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
