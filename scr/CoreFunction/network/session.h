#pragma once

#include <iostream>
#include <asio.hpp>

#include "../game/info.h"

namespace UNO {

namespace Test {
    class SessionFixture;
}

namespace Network {

using asio::ip::tcp;

class Session {
public:
    explicit Session(tcp::socket socket);

    template<typename InfoT>
    std::unique_ptr<InfoT> ReceiveInfo() {
        Read();
        return InfoT::Deserialize(mReadBuffer);
    }

    template<typename InfoT>
    void DeliverInfo(const InfoT &info) {
        info.Serialize(mWriteBuffer);
        Write();
    }

    // for test
    template<typename InfoT, typename... Types>
    void DeliverInfo(Types&&... args) {
        InfoT info(args...);
        info.Serialize(mWriteBuffer);
        Write();
    }

    // 新增：检查连接状态
    bool IsConnected() const {
        return mSocket.is_open();
    }

    // 新增：关闭连接
    void Close() {
        if (mSocket.is_open()) {
            std::error_code ec;
            mSocket.close(ec);
        }
    }

    // 新增：获取远程端点信息（用于调试）
    std::string GetRemoteEndpoint() const {
        if (mSocket.is_open()) {
            std::error_code ec;
            auto endpoint = mSocket.remote_endpoint(ec);
            if (!ec) {
                return endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
            }
        }
        return "unknown";
    }

private:
    // read from mSocket to mReadBuffer
    void Read();

    // write from mWriteBuffer to mSocket
    void Write();

    // 新增：验证消息长度
    bool ValidateMessageLength(int expectedLen, int actualLen);

private:
    // 增加缓冲区大小以容纳新增的消息类型
    constexpr static int MAX_BUFFER_SIZE = 512;  // 从256增加到512

    tcp::socket mSocket;
    uint8_t mReadBuffer[MAX_BUFFER_SIZE];
    uint8_t mWriteBuffer[MAX_BUFFER_SIZE];

    friend class Test::SessionFixture;
};
}}
