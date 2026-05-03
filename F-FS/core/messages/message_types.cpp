#pragma once
#include <cstdint>

enum class MessageType : uint8_t {
    CLIENT_TO_MASTER = 1,
    MASTER_TO_CLIENT = 2,
    CS_TO_MASTER     = 3,
    MASTER_TO_CS     = 4,
    CLIENT_TO_CS     = 5,
    CS_TO_CLIENT     = 6,
    CS_TO_CS         = 7,
    CS_ACK           = 8
};