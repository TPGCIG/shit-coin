cmake_minimum_required(VERSION 3.15)
project(P2PClient VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
endif()

include(FetchContent)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3 # Use a stable release tag
)
FetchContent_MakeAvailable(json)

find_package(Threads REQUIRED)

set(PROJECT_SOURCES
    src/main.cpp
    src/p2p/net/listener.cpp
    src/p2p/net/dispatcher.cpp
    src/p2p/net/scopedsocket.cpp
    src/p2p/net/node.cpp
    src/p2p/peers.cpp
)

add_executable(p2p_client
    src/main.cpp
    src/p2p/net/listener.cpp
    src/p2p/net/dispatcher.cpp
    src/p2p/net/scopedsocket.cpp
    src/p2p/net/node.cpp
    src/p2p/peers.cpp
)

target_include_directories(p2p_client PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(p2p_client PRIVATE 
    Threads::Threads
    nlohmann_json::nlohmann_json
)
