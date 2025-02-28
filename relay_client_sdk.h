//
// Created by RGAA on 28/02/2025.
//

#ifndef GAMMARAY_RELAY_CLIENT_SDK_H
#define GAMMARAY_RELAY_CLIENT_SDK_H

#include <memory>
#include "relay_client_sdk_param.h"

namespace tc
{
    class RelayContext;
    class RelayWsClient;

    class RelayClientSdk {
    public:
        explicit RelayClientSdk(const RelayClientSdkParam& param);
        void Start();
        void Stop();

    private:
        std::shared_ptr<RelayContext> context_ = nullptr;
        RelayClientSdkParam sdk_param_;
        std::shared_ptr<RelayWsClient> ws_client_ = nullptr;
    };
}

#endif //GAMMARAY_RELAY_CLIENT_SDK_H
