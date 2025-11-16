#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <random>

// 前向声明
class Player;
class GameState;
class Card;

// 角色基类
class Role {
protected:
    std::string name;
    std::string description;
    int cooldown;      // 当前冷却回合数
    int maxCooldown;   // 最大冷却回合数
    int usesLeft;      // 剩余使用次数
    int maxUses;       // 最大使用次数
    bool isPassive;    // 是否是被动技能

public:
    Role(const std::string& n, const std::string& desc, bool passive = false) 
        : name(n), description(desc), cooldown(0), maxCooldown(1), 
          usesLeft(-1), maxUses(-1), isPassive(passive) {}
    
    virtual ~Role() = default;
    
    // 检查技能是否可以使用
    virtual bool canUseSkill(const Player& user, const GameState& game) const {
        // 被动技能不能主动使用
        if (isPassive) return false;
        // 检查冷却时间和使用次数
        return cooldown == 0 && (usesLeft == -1 || usesLeft > 0);
    }
    
    // 使用技能
    virtual bool useSkill(Player& user, GameState& game, Player* target = nullptr) = 0;
    
    // 更新冷却状态（每回合结束时调用）
    virtual void updateCooldown() {
        if (cooldown > 0) cooldown--;
    }
    
    // 获取角色信息
    virtual std::string getInfo() const {
        return name + ": " + description + 
               "\n冷却: " + std::to_string(cooldown) + "回合" +
               "\n剩余次数: " + (usesLeft == -1 ? "无限" : std::to_string(usesLeft));
    }
    
    // Getter方法
    std::string getName() const { return name; }
    int getCooldown() const { return cooldown; }
    int getUsesLeft() const { return usesLeft; }
    bool getIsPassive() const { return isPassive; }
};

// 幸运星角色
class LuckyStar : public Role {
public:
    LuckyStar() : Role("幸运星", "抽牌时查看牌堆顶部3张牌并选择1张", false) {
        usesLeft = 3;      // 最多使用3次
        maxUses = 3;
        maxCooldown = 1;   // 使用后冷却1回合
    }
    
    bool useSkill(Player& user, GameState& game, Player* target = nullptr) override {
        if (!canUseSkill(user, game)) {
            std::cout << "技能不可用！" << std::endl;
            return false;
        }
        
        std::cout << "【幸运星技能发动】查看牌堆顶部3张牌..." << std::endl;
        
        // 模拟查看牌堆顶部3张牌
        std::vector<Card> topCards = game.peekTopCards(3);
        
        // 显示可选的牌
        std::cout << "顶部3张牌为：" << std::endl;
        for (int i = 0; i < topCards.size(); i++) {
            std::cout << i + 1 << ". " << topCards[i].toString() << std::endl;
        }
        
        // 让玩家选择（这里简化处理，实际游戏中需要图形界面）
        int choice;
        std::cout << "请选择要加入手牌的牌(1-3): ";
        std::cin >> choice;
        
        if (choice >= 1 && choice <= 3) {
            // 获取选择的牌
            Card chosenCard = topCards[choice - 1];
            
            // 从牌堆移除选择的牌
            game.removeCardFromDeck(choice - 1);
            
            // 将牌加入玩家手牌
            user.addCardToHand(chosenCard);
            
            std::cout << "获得了: " << chosenCard.toString() << std::endl;
            
            // 更新使用次数和冷却
            usesLeft--;
            cooldown = maxCooldown;
            
            return true;
        }
        
        return false;
    }
};

// 收藏家角色
class Collector : public Role {
public:
    Collector() : Role("收藏家", "从弃牌堆选择1张牌加入手牌", false) {
        usesLeft = -1;     // 无限使用
        maxCooldown = 1;   // 使用后冷却1回合
    }
    
    bool useSkill(Player& user, GameState& game, Player* target = nullptr) override {
        if (!canUseSkill(user, game)) {
            std::cout << "技能不可用！" << std::endl;
            return false;
        }
        
        std::cout << "【收藏家技能发动】查看弃牌堆..." << std::endl;
        
        // 获取弃牌堆中的所有牌
        std::vector<Card> discardPile = game.getDiscardPile();
        
        if (discardPile.empty()) {
            std::cout << "弃牌堆为空，技能使用失败！" << std::endl;
            return false;
        }
        
        // 显示弃牌堆中的牌（排除刚刚丢弃的牌）
        std::cout << "弃牌堆中的牌：" << std::endl;
        for (int i = 0; i < discardPile.size(); i++) {
            // 检查是否为刚刚丢弃的牌（需要游戏状态提供这个信息）
            if (!game.isJustDiscarded(discardPile[i])) {
                std::cout << i + 1 << ". " << discardPile[i].toString() << std::endl;
            }
        }
        
        // 让玩家选择
        int choice;
        std::cout << "请选择要加入手牌的牌: ";
        std::cin >> choice;
        
        // 验证选择
        if (choice >= 1 && choice <= discardPile.size()) {
            Card chosenCard = discardPile[choice - 1];
            
            // 检查是否为刚刚丢弃的牌
            if (game.isJustDiscarded(chosenCard)) {
                std::cout << "不能选择刚刚丢弃的牌！" << std::endl;
                return false;
            }
            
            // 从弃牌堆移除牌并加入玩家手牌
            game.removeCardFromDiscard(choice - 1);
            user.addCardToHand(chosenCard);
            
            std::cout << "获得了: " << chosenCard.toString() << "（已向所有玩家展示）" << std::endl;
            
            // 进入冷却
            cooldown = maxCooldown;
            
            return true;
        }
        
        return false;
    }
};

