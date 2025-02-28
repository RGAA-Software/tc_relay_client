project(tc_relay_client)

set(SRCS
        relay_context.cpp
        relay_client_sdk.cpp
        relay_ws_client.cpp
)

add_library(${PROJECT_NAME} ${SRCS})