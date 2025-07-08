//
// Created by RGAA on 1/03/2025.
//

#include "relay_server_sdk.h"
#include "relay_ws_client.h"
#include "relay_message.pb.h"
#include "relay_room.h"
#include "tc_common_new/time_util.h"
#include "tc_common_new/md5.h"
#include "tc_common_new/data.h"
#include "tc_common_new/uuid.h"
#include "relay_connected_info.h"

using namespace relay;

namespace tc
{

    RelayServerSdk::RelayServerSdk(const RelayServerSdkParam& param) {
        sdk_param_ = param;
        ws_client_ = std::make_shared<RelayWsClient>(sdk_param_.host_,
                                                     sdk_param_.port_,
                                                     sdk_param_.device_id_,
                                                     sdk_param_.device_name_,
                                                     sdk_param_.stream_id_);
        ws_client_->SetDeviceNetInfo(param.net_info_);
    }

    void RelayServerSdk::Start() {
        if (ws_client_) {
            ws_client_->Start();
        }
    }

    void RelayServerSdk::Stop() {
        if (ws_client_) {
            ws_client_->Stop();
        }
    }

    void RelayServerSdk::SetOnConnectedCallback(OnRelayServerConnected&& cbk) {
        ws_client_->SetOnRelayServerConnectedCallback([=, this]() {
            cbk();
            connected_ = true;
        });
    }

    void RelayServerSdk::SetOnDisConnectedCallback(OnRelayServerDisConnected&& cbk) {
        ws_client_->SetOnRelayServerDisConnectedCallback([=, this]() {
            cbk();
            connected_ = false;
            rooms_.Clear();
        });
    }

    void RelayServerSdk::SetOnRelayProtoMessageCallback(std::function<void(const std::shared_ptr<RelayMessage>&)>&& cbk) {
        ws_client_->SetOnRelayProtoMessageCallback([=, this](std::shared_ptr<Data> msg) {
            auto proto_msg = ProcessProtoMessage(msg);
            if (proto_msg) {
                cbk(proto_msg);
            }
            else {
                LOGE("Parse relay proto message failed!");
            }
        });
    }

    void RelayServerSdk::SetOnRoomPreparedCallback(OnRelayRoomPrepared&& cbk) {
        room_prepared_cbk_ = cbk;
    }

    void RelayServerSdk::SetOnRoomDestroyedCallback(OnRelayRoomDestroyed&& cbk) {
        room_destroyed_cbk_ = cbk;
    }

    void RelayServerSdk::SetOnRequestPauseStreamCallback(OnRelayRequestPausedStream&& cbk) {
        pause_stream_cbk_ = cbk;
    }

    void RelayServerSdk::SetOnRequestResumeStreamCallback(OnRelayRequestResumeStream&& cbk) {
        resume_stream_cbk_ = cbk;
    }

//    void RelayServerSdk::RelayProtoMessage(const std::string& msg) {
//        if (rooms_.Size() <= 0 || !IsAlive()) {
//            return;
//        }
//
//        //LOGI("Relay proto message size: {}", msg.size());
//        // msg : tc::Message
//        // rl_msg : tc::RelayMessage
//        RelayMessage rl_msg;
//        rl_msg.set_from_device_id(sdk_param_.device_id_);
//        rl_msg.set_type(RelayMessageType::kRelayTargetMessage);
//        auto relay = rl_msg.mutable_relay();
//        relay->set_relay_msg_index(relay_msg_index_++);
//        auto room_ids = relay->mutable_room_ids();
//        rooms_.ApplyAll([&](const auto& k, const std::shared_ptr<RelayRoom>& r) {
//            room_ids->Add(r->room_id_.c_str());
//        });
//        relay->set_payload(msg);
//
//        this->PostBinMessage(rl_msg.SerializeAsString());
//    }

    void RelayServerSdk::RelayProtoMessage(const std::string& stream_id, std::shared_ptr<Data> msg) {
        if (rooms_.Size() <= 0 || !IsAlive() || !msg) {
            return;
        }

        RelayMessage rl_msg;
        rl_msg.set_from_device_id(sdk_param_.device_id_);
        rl_msg.set_type(RelayMessageType::kRelayTargetMessage);
        auto relay = rl_msg.mutable_relay();
        relay->set_relay_msg_index(relay_msg_index_++);
        auto room_ids = relay->mutable_room_ids();
        rooms_.ApplyAll([&](const auto& k, const std::shared_ptr<RelayRoom>& r) {
            room_ids->Add(r->room_id_.c_str());
        });
        relay->set_payload(msg->AsString());

        this->PostBinMessage(rl_msg.SerializeAsString());
    }

    void RelayServerSdk::PostBinMessage(const std::string& msg) {
        if (ws_client_) {
            ws_client_->PostBinaryMessage(msg);
        }
    }

