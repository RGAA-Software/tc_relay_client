//
// Created by RGAA on 28/02/2025.
//

#include "relay_client_sdk.h"
#include "relay_ws_client.h"

namespace tc
{

    RelayClientSdk::RelayClientSdk(const RelayClientSdkParam& param) {
        sdk_param_ = param;
    }

    void RelayClientSdk::Start() {
        ws_client_ = std::make_shared<RelayWsClient>(sdk_param_.host_, sdk_param_.port_, sdk_param_.path_);
        ws_client_->Start();
    }

    void RelayClientSdk::Stop() {
        if (ws_client_) {
            ws_client_->Stop();
        }
    }

}