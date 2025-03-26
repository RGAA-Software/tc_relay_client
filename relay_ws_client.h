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
#include "relay_device_info.h"

namespace asio2 {
    class ws_client;
    class timer;
}

namespace tc
{

    class RelayWsClient : public RelayNetClient, public std::enable_shared_from_this<RelayWsClient> {
    public:
        explicit RelayWsClient(const std::string& host, int port, const std::string& device_id);
        ~RelayWsClient() override;
        void Start() override;
        void Stop() override;
        void PostBinaryMessage(const std::string& msg) override;
        void PostTextMessage(const std::string& msg) override;
        void SyncDeviceId(const std::string& device_id) override;

        void SetDeviceNetInfo(const std::vector<tc::RelayDeviceNetInfo>& info);

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
        unsigned int post_thread_id_ = 0;
        std::vector<tc::RelayDeviceNetInfo> net_info_;
    };

}

#endif //GAMMARAY_RELAY_WS_CLIENT_H