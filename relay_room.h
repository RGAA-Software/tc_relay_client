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
        [[nodiscard]] bool IsValid() const;
        void Clear();

    public:
        std::string room_id_;
        // creator
        std::string device_id_;
        //
        std::string remote_device_id_;
        //
        int64_t created_timestamp_{0};
        //
        std::string the_conn_id_;
    };

}

#endif //GAMMARAY_RELAY_ROOM_H
