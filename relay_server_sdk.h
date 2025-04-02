//
// Created by RGAA on 1/03/2025.
//

#ifndef GAMMARAY_RELAY_SERVER_SDK_H
#define GAMMARAY_RELAY_SERVER_SDK_H

#include <memory>
#include <functional>
#include <atomic>
#include "relay_callbacks.h"
#include "relay_server_sdk_param.h"
#include "tc_common_new/concurrent_hashmap.h"

namespace relay
{
    class RelayMessage;
}

namespace tc
{
    class RelayRoom;
    class RelayContext;
    class RelayWsClient;

    class RelayServerSdk {
    public:
        explicit RelayServerSdk(const RelayServerSdkParam& param);
        void Start();
        void Stop();
        void SetOnConnectedCallback(OnRelayServerConnected&& cbk);
        void SetOnDisConnectedCallback(OnRelayServerDisConnected && cbk);
        void SetOnRelayProtoMessageCallback(std::function<void(const std::shared_ptr<relay::RelayMessage>&)>&& cbk);
        void SetOnRoomPreparedCallback(OnRelayRoomPrepared&& cbk);
        void SetOnRoomDestroyedCallback(OnRelayRoomDestroyed&& cbk);

        void RelayProtoMessage(const std::string& msg);
        void RelayProtoMessage(const std::string& stream_id, const std::string& msg);
        void SyncDeviceId(const std::string& device_id);

        int64_t GetQueuingMsgCount();

    private:
        void PostBinMessage(const std::string& msg);
        std::shared_ptr<relay::RelayMessage> ProcessProtoMessage(const std::string& msg);
        // request to control this device
        void OnRequestControl(const std::shared_ptr<relay::RelayMessage>& msg);

        void OnRoomPrepared(const std::shared_ptr<relay::RelayMessage>& msg);
        void OnRoomInfoChanged(const std::shared_ptr<relay::RelayMessage>& msg);
        void OnRoomDestroyed(const std::shared_ptr<relay::RelayMessage>& msg);

    private:
        std::shared_ptr<RelayContext> context_ = nullptr;
        RelayServerSdkParam sdk_param_;
        std::shared_ptr<RelayWsClient> ws_client_ = nullptr;
        tc::ConcurrentHashMap<std::string, std::shared_ptr<RelayRoom>> rooms_;
        OnRelayRoomPrepared room_prepared_cbk_;
        OnRelayRoomDestroyed room_destroyed_cbk_;
        std::atomic_uint64_t relay_msg_index_ = 0;
    };

}


#endif //GAMMARAY_RELAY_SERVER_SDK_H
