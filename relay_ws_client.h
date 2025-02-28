//
// Created by RGAA on 28/02/2025.
//

#ifndef GAMMARAY_RELAY_WS_CLIENT_H
#define GAMMARAY_RELAY_WS_CLIENT_H

#include <memory>
#include <string>
#include <functional>
#include "relay_client_sdk_param.h"

namespace asio2 {
    class ws_client;
    class timer;
}

namespace tc
{

    class RelayWsClient {
    public:
        explicit RelayWsClient(const RelayClientSdkParam& param);
        ~RelayWsClient();
        void Start();
        void Stop();
        void PostBinaryMessage(const std::string &msg);
        void SetOnRelayProtoMessageCallback(std::function<void(const std::string&)>&& cbk);
        void SyncDeviceId(const std::string& device_id);

    private:
        bool IsAlive();
        void SendHello();
        void HeartBeat();

    private:
        RelayClientSdkParam sdk_param_;
        std::shared_ptr<asio2::ws_client> client_ = nullptr;
        std::atomic_int queued_msg_count_ = 0;
        std::function<void(const std::string&)> msg_cbk_;
    };

}

#endif //GAMMARAY_RELAY_WS_CLIENT_H