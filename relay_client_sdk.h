//
// Created by RGAA on 28/02/2025.
//

#ifndef GAMMARAY_RELAY_CLIENT_SDK_H
#define GAMMARAY_RELAY_CLIENT_SDK_H

#include <memory>
#include <functional>
#include "relay_callbacks.h"
#include "relay_client_sdk_param.h"
#include "tc_common_new/concurrent_hashmap.h"

namespace relay
{
    class RelayMessage;
}

namespace tc
{

    class RelayRoom;
    class RelayContext;
    class RelayNetClient;

    // Only connect to one remote
    class RelayClientSdk {
    public:
        explicit RelayClientSdk(const RelayClientSdkParam& param);
        void Start();
        void Stop();
        void SetOnRelayServerConnectedCallback(OnRelayServerConnected&& cbk);
        void SetOnRelayServerDisConnectedCallback(OnRelayServerDisConnected&& cbk);
        void SetOnRelayProtoMessageCallback(std::function<void(const std::string&)>&& cbk);
        void SetOnRelayProtoMessageCallback(std::function<void(const std::shared_ptr<relay::RelayMessage>&)>&& cbk);
        void RelayProtoMessage(const std::string& msg);

        std::shared_ptr<RelayNetClient> GetNetClient();
        int64_t GetQueuingMsgCount();

        // send from client
        void RequestCreateRoom();
        // received from server
        void OnCreatedRoomResp(const std::shared_ptr<relay::RelayMessage>& msg);

        // send from client
        void RequestControl();
        // received from server
        void OnRequestControlResp(const std::shared_ptr<relay::RelayMessage>& msg);

        // send from client
        void RequestStopRelay();
        // received from server
        void OnRequestStopRelayResp(const std::shared_ptr<relay::RelayMessage>& msg);

        // received from server
        void OnErrorMessage(const std::shared_ptr<relay::RelayMessage>& msg);

        // has relay rooms or not
        bool IsInRoom();

    private:
        void PostBinMessage(const std::string& msg);
        std::shared_ptr<relay::RelayMessage> ProcessProtoMessage(const std::string& msg);

        void OnRoomPrepared(const std::shared_ptr<relay::RelayMessage>& msg);
        void OnRoomInfoChanged(const std::shared_ptr<relay::RelayMessage>& msg);
        void OnRoomDestroyed(const std::shared_ptr<relay::RelayMessage>& msg);

    private:
        std::shared_ptr<RelayContext> context_ = nullptr;
        RelayClientSdkParam sdk_param_;
        std::shared_ptr<RelayNetClient> ws_client_ = nullptr;
        std::shared_ptr<RelayRoom> room_ = nullptr;
        std::atomic_int64_t relay_msg_index_ = 0;
        std::mutex relay_mtx_;
    };
}

#endif //GAMMARAY_RELAY_CLIENT_SDK_H
