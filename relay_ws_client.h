//
// Created by RGAA on 28/02/2025.
//

#ifndef GAMMARAY_RELAY_WS_CLIENT_H
#define GAMMARAY_RELAY_WS_CLIENT_H

#include <memory>
#include <string>
#include <functional>
#include "relay_callbacks.h"
#include "relay_net_client.h"

namespace asio2 {
    class ws_client;
    class timer;
}

namespace tc
{

    class RelayWsClient : public RelayNetClient {
    public:
        explicit RelayWsClient(const std::string& host, int port, const std::string& device_id);
        ~RelayWsClient() override;
        void Start() override;
        void Stop() override;
        void PostBinaryMessage(const std::string &msg) override;
        void SyncDeviceId(const std::string& device_id) override;

    private:
        bool IsAlive();
        void SendHello();
        void HeartBeat();

    private:
        std::string host_;
        int port_{0};
        std::string device_id_;
        std::shared_ptr<asio2::ws_client> client_ = nullptr;
        std::atomic_int queued_msg_count_ = 0;

    };

}

#endif //GAMMARAY_RELAY_WS_CLIENT_H