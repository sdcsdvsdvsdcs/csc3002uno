// LuckyStar.cpp
#include "LuckyStar.h"
#include "GameState.h"
#include "Player.h"
#include <iostream>

// 幸运星构造函数
LuckyStar::LuckyStar() : Role("幸运星", "抽牌时查看牌堆顶部3张牌并选择1张", false) {
    usesLeft = 3;
    maxUses = 3;
    maxCooldown = 1;
}

// 使用幸运星技能
SkillResult LuckyStar::useSkill(Player& user, GameState& game, Player* target) {
    SkillResult result;
    
    if (!canUseSkill(user, game)) {
        result.success = false;
        result.message = "技能不可用！冷却中或使用次数已用完。";
        return result;
    }
    
    // 检查UI管理器是否设置
    if (!uiManager) {
        result.success = false;
        result.message = "UI系统未初始化";
        return result;
    }
    
    std::cout << "【幸运星技能发动】查看牌堆顶部3张牌..." << std::endl;
    
    try {
        // 获取牌堆顶部3张牌
        std::vector<Card> topCards = game.peekTopCards(3);
        
        if (topCards.size() < 3) {
            result.success = false;
            result.message = "牌堆中牌不足3张，无法使用技能";
            return result;
        }
        
        // 通过UI管理器显示卡牌选择
        int choice = uiManager->showCardSelection(topCards, "选择一张牌加入手牌");
        
        if (choice >= 0 && choice < topCards.size()) {
            Card chosenCard = topCards[choice];
            
            // 从牌堆移除选择的牌
            game.removeCardFromDeck(choice);
            
            // 将牌加入玩家手牌
            user.addCardToHand(chosenCard);
            
            result.success = true;
            result.message = "获得了: " + chosenCard.toString();
            result.affectedCards.push_back(chosenCard);
            
            // 更新使用次数和冷却
            usesLeft--;
            cooldown = maxCooldown;
            
            std::cout << "幸运星技能使用成功！获得了: " << chosenCard.toString() << std::endl;
        } else {
            result.success = false;
            result.message = "选择取消，技能未使用";
        }
    } catch (const std::exception& e) {
        result.success = false;
        result.message = "技能使用失败: " + std::string(e.what());
    }
    
    return result;
}
