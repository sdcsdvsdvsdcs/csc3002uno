// Collector.cpp
#include "Collector.h"
#include "GameState.h"
#include "Player.h"
#include <iostream>

// 收藏家构造函数
Collector::Collector() : Role("收藏家", "从弃牌堆选择1张牌加入手牌", false) {
    usesLeft = -1;     // 无限使用
    maxCooldown = 1;   // 使用后冷却1回合
}

// 使用收藏家技能
SkillResult Collector::useSkill(Player& user, GameState& game, Player* target) {
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
    
    std::cout << "【收藏家技能发动】查看弃牌堆..." << std::endl;
    
    try {
        // 获取弃牌堆中的所有牌
        std::vector<Card> discardPile = game.getDiscardPile();
        
        if (discardPile.empty()) {
            result.success = false;
            result.message = "弃牌堆为空，技能使用失败！";
            return result;
        }
        
        // 过滤掉刚刚丢弃的牌
        std::vector<Card> availableCards;
        std::vector<int> availableIndices;
        
        for (int i = 0; i < discardPile.size(); i++) {
            if (!game.isJustDiscarded(discardPile[i])) {
                availableCards.push_back(discardPile[i]);
                availableIndices.push_back(i);
            }
        }
        
        if (availableCards.empty()) {
            result.success = false;
            result.message = "没有可选择的牌（都是刚刚丢弃的牌）";
            return result;
        }
        
        // 通过UI管理器显示卡牌选择
        int choiceIndex = uiManager->showCardSelection(availableCards, "从弃牌堆选择一张牌");
        
        if (choiceIndex >= 0 && choiceIndex < availableCards.size()) {
            int actualIndex = availableIndices[choiceIndex];
            Card chosenCard = discardPile[actualIndex];
            
            // 从弃牌堆移除牌
            game.removeCardFromDiscard(actualIndex);
            
            // 将牌加入玩家手牌
            user.addCardToHand(chosenCard);
            
            result.success = true;
            result.message = "获得了: " + chosenCard.toString() + "（已向所有玩家展示）";
            result.affectedCards.push_back(chosenCard);
            
            // 通知所有玩家（展示选择的牌）
            game.broadcastMessage(user.getName() + " 使用收藏家技能获得了: " + chosenCard.toString());
            
            // 进入冷却
            cooldown = maxCooldown;
            
            std::cout << "收藏家技能使用成功！获得了: " << chosenCard.toString() << std::endl;
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