// 小偷角色
class Thief : public Role {
public:
    Thief() : Role("小偷", "偷取其他玩家1张指定类型的牌", false) {
        usesLeft = -1;     // 无限使用
        maxCooldown = 1;   // 使用后冷却1回合
    }
    
    bool useSkill(Player& user, GameState& game, Player* target = nullptr) override {
        if (!canUseSkill(user, game)) {
            std::cout << "技能不可用！" << std::endl;
            return false;
        }
        
        if (!target) {
            std::cout << "请选择目标玩家！" << std::endl;
            return false;
        }
        
        std::cout << "【小偷技能发动】选择要偷取的牌类型..." << std::endl;
        
        // 选择牌类型
        std::cout << "选择牌类型: 1.功能牌 2.数字牌" << std::endl;
        int typeChoice;
        std::cin >> typeChoice;
        
        bool wantActionCards = (typeChoice == 1);
        std::string cardType = wantActionCards ? "功能牌" : "数字牌";
        
        std::cout << "尝试偷取" << target->getName() << "的" << cardType << "..." << std::endl;
        
        // 检查目标玩家是否有指定类型的牌
        if (!target->hasCardType(wantActionCards)) {
            std::cout << "目标玩家没有" << cardType << "，技能使用失败！" << std::endl;
            cooldown = maxCooldown;  // 即使失败也进入冷却
            return false;
        }
        
        // 随机偷取一张指定类型的牌
        Card stolenCard = target->stealRandomCardOfType(wantActionCards);
        
        // 从自己手牌中随机选择一张牌返还
        if (!user.getHand().empty()) {
            Card returnCard = user.getRandomCardFromHand();
            user.removeCardFromHand(returnCard);
            target->addCardToHand(returnCard);
            
            std::cout << "偷取了: " << stolenCard.toString() << std::endl;
            std::cout << "返还了: " << returnCard.toString() << std::endl;
        } else {
            std::cout << "偷取了: " << stolenCard.toString() << "（没有牌可返还）" << std::endl;
        }
        
        // 将偷取的牌加入自己手牌
        user.addCardToHand(stolenCard);
        
        // 进入冷却
        cooldown = maxCooldown;
        
        return true;
    }
};

// 防御者角色
class Defender : public Role {
private:
    bool canDefendThisRound;  // 本回合是否可以防御
    
public:
    Defender() : Role("防御者", "被动抵挡其他玩家的技能", true) {
        canDefendThisRound = true;
        maxCooldown = 1;      // 成功防御后冷却1回合
    }
    
    // 防御者不能主动使用技能
    bool canUseSkill(const Player& user, const GameState& game) const override {
        return false;
    }
    
    bool useSkill(Player& user, GameState& game, Player* target = nullptr) override {
        std::cout << "防御者技能是被动技能，不能主动使用！" << std::endl;
        return false;
    }
    
    // 尝试防御技能（由游戏系统调用）
    bool tryDefend(Player& attacker, Player& defender, GameState& game) {
        // 检查是否可以防御
        if (!canDefendThisRound || cooldown > 0) {
            return false;
        }
        
        // 检查是否为可防御的技能类型
        if (!isDefendableSkill(attacker.getCurrentSkill())) {
            return false;
        }
        
        std::cout << "【防御者技能发动】" << defender.getName() << "抵挡了" 
                  << attacker.getName() << "的技能！" << std::endl;
        
        // 向所有玩家揭示防御者身份
        game.revealRole(defender);
        
        // 防御者进入冷却
        canDefendThisRound = false;
        cooldown = maxCooldown;
        
        return true;
    }
    
    // 每回合开始时重置防御状态
    void resetDefense() {
        canDefendThisRound = true;
    }
    
    void updateCooldown() override {
        Role::updateCooldown();
        // 冷却结束后重置防御状态
        if (cooldown == 0) {
            resetDefense();
        }
    }
    
private:
    // 检查技能是否可防御
    bool isDefendableSkill(const std::string& skillName) {
        // 不能防御游戏规则效果和非指向性全局技能
        std::vector<std::string> undefendableSkills = {
            "draw_two", "wild_draw_four", "global_skill"
        };
        
        return std::find(undefendableSkills.begin(), 
                        undefendableSkills.end(), skillName) == undefendableSkills.end();
    }
};


    
    return 0;
}
