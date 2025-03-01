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

    // Only connect to one remote
    class RelayClientSdk {
    public:
        explicit RelayClientSdk(const RelayClientSdkParam& param);
        void Start();
        void Stop();
        void SetOnRelayProtoMessageCallback(std::function<void(const std::shared_ptr<RelayMessage>&)>&& cbk);
        void RelayProtoMessage(const std::string& msg);

        // send from client
        void RequestCreateRoom();
        // received from server
        void OnCreatedRoomResp(const std::shared_ptr<RelayMessage>& msg);

        // send from client
        void RequestControl();
        // received from server
        void OnRequestControlResp(const std::shared_ptr<RelayMessage>& msg);

        // send from client
        void RequestStopRelay();
        // received from server
        void OnRequestStopRelayResp(const std::shared_ptr<RelayMessage>& msg);

    private:
        void PostBinMessage(const std::string& msg);
        std::shared_ptr<RelayMessage> ProcessProtoMessage(const std::string& msg);

        void OnRoomPrepared(const std::shared_ptr<RelayMessage>& msg);
        void OnRoomInfoChanged(const std::shared_ptr<RelayMessage>& msg);
        void OnRoomDestroyed(const std::shared_ptr<RelayMessage>& msg);

    private:
        std::shared_ptr<RelayContext> context_ = nullptr;
        RelayClientSdkParam sdk_param_;
        std::shared_ptr<RelayWsClient> ws_client_ = nullptr;
        std::shared_ptr<RelayRoom> room_ = nullptr;
    };
}

#endif //GAMMARAY_RELAY_CLIENT_SDK_H
