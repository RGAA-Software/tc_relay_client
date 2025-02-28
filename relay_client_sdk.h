//
// Created by RGAA on 28/02/2025.
//

#ifndef GAMMARAY_RELAY_CLIENT_SDK_H
#define GAMMARAY_RELAY_CLIENT_SDK_H

#include <memory>
#include <functional>
#include "relay_client_sdk_param.h"
#include "tc_common_new/concurrent_hashmap.h"

namespace tc
{

    class RelayRoom;
    class RelayContext;
    class RelayWsClient;
    class RelayMessage;

    class RelayClientSdk {
    public:
        explicit RelayClientSdk(const RelayClientSdkParam& param);
        void Start();
        void Stop();
        void SetOnRelayProtoMessageCallback(std::function<void(const std::shared_ptr<RelayMessage>&)>&& cbk);
        void PostProtoMessage(const std::string& msg);
        void SyncDeviceId(const std::string& device_id);

    private:
        std::shared_ptr<RelayMessage> ProcessProtoMessage(const std::string& msg);

    private:
        std::shared_ptr<RelayContext> context_ = nullptr;
        RelayClientSdkParam sdk_param_;
        std::shared_ptr<RelayWsClient> ws_client_ = nullptr;
        tc::ConcurrentHashMap<std::string, std::shared_ptr<RelayRoom>> rooms_;
    };
}

#endif //GAMMARAY_RELAY_CLIENT_SDK_H
