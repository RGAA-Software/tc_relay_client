//
// Created by RGAA on 28/02/2025.
//

#include "relay_client_sdk.h"
#include "relay_ws_client.h"
#include "relay_message.pb.h"

namespace tc
{

    RelayClientSdk::RelayClientSdk(const RelayClientSdkParam& param) {
        sdk_param_ = param;
        ws_client_ = std::make_shared<RelayWsClient>(sdk_param_);
    }

    void RelayClientSdk::SetOnRelayProtoMessageCallback(std::function<void(const std::shared_ptr<RelayMessage>&)>&& cbk) {
        ws_client_->SetOnRelayProtoMessageCallback([=, this](const std::string& msg) {
            auto proto_msg = ProcessProtoMessage(msg);
            if (proto_msg) {
                cbk(proto_msg);
            }
            else {
                LOGE("Parse relay proto message failed!");
            }
        });
    }

    void RelayClientSdk::Start() {
        ws_client_->Start();
    }

    void RelayClientSdk::Stop() {
        if (ws_client_) {
            ws_client_->Stop();
        }
    }

    void RelayClientSdk::SyncDeviceId(const std::string& device_id) {
        sdk_param_.device_id_ = device_id;
    }

    void RelayClientSdk::PostProtoMessage(const std::string& msg) {
        // msg : tc::Message
        // rl_msg : tc::RelayMessage
        RelayMessage rl_msg;
        rl_msg.set_device_id(sdk_param_.device_id_);
        rl_msg.set_type(RelayMessageType::kRelayTargetMessage);
        auto relay = rl_msg.mutable_relay();
        relay->set_room_id("");
        relay->set_remote_device_id("");
        relay->set_payload(msg);
    }

    std::shared_ptr<RelayMessage> RelayClientSdk::ProcessProtoMessage(const std::string& msg) {
        return nullptr;
    }
}