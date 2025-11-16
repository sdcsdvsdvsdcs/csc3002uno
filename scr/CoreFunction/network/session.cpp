#include "session.h"
#include <spdlog/spdlog.h>

namespace UNO { namespace Network {

Session::Session(tcp::socket socket) : mSocket(std::move(socket)) 
{
#ifdef ENABLE_LOG
    spdlog::info("Session created for endpoint: {}", GetRemoteEndpoint());
#endif
}

/**
 * Read will throw end-of-file exception if the corresponding client has disconnected
 */
void Session::Read()
{
    std::memset(mReadBuffer, 0, MAX_BUFFER_SIZE);
    
    try {
        // read header
        asio::read(mSocket, asio::buffer(mReadBuffer, sizeof(Msg)));

        // read body
        int len = reinterpret_cast<Msg *>(mReadBuffer)->mLen;
        
        // 验证消息长度
        if (len < 0 || len > (MAX_BUFFER_SIZE - sizeof(Msg))) {
            throw std::runtime_error("Invalid message length: " + std::to_string(len));
        }
        
        asio::read(mSocket, asio::buffer(mReadBuffer + sizeof(Msg), len));

#ifdef ENABLE_LOG
        // 记录接收的消息类型（调试用）
        MsgType msgType = reinterpret_cast<Msg *>(mReadBuffer)->mType;
        spdlog::debug("Received message type: {}, length: {} from {}", 
                     static_cast<int>(msgType), len, GetRemoteEndpoint());
#endif
    }
    catch (const std::exception &e) {
#ifdef ENABLE_LOG
        spdlog::error("Read error from {}: {}", GetRemoteEndpoint(), e.what());
#endif
        throw; // 重新抛出异常，让上层处理
    }
}

void Session::Write() 
{
    try {
        Msg *msg = reinterpret_cast<Msg *>(mWriteBuffer);
        int len = sizeof(Msg) + msg->mLen;
        
        // 验证消息长度
        if (len > MAX_BUFFER_SIZE) {
            throw std::runtime_error("Message too large for buffer: " + std::to_string(len));
        }
        
        asio::async_write(mSocket, asio::buffer(mWriteBuffer, len), 
            [this](std::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    std::memset(mWriteBuffer, 0, MAX_BUFFER_SIZE);
#ifdef ENABLE_LOG
                    spdlog::debug("Successfully sent {} bytes to {}", 
                                 bytes_transferred, GetRemoteEndpoint());
#endif
                } else {
#ifdef ENABLE_LOG
                    spdlog::error("Write error to {}: {}", 
                                 GetRemoteEndpoint(), ec.message());
#endif
                }
            }
        );

#ifdef ENABLE_LOG
        // 记录发送的消息类型（调试用）
        MsgType msgType = reinterpret_cast<Msg *>(mWriteBuffer)->mType;
        spdlog::debug("Sending message type: {}, length: {} to {}", 
                     static_cast<int>(msgType), len, GetRemoteEndpoint());
#endif
    }
    catch (const std::exception &e) {
#ifdef ENABLE_LOG
        spdlog::error("Write error to {}: {}", GetRemoteEndpoint(), e.what());
#endif
        throw;
    }
}

bool Session::ValidateMessageLength(int expectedLen, int actualLen)
{
    if (expectedLen != actualLen) {
#ifdef ENABLE_LOG
        spdlog::warn("Message length mismatch: expected {}, got {} from {}", 
                    expectedLen, actualLen, GetRemoteEndpoint());
#endif
        return false;
    }
    return true;
}

}}
