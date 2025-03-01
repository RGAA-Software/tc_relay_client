//
// Created by RGAA on 28/02/2025.
//

#include "relay_client_sdk.h"
#include "relay_ws_client.h"
#include "relay_message.pb.h"
#include "relay_room.h"

namespace tc
{

    RelayClientSdk::RelayClientSdk(const RelayClientSdkParam& param) {
        sdk_param_ = param;
        ws_client_ = std::make_shared<RelayWsClient>(sdk_param_.host_, sdk_param_.port_, sdk_param_.device_id_);
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

    std::shared_ptr<RelayNetClient> RelayClientSdk::GetNetClient() {
        return ws_client_;
    }

    void RelayClientSdk::RelayProtoMessage(const std::string& msg) {
        if (!room_) {
            LOGE("Can't relay message, room is null.");
            return;
        }
        // msg : tc::Message
        // rl_msg : tc::RelayMessage
        RelayMessage rl_msg;
        rl_msg.set_from_device_id(sdk_param_.device_id_);
        rl_msg.set_type(RelayMessageType::kRelayTargetMessage);
        auto relay = rl_msg.mutable_relay();
        auto room_ids = relay->mutable_room_ids();
        room_ids->Add(room_->room_id_.c_str());
        relay->set_payload(msg);

        this->PostBinMessage(rl_msg.SerializeAsString());
    }

    void RelayClientSdk::PostBinMessage(const std::string& msg) {
        if (ws_client_) {
            ws_client_->PostBinaryMessage(msg);
        }
    }

    std::shared_ptr<RelayMessage> RelayClientSdk::ProcessProtoMessage(const std::string& msg) {
        auto rl_msg = std::make_shared<RelayMessage>();
        if (!rl_msg->ParseFromString(msg)) {
            return nullptr;
        }

        return rl_msg;
    }

    // send from client
    void RelayClientSdk::RequestCreateRoom() {
        RelayMessage rl_msg;
        rl_msg.set_type(RelayMessageType::kRelayCreateRoom);
        auto sub = rl_msg.mutable_create_room();
        sub->set_device_id(sdk_param_.device_id_);
        sub->set_remote_device_id(sdk_param_.remote_device_id_);
        this->PostBinMessage(rl_msg.SerializeAsString());
    }

    // received from server
    void RelayClientSdk::OnCreatedRoomResp(const std::shared_ptr<RelayMessage>& msg) {
        auto cr = msg->create_room_resp();
        room_ = std::make_shared<RelayRoom>();
        room_->room_id_ = cr.room_id();
    }

    // send from client
    void RelayClientSdk::RequestControl() {
        if (!room_) {
            LOGE("Can't request control, room is null.");
            return;
        }
        RelayMessage rl_msg;
        rl_msg.set_type(RelayMessageType::kRelayRequestControl);
        auto sub = rl_msg.mutable_request_control();
        sub->set_device_id(sdk_param_.device_id_);
        sub->set_remote_device_id(sdk_param_.remote_device_id_);
        sub->set_room_id(room_->room_id_);
        this->PostBinMessage(rl_msg.SerializeAsString());
    }

    // received from server
    void RelayClientSdk::OnRequestControlResp(const std::shared_ptr<RelayMessage>& msg) {

    }

    // send from client
    void RelayClientSdk::RequestStopRelay() {
        if (!room_) {
            LOGE("Can't request stop, room is null.");
            return;
        }
        RelayMessage rl_msg;
        rl_msg.set_type(RelayMessageType::kRelayRequestStop);
        auto sub = rl_msg.mutable_request_stop();
        sub->set_device_id(sdk_param_.device_id_);
        sub->set_room_id(room_->room_id_);
        sub->set_remote_device_id(sdk_param_.remote_device_id_);
        this->PostBinMessage(rl_msg.SerializeAsString());
    }

    // received from server
    void RelayClientSdk::OnRequestStopRelayResp(const std::shared_ptr<RelayMessage>& msg) {
        // todo: callback it.
    }

    void RelayClientSdk::OnRoomPrepared(const std::shared_ptr<RelayMessage>& msg) {
        auto in_room_id = msg->room_prepared().room_id();
        if (!room_ || room_->room_id_ != in_room_id) {
            LOGE("Can't process RoomPrepared, room is null! {}", in_room_id);
            return;
        }
        auto rp = msg->room_prepared();
        room_->device_id_ = rp.device_id();
        room_->remote_device_id_ = rp.remote_device_id();
    }

    void RelayClientSdk::OnRoomInfoChanged(const std::shared_ptr<RelayMessage>& msg) {
        auto rc = msg->room_info_changed();
        const auto& room_id = rc.room_id();

    }

    void RelayClientSdk::OnRoomDestroyed(const std::shared_ptr<RelayMessage>& msg) {
        auto rd = msg->room_destroyed();

    }

}