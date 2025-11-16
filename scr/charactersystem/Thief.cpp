// Thief.cpp
#include "Thief.h"
#include "GameState.h"
#include "Player.h"
#include <iostream>
#include <random>

// 小偷构造函数
Thief::Thief() : Role("小偷", "偷取其他玩家1张指定类型的牌", false) {
    usesLeft = -1;     // 无限使用
    maxCooldown = 1;   // 使用后冷却1回合
}

// 使用小偷技能
SkillResult Thief::useSkill(Player& user, GameState& game, Player* target) {
    SkillResult result;
    
    if (!canUseSkill(user, game)) {
        result.success = false;
        result.message = "技能不可用！冷却中。";
        return result;
    }
    
    if (!uiManager) {
        result.success = false;
        result.message = "UI系统未初始化";
        return result;
    }
    
    // 如果没有指定目标，先让玩家选择目标
    if (!target) {
        std::vector<Player*> otherPlayers = game.getOtherPlayers(user.getId());
        
        if (otherPlayers.empty()) {
            result.success = false;
            result.message = "没有其他玩家可以偷取";
            return result;
        }
        
        int targetIndex = uiManager->showPlayerSelection(otherPlayers, "选择要偷取的目标玩家");
        
        if (targetIndex >= 0 && targetIndex < otherPlayers.size()) {
            target = otherPlayers[targetIndex];
        } else {
            result.success = false;
            result.message = "选择取消，技能未使用";
            return result;
        }
    }
    
    std::cout << "【小偷技能发动】选择要偷取的牌类型..." << std::endl;
    
    try {
        // 选择牌类型
        std::vector<std::string> cardTypes = {"功能牌", "数字牌"};
        std::vector<Card> dummyCards; // 用于UI显示的假数据
        for (const auto& type : cardTypes) {
            dummyCards.push_back(Card("", type, false));
        }
        
        int typeChoice = uiManager->showCardSelection(dummyCards, "选择要偷取的牌类型");
        
        if (typeChoice < 0 || typeChoice >= cardTypes.size()) {
            result.success = false;
            result.message = "选择取消，技能未使用";
            return result;
        }
        
        bool wantActionCards = (typeChoice == 0); // 0=功能牌, 1=数字牌
        std::string cardType = wantActionCards ? "功能牌" : "数字牌";
        
        std::cout << "尝试偷取" << target->getName() << "的" << cardType << "..." << std::endl;
        
        // 检查目标玩家是否有指定类型的牌
        if (!target->hasCardType(wantActionCards)) {
            result.success = false;
            result.message = "目标玩家没有" + cardType + "，技能使用失败！";
            cooldown = maxCooldown;  // 即使失败也进入冷却
            return result;
        }
        
        // 随机偷取一张指定类型的牌
        Card stolenCard = target->stealRandomCardOfType(wantActionCards);
        
        // 从自己手牌中随机选择一张牌返还
        Card returnCard;
        bool hasReturned = false;
        
        if (!user.getHand().empty()) {
            returnCard = user.getRandomCardFromHand();
            user.removeCardFromHand(returnCard);
            target->addCardToHand(returnCard);
            hasReturned = true;
        }
        
        // 将偷取的牌加入自己手牌
        user.addCardToHand(stolenCard);
        
        // 构建结果消息
        result.success = true;
        result.message = "偷取了: " + stolenCard.toString();
        if (hasReturned) {
            result.message += "，返还了: " + returnCard.toString();
        } else {
            result.message += "（没有牌可返还）";
        }
        
        result.affectedCards.push_back(stolenCard);
        if (hasReturned) {
            result.affectedCards.push_back(returnCard);
        }
        result.affectedPlayers.push_back(target);
        
        // 通知所有玩家
        game.broadcastMessage(user.getName() + " 对小偷 " + target->getName() + 
                             " 使用了偷窃技能");
        
        // 进入冷却
        cooldown = maxCooldown;
        
        std::cout << "小偷技能使用成功！" << result.message << std::endl;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.message = "技能使用失败: " + std::string(e.what());
        cooldown = maxCooldown;  // 发生异常也进入冷却
    }
    
    return result;
}
