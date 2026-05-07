cmake_minimum_required(VERSION 3.10)
project(F_FS_DistributedSystem VERSION 1.0 LANGUAGES CXX)

# 1. Global Settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Platform Specific Dependencies (Winsock for Windows)
if(WIN32)
    set(NET_LIBS ws2_32)
    add_definitions(-D_WIN32_WINNT=0x0601) # Target Windows 7 or higher
endif()

# 3. Include Directories
# This allows you to use #include "core/network/socket.hpp" without relative paths
include_directories(${CMAKE_SOURCE_DIR})

# 4. Source Groups (Organizing code for better management)
set(COMMON_SOURCES
    core/network/socket.cpp
    core/network/connection.cpp
    core/protocol/frame.cpp
)

set(MASTER_SOURCES
    services/master/master_logic.cpp
    storage/oplog/log_manager.cpp
    ${COMMON_SOURCES}
)

set(CHUNK_SOURCES
    services/chunkserver/chunk_store.cpp
    ${COMMON_SOURCES}
)

# 5. Define Executables
# Master Node
add_executable(master_node bin/master_node.cpp ${MASTER_SOURCES})
target_link_libraries(master_node PRIVATE ${NET_LIBS})

# Chunk Server Node
add_executable(chunk_server bin/chunk_server_node.cpp ${CHUNK_SOURCES})
target_link_libraries(chunk_server PRIVATE ${NET_LIBS})

# 6. Unit Tests
enable_testing()

add_executable(message_test tests/unit/message_test.cpp)
add_executable(oplog_test tests/unit/oplog_test.cpp storage/oplog/log_manager.cpp)
add_executable(chunk_test tests/unit/chunk_store_test.cpp services/chunkserver/chunk_store.cpp)

# Link networking to tests if they include headers that use socket types
target_link_libraries(message_test PRIVATE ${NET_LIBS})
target_link_libraries(oplog_test PRIVATE ${NET_LIBS})
target_link_libraries(chunk_test PRIVATE ${NET_LIBS})