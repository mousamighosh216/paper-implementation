#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <optional>

struct Lease {
    std::string primary_addr;
    std::vector<std::string> secondaries;
    std::chrono::steady_clock::time_point expires_at;

    bool is_expired() const {
        return std::chrono::steady_clock::now() > expires_at;
    }
};

class LeaseManager {
public:
    // Tries to get an existing lease or grants a new one
    // Returns {Primary, Secondaries}
    std::pair<std::string, std::vector<std::string>> grantOrGetLease(
        uint64_t handle, 
        const std::vector<std::string>& replicas
    );

private:
    std::unordered_map<uint64_t, Lease> leases;
    const std::chrono::seconds LEASE_DURATION = std::chrono::seconds(60);
};