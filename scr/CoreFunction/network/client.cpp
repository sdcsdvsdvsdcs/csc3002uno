#include <iostream>
#include <map>

#include "client.h"

namespace UNO { namespace Network {

using asio::ip::tcp;

Client::Client(std::string host, std::string port) 
    : mHost(host), mPort(port)
{}

void Client::Connect()
{
    tcp::resolver resolver(mContext);
    auto endpoints = resolver.resolve(mHost, mPort);

    while (mShouldReset) {
        mShouldReset = false;
        tcp::socket socket(mContext);
        asio::async_connect(socket, endpoints,
            [this, &socket](std::error_code ec, tcp::endpoint) {
                if (!ec) {
                    mSession = std::make_unique<Session>(std::move(socket));
                }
            }
        );

        mContext.run();
        if (!mSession) {
            // connection failure
            std::cout << "Service not found, connection failure." << std::endl;
            std::exit(-1);
        }
        OnConnect();
    }
}

void Client::Reset()
{
    mShouldReset = true;
    mContext.restart();
}

std::unique_ptr<Info> Client::ReceiveInfo(const std::type_info *infoType)
{
    using funcType = std::function<std::unique_ptr<Info>()>;
    static std::map<const std::type_info *, funcType> mapping{
        {&typeid(JoinGameInfo),    [this] { return ReceiveInfoImpl<JoinGameInfo>(); }},
        {&typeid(JoinGameRspInfo), [this] { return ReceiveInfoImpl<JoinGameRspInfo>(); }},
        {&typeid(GameStartInfo),   [this] { return ReceiveInfoImpl<GameStartInfo>(); }},
        {&typeid(ActionInfo),      [this] { return ReceiveInfoImpl<ActionInfo>(); }},
        {&typeid(DrawInfo),        [this] { return ReceiveInfoImpl<DrawInfo>(); }},
        {&typeid(SkipInfo),        [this] { return ReceiveInfoImpl<SkipInfo>(); }},
        {&typeid(PlayInfo),        [this] { return ReceiveInfoImpl<PlayInfo>(); }},
        {&typeid(DrawRspInfo),     [this] { return ReceiveInfoImpl<DrawRspInfo>(); }},
        // 新增：角色系统消息类型
        {&typeid(SkillUseInfo),    [this] { return ReceiveInfoImpl<SkillUseInfo>(); }},
        {&typeid(SkillRspInfo),    [this] { return ReceiveInfoImpl<SkillRspInfo>(); }},
        // 新增：特殊效果消息类型
        {&typeid(SpecialEffectInfo), [this] { return ReceiveInfoImpl<SpecialEffectInfo>(); }},
        // 新增：游戏状态更新消息
        {&typeid(GameStateUpdateInfo), [this] { return ReceiveInfoImpl<GameStateUpdateInfo>(); }}
    };

    auto it = mapping.find(infoType);
    assert(it != mapping.end());
    std::unique_ptr<Info> info;
    try {
        info = it->second();
    }
    catch (const std::exception &e) {
        /// TODO: handle the condition that server has shutdown
        std::cout << "oops, server has shutdown" << std::endl;
        std::exit(-1);
    }
    return info;
}

void Client::DeliverInfo(const std::type_info *infoType, const Info &info)
{
    using funcType = std::function<void(const Info &)>;
    static std::map<const std::type_info *, funcType> mapping{
        {&typeid(JoinGameInfo),    [this](const Info &info) { 
            DeliverInfoImpl<JoinGameInfo>(dynamic_cast<const JoinGameInfo &>(info)); 
        }},
        {&typeid(JoinGameRspInfo), [this](const Info &info) { 
            DeliverInfoImpl<JoinGameRspInfo>(dynamic_cast<const JoinGameRspInfo &>(info)); 
        }},
        {&typeid(GameStartInfo),   [this](const Info &info) { 
            DeliverInfoImpl<GameStartInfo>(dynamic_cast<const GameStartInfo &>(info)); 
        }},
        {&typeid(ActionInfo),      [this](const Info &info) { 
            DeliverInfoImpl<ActionInfo>(dynamic_cast<const ActionInfo &>(info)); 
        }},
        {&typeid(DrawInfo),        [this](const Info &info) { 
            DeliverInfoImpl<DrawInfo>(dynamic_cast<const DrawInfo &>(info)); 
        }},
        {&typeid(SkipInfo),        [this](const Info &info) { 
            DeliverInfoImpl<SkipInfo>(dynamic_cast<const SkipInfo &>(info)); 
        }},
        {&typeid(PlayInfo),        [this](const Info &info) { 
            DeliverInfoImpl<PlayInfo>(dynamic_cast<const PlayInfo &>(info)); 
        }},
        {&typeid(DrawRspInfo),     [this](const Info &info) { 
            DeliverInfoImpl<DrawRspInfo>(dynamic_cast<const DrawRspInfo &>(info)); 
        }},
        // 新增：角色系统消息类型
        {&typeid(SkillUseInfo),    [this](const Info &info) { 
            DeliverInfoImpl<SkillUseInfo>(dynamic_cast<const SkillUseInfo &>(info)); 
        }},
        {&typeid(SkillRspInfo),    [this](const Info &info) { 
            DeliverInfoImpl<SkillRspInfo>(dynamic_cast<const SkillRspInfo &>(info)); 
        }},
        // 新增：特殊效果消息类型
        {&typeid(SpecialEffectInfo), [this](const Info &info) { 
            DeliverInfoImpl<SpecialEffectInfo>(dynamic_cast<const SpecialEffectInfo &>(info)); 
        }},
        // 新增：游戏状态更新消息
        {&typeid(GameStateUpdateInfo), [this](const Info &info) { 
            DeliverInfoImpl<GameStateUpdateInfo>(dynamic_cast<const GameStateUpdateInfo &>(info)); 
        }}
    };
    auto it = mapping.find(infoType);
    assert(it != mapping.end());
    return it->second(info);
}
}}
