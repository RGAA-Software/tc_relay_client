//
// Created by RGAA on 28/02/2025.
//

#ifndef GAMMARAY_RELAY_WS_CLIENT_H
#define GAMMARAY_RELAY_WS_CLIENT_H

#include <memory>
#include <string>

namespace asio2 {
    class ws_client;
    class timer;
}

namespace tc
{

    class RelayWsClient {
    public:
        RelayWsClient(const std::string& host, int port, const std::string& path);
        ~RelayWsClient();
        void Start();
        void Stop();
        void PostBinaryMessage(const std::string &msg);

    private:
        bool IsAlive();
        void HeartBeat();

    private:
        std::string host_;
        int port_ = 0;
        std::string path_;
        std::shared_ptr<asio2::ws_client> client_ = nullptr;
        std::atomic_int queued_msg_count_ = 0;
    };

}

#endif //GAMMARAY_RELAY_WS_CLIENT_H