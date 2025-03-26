//
// Created by RGAA on 1/03/2025.
//

#ifndef GAMMARAY_RELAY_SERVER_SDK_PARAM_H
#define GAMMARAY_RELAY_SERVER_SDK_PARAM_H

#include <string>

namespace tc
{
    //
    class RelayDeviceNetInfo {
    public:
        std::string ip_;
        std::string mac_;
    };

    // RelayServerSdkParam
    class RelayServerSdkParam {
    public:
        std::string host_;
        int port_{0};
        bool ssl_ = false;
        std::string device_id_;
        std::vector<RelayDeviceNetInfo> net_info_;
    };


}

#endif //GAMMARAY_RELAY_SERVER_SDK_PARAM_H
