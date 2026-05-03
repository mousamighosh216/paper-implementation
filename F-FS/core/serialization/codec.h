#pragma once
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
#endif

class Serializer {
public:
    std::vector<uint8_t> buffer;

    // We use Little-Endian to match Rust's bincode default
    void write_u64(uint64_t v) {
        for (int i = 0; i < 8; ++i) {
            buffer.push_back((v >> (i * 8)) & 0xFF);
        }
    }

    void write_string(const std::string& s) {
        write_u64(s.size()); // Length prefix
        buffer.insert(buffer.end(), s.begin(), s.end());
    }

    // This is the "Pro" Improvement: One function for any vector!
    template<typename T>
    void write_vector(const std::vector<T>& vec) {
        write_u64(vec.size());
        for (const auto& item : vec) {
            if constexpr (std::is_same_v<T, std::string>) {
                write_string(item);
            } else if constexpr (std::is_same_v<T, uint64_t>) {
                write_u64(item);
            }
            // You can add more types (uint32, bool) here as needed
        }
    }
};

class Deserializer {
public:
    const uint8_t* data;
    size_t offset;
    size_t total_size;

    Deserializer(const std::vector<uint8_t>& b) 
        : data(b.data()), offset(0), total_size(b.size()) {}

    uint64_t read_u64() {
        if (offset + 8 > total_size) throw std::runtime_error("Buffer overflow");
        uint64_t v = 0;
        for (int i = 0; i < 8; ++i) {
            v |= (static_cast<uint64_t>(data[offset + i]) << (i * 8));
        }
        offset += 8;
        return v;
    }

    std::string read_string() {
        size_t len = read_u64();
        if (offset + len > total_size) throw std::runtime_error("Buffer overflow");
        std::string s((const char*)(data + offset), len);
        offset += len;
        return s;
    }

    template<typename T>
    std::vector<T> read_vector() {
        size_t count = read_u64();
        std::vector<T> vec;
        vec.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            if constexpr (std::is_same_v<T, std::string>) {
                vec.push_back(read_string());
            } else if constexpr (std::is_same_v<T, uint64_t>) {
                vec.push_back(read_u64());
            }
        }
        return vec;
    }
};