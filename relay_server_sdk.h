//
// Created by RGAA on 1/03/2025.
//

#ifndef GAMMARAY_RELAY_SERVER_SDK_H
#define GAMMARAY_RELAY_SERVER_SDK_H

#include <memory>
#include <functional>
#include "relay_server_sdk_param.h"
#include "tc_common_new/concurrent_hashmap.h"

namespace tc
{
    class RelayRoom;
    class RelayContext;
    class RelayWsClient;
    class RelayMessage;

    class RelayServerSdk {
    public:
        explicit RelayServerSdk(const RelayServerSdkParam& param);
        void Start();
        void Stop();
        void SetOnRelayProtoMessageCallback(std::function<void(const std::shared_ptr<RelayMessage>&)>&& cbk);
        void RelayProtoMessage(const std::string& msg);
        void RelayProtoMessage(const std::string& stream_id, const std::string& msg);
        void SyncDeviceId(const std::string& device_id);

    private:
        void PostBinMessage(const std::string& msg);
        std::shared_ptr<RelayMessage> ProcessProtoMessage(const std::string& msg);
        // request to control this device
        void OnRequestControl(const std::shared_ptr<RelayMessage>& msg);

        void OnRoomPrepared(const std::shared_ptr<RelayMessage>& msg);
        void OnRoomInfoChanged(const std::shared_ptr<RelayMessage>& msg);
        void OnRoomDestroyed(const std::shared_ptr<RelayMessage>& msg);

    private:
        std::shared_ptr<RelayContext> context_ = nullptr;
        RelayServerSdkParam sdk_param_;
        std::shared_ptr<RelayWsClient> ws_client_ = nullptr;
        tc::ConcurrentHashMap<std::string, std::shared_ptr<RelayRoom>> rooms_;
    };

}


#endif //GAMMARAY_RELAY_SERVER_SDK_H
