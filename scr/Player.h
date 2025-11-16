// 在 Player 类中
class Player {
    //...
public:
    void DiscardAllNumberCardsOfColor(Card::CardColor color, 
                                      DiscardPile& discardPile);
private:
    PlayerHand m_hand; // std::vector<std::unique_ptr<Card>>
};
