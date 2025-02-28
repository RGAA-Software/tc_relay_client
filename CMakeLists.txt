project(tc_relay_client)

set(SRCS
        relay_context.cpp
        relay_client_sdk.cpp
        relay_ws_client.cpp
        relay_room.cpp
)

add_library(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} tc_relay_message libprotobuf)