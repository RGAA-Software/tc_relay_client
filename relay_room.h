//
// Created by RGAA on 28/02/2025.
//

#ifndef GAMMARAY_RELAY_ROOM_H
#define GAMMARAY_RELAY_ROOM_H

#include <string>

namespace tc
{
    // which room I'm in
    class RelayRoom {
    public:
        std::string room_id_;
        // creator
        std::string device_id_;
        //
        std::string remote_device_id_;
    };

}

#endif //GAMMARAY_RELAY_ROOM_H
