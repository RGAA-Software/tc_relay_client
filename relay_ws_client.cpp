//
// Created by RGAA on 28/02/2025.
//

#include "relay_ws_client.h"
#include "tc_common_new/log.h"
#include <asio2/websocket/ws_client.hpp>
#include <asio2/asio2.hpp>
#include "relay_message.pb.h"

namespace tc
{

    const int kMaxClientQueuedMessage = 4096;

    RelayWsClient::RelayWsClient(const std::string& host, int port, const std::string& device_id) : RelayNetClient() {
        this->host_ = host;
        this->port_ = port;
        this->device_id_ = device_id;
    }

    RelayWsClient::~RelayWsClient() {

    }

    void RelayWsClient::Start() {
        client_ = std::make_shared<asio2::ws_client>();
        client_->set_auto_reconnect(true);
        client_->set_timeout(std::chrono::milliseconds(2000));
        client_->start_timer("ws-heartbeat", std::chrono::seconds(1), [=, this]() {
            this->HeartBeat();
        });

        client_->bind_init([=, this]() {
            client_->ws_stream().binary(true);
            client_->set_no_delay(true);
            client_->ws_stream().set_option(
                    websocket::stream_base::decorator([](websocket::request_type &req) {
                        req.set(http::field::authorization, "websocket-client-authorization");}
                    )
            );

        }).bind_connect([=, this]() {
            if (asio2::get_last_error()) {
                LOGE("connect failure : {} {} [ {} - {} - {}]",
                     asio2::last_error_val(), asio2::last_error_msg().c_str(),host_, port_, device_id_);
            } else {
                LOGI("connect success : {} {} ", client_->local_address().c_str(), client_->local_port());
                client_->post_queued_event([=, this]() {
                    if (srv_conn_cbk_) {
                        srv_conn_cbk_();
                    }
                    this->SendHello();
                });
            }
        }).bind_disconnect([this]() {
            if (srv_dis_conn_cbk_) {
                srv_dis_conn_cbk_();
            }
        }).bind_upgrade([]() {
            if (asio2::get_last_error()) {
                LOGE("upgrade failure : {}, {}", asio2::last_error_val(), asio2::last_error_msg());
            }
        }).bind_recv([=, this](std::string_view data) {
            if (msg_cbk_) {
                std::string cpy_data(data.data(), data.size());
                msg_cbk_(cpy_data);
            }
        });

        // the /ws is the websocket upgraged target
        auto ws_path = std::format("/relay?device_id={}", device_id_);
        LOGI("Will connect: {}:{}{}", host_, port_, ws_path);
        if (!client_->async_start(host_, port_, ws_path)) {
            LOGE("connect websocket server failure : {} {}", asio2::last_error_val(), asio2::last_error_msg().c_str());
        }
    }

    void RelayWsClient::Stop() {
        if (client_ && client_->is_started()) {
            client_->stop_all_timers();
            client_->stop();
            client_ = nullptr;
        }
    }

    void RelayWsClient::PostBinaryMessage(const std::string &msg) {
        if (!IsAlive()) {
            return;
        }
        if (queued_msg_count_ > kMaxClientQueuedMessage) {
            LOGW("You've queued too many messages!");
            return;
        }
        queued_msg_count_++;
        client_->async_send(msg, [this]() {
            queued_msg_count_--;
        });
    }

    bool RelayWsClient::IsAlive() {
        return client_ && client_->is_started();
    }

    void RelayWsClient::SyncDeviceId(const std::string& device_id) {
        this->device_id_ = device_id;
    }

    void RelayWsClient::SendHello() {
        if (!IsAlive()) {
            return;
        }
        RelayMessage rl_msg;
        rl_msg.set_from_device_id(this->device_id_);
        auto sub = rl_msg.mutable_hello();
        auto msg = rl_msg.SerializeAsString();
        PostBinaryMessage(msg);
    }

    void RelayWsClient::HeartBeat() {
        if (!IsAlive()) {
            return;
        }
        RelayMessage rl_msg;
        rl_msg.set_from_device_id(this->device_id_);
        auto sub = rl_msg.mutable_heartbeat();
        static int64_t hb_ibx = 0;
        sub->set_index(hb_ibx++);
        auto msg = rl_msg.SerializeAsString();
        PostBinaryMessage(msg);
    }

}