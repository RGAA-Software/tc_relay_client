//
// Created by RGAA on 28/02/2025.
//

#include "relay_ws_client.h"
#include "tc_common_new/log.h"
#include <asio2/websocket/ws_client.hpp>
#include <asio2/asio2.hpp>

namespace tc
{

    const int kMaxClientQueuedMessage = 4096;

    RelayWsClient::RelayWsClient(const std::string& host, int port, const std::string& path) {
        this->host_ = host;
        this->port_ = port;
        this->path_ = path;
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
                     asio2::last_error_val(), asio2::last_error_msg().c_str(), host_, port_, path_);
            } else {
                LOGI("connect success : {} {} ", client_->local_address().c_str(), client_->local_port());
                client_->post_queued_event([=, this]() {

                });
            }
        }).bind_disconnect([this]() {

        }).bind_upgrade([]() {
            if (asio2::get_last_error()) {
                LOGE("upgrade failure : {}, {}", asio2::last_error_val(), asio2::last_error_msg());
            }
        }).bind_recv([=, this](std::string_view data) {
            std::string cpy_data(data.data(), data.size());
        });

        // the /ws is the websocket upgraged target
        if (!client_->async_start(this->host_, this->port_, this->path_)) {
            LOGE("connect websocket server failure : {} {}", asio2::last_error_val(), asio2::last_error_msg().c_str());
        }
    }

    void RelayWsClient::Stop() {
        if (client_ && client_->is_started()) {
            client_->stop_all_timers();
            client_->stop();
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
        LOGI("Post message: {}", msg);
        client_->async_send(msg, [this]() {
            queued_msg_count_--;
        });
    }

    bool RelayWsClient::IsAlive() {
        return client_ && client_->is_started();
    }

    void RelayWsClient::HeartBeat() {
        //todo:
        PostBinaryMessage("ddfb");
    }

}