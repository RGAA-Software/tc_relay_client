//
// Created by RGAA on 12/06/2025.
//

#include "relay_api.h"
#include "tc_common_new/log.h"
#include "tc_common_new/http_client.h"
#include "tc_3rdparty/json/json.hpp"
#include "relay_message.pb.h"

using namespace tc;
using namespace nlohmann;

namespace relay
{

    tc::Result<std::shared_ptr<RelayDeviceInfo>, RelayError> RelayApi::GetRelayDeviceInfo(const std::string& host, int port, const std::string& device_id) {
        auto client =
                HttpClient::Make(host, port, kRelayGetDeviceInfo, 3000);
        auto resp = client->Request({
            {"device_id", device_id},
        });
        if (resp.status != 200 || resp.body.empty()) {
            LOGE("Request new device failed.");
            return TRError(RelayError::kRelayRequestFailed);
        }

        try {
            //LOGI("==> RelayDevice body: {}", resp.body);
            auto obj = json::parse(resp.body);
            if (obj["code"].get<int>() != 200) {
                LOGE("GetDevice info failed: {}", obj["code"]);
                return TRError(RelayError::kRelayJustCodeError);
            }
            // "device_local_ips": "110.0.0.16;192.168.56.1;",
            // "relay_server_ip": "139.171.84.236",
            // "device_w3c_ip": "139.171.84.236",
            // "device_id": "server_488050198",
            // "relay_server_port": "40302"
            auto device_w3c_ip = obj["data"]["device_w3c_ip"].get<std::string>();
            auto relay_server_ip = obj["data"]["relay_server_ip"].get<std::string>();
            auto relay_server_port = std::atoi(obj["data"]["relay_server_port"].get<std::string>().c_str());

            auto info = std::make_shared<RelayDeviceInfo>();
            info->set_device_id(device_id);
            info->set_device_w3c_ip(device_w3c_ip);
            info->set_relay_server_ip(relay_server_ip);
            info->set_relay_server_port(relay_server_port);
            return info;
        } catch(std::exception& e) {
            LOGE("GetRelayDeviceInfo Exception: {}", e.what());
            return TRError(RelayError::kRelayParseJsonFailed);
        }
    }

    bool RelayApi::IsRelayDeviceValid(const std::shared_ptr<RelayDeviceInfo>& info) {
        return !info->device_id().empty();
    }

}