    std::shared_ptr<RelayMessage> RelayServerSdk::ProcessProtoMessage(std::shared_ptr<Data> msg) {
        auto rl_msg = std::make_shared<RelayMessage>();
        if (!rl_msg->ParsePartialFromArray(msg->CStr(), msg->Size())) {
            return nullptr;
        }

        auto type = rl_msg->type();
        if (type == RelayMessageType::kRelayRequestControl) {
            this->OnRequestControl(rl_msg);
        }
        else if (type == RelayMessageType::kRelayRoomPrepared) {
            this->OnRoomPrepared(rl_msg);
            if (room_prepared_cbk_) {
                room_prepared_cbk_(rl_msg);
            }
        }
        else if (type == RelayMessageType::kRelayRoomDestroyed) {
            if (room_destroyed_cbk_) {
                room_destroyed_cbk_(rl_msg);
            }
            this->OnRoomDestroyed(rl_msg);
        }
        else if (type == RelayMessageType::kRelayRequestPausedStream) {
            if (pause_stream_cbk_) {
                pause_stream_cbk_();
            }
        }
        else if (type == RelayMessageType::kRelayRequestResumeStream) {
            if (resume_stream_cbk_) {
                resume_stream_cbk_();
            }
        }

        return rl_msg;
    }

    void RelayServerSdk::OnRequestControl(const std::shared_ptr<RelayMessage>& msg) {
        auto rc = msg->request_control();
        if (sdk_param_.device_id_ != rc.remote_device_id()) {
            LOGE("My device id: {}, request remote id: {}", sdk_param_.device_id_, rc.remote_device_id());
            return;
        }

        RelayMessage rl_msg;
        rl_msg.set_type(RelayMessageType::kRelayRequestControlResp);
        auto sub = rl_msg.mutable_request_control_resp();
        sub->set_device_id(rc.device_id());
        sub->set_remote_device_id(rc.remote_device_id());
        sub->set_room_id(rc.room_id());
        sub->set_message("ok");
        sub->set_under_control(true);
        auto resp_msg = rl_msg.SerializeAsString();
        this->PostBinMessage(resp_msg);
    }

    void RelayServerSdk::OnRoomPrepared(const std::shared_ptr<RelayMessage>& msg) {
        auto rp = msg->room_prepared();
        auto room = std::make_shared<RelayRoom>();
        room->room_id_ = rp.room_id();
        room->device_id_ = rp.device_id();
        room->remote_device_id_ = rp.remote_device_id();
        room->created_timestamp_ = (int64_t)TimeUtil::GetCurrentTimestamp();
        room->creator_device_name_ = rp.creator_device_name();
        room->creator_stream_id_ = rp.creator_stream_id();
        room->conn_id_ = MD5::Hex(GetUUID());
        rooms_.Insert(room->room_id_, room);
        LOGI("** OnRoomPrepared: {}", room->room_id_);
    }

    void RelayServerSdk::OnRoomInfoChanged(const std::shared_ptr<RelayMessage>& msg) {
        auto rc = msg->room_info_changed();
        const auto& room_id = rc.room_id();
        auto room = rooms_.TryGet(room_id).value_or(nullptr);
        if (!room) {
            return;
        }
        rooms_.VisitAll([=](std::string k, std::shared_ptr<RelayRoom>& rr) {

        });
    }

    void RelayServerSdk::OnRoomDestroyed(const std::shared_ptr<RelayMessage>& msg) {
        auto rd = msg->room_destroyed();
        rooms_.Remove(rd.room_id());
        LOGI("** OnRoomDestroyed: {}", rd.room_id());
    }

    bool RelayServerSdk::IsAlive() {
        return ws_client_ && ws_client_->IsAlive();
    }

    int64_t RelayServerSdk::GetQueuingMsgCount() {
        return ws_client_->GetQueuingMsgCount();
    }

    bool RelayServerSdk::HasRelayRooms() {
        return rooms_.Size() > 0;
    }

    std::shared_ptr<RelayRoom> RelayServerSdk::GetRoomById(const std::string& room_id) {
        if (auto r = rooms_.TryGet(room_id); r.has_value()) {
            return r.value();
        }
        return nullptr;
    }

    int RelayServerSdk::GetConnectedClientsCount() {
        //LOGI("Connected: {}, room size: {}", connected_, rooms_.Size());
        return connected_ && rooms_.Size() > 0 ? 1 : 0;
    }

    std::vector<std::shared_ptr<RelayConnectedClientInfo>> RelayServerSdk::GetConnectedClientInfo() {
        std::vector<std::shared_ptr<RelayConnectedClientInfo>> clients_info;
        rooms_.VisitAll([&](std::string k, std::shared_ptr<RelayRoom>& room) {
            clients_info.push_back(std::make_shared<RelayConnectedClientInfo>(RelayConnectedClientInfo {
                .room_id_ = room->room_id_,
                .device_id_ = room->device_id_,
                .stream_id_ = room->creator_stream_id_,
                .device_name_ = room->creator_device_name_,
            }));
        });
        return clients_info;
    }

}