//
// Created by RGAA on 1/03/2025.
//

#ifndef GAMMARAY_RELAY_CALLBACKS_H
#define GAMMARAY_RELAY_CALLBACKS_H

#include <string>
#include <memory>
#include <functional>

namespace tc
{

    using OnRelayServerConnected = std::function<void()>;
    using OnRelayServerDisConnected = std::function<void()>;

}

#endif //GAMMARAY_RELAY_CALLBACKS_H
