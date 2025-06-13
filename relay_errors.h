//
// Created by RGAA on 12/06/2025.
//

#ifndef GAMMARAY_RELAY_ERRORS_H
#define GAMMARAY_RELAY_ERRORS_H

#include <string>

namespace relay
{
    enum class RelayError {
        kRelayOk,
        kRelayUnknown,
        kRelayParseJsonFailed,   // parse json failed
        kRelayRequestFailed,     // http request failed
        kRelayJustCodeError,     // code != 200
        // detail code errors below
        // kRelay xxx
        kRelayDataError,         // data is invalid
    };

    static std::string RelayError2String(const RelayError& err) {
        if (err == RelayError::kRelayOk) {
            return "Ok";
        }
        else if (err == RelayError::kRelayUnknown) {
            return "Unknown";
        }
        else if (err == RelayError::kRelayParseJsonFailed) {
            return "Parse json failed";
        }
        else if (err == RelayError::kRelayRequestFailed) {
            return "Network request failed";
        }
        else if (err == RelayError::kRelayJustCodeError) {
            return "Code error, != 200";
        }
        else if (err == RelayError::kRelayDataError) {
            return "Data error";
        }
        else {
            return "Unknown";
        }
    }
}

#endif //GAMMARAY_RELAY_ERRORS_H
