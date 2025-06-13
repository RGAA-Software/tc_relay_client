project(tc_relay_client)

set(SRCS
        relay_context.cpp
        relay_client_sdk.cpp
        relay_ws_client.cpp
        relay_room.cpp
        relay_server_sdk.cpp
        relay_api.cpp
)

add_library(${PROJECT_NAME} ${SRCS})
if(ANDROID)
    find_package(Protobuf REQUIRED)
    target_link_libraries(${PROJECT_NAME} tc_relay_message protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)
else ()
    target_link_libraries(${PROJECT_NAME} tc_relay_message libprotobuf)
endif ()