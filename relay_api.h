//
// Created by RGAA on 28/02/2025.
//

#ifndef GAMMARAY_RELAY_APIS_H
#define GAMMARAY_RELAY_APIS_H

#include <string>
#include <memory>
#include "relay_errors.h"
#include "tc_3rdparty/expt/expected.h"

namespace relay
{

    const std::string kRelayGetDeviceInfo = "/query/device";

    class RelayDeviceInfo;

    class RelayApi {
    public:
        // id has prefix, eg: server_xxxx
        static tc::Result<std::shared_ptr<RelayDeviceInfo>, RelayError>
                GetRelayDeviceInfo(const std::string& host, int port, const std::string& device_id);

        static bool IsRelayDeviceValid(const std::shared_ptr<RelayDeviceInfo>& info);

    };

}

#endif //GAMMARAY_RELAY_APIS_H
