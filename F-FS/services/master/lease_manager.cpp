#include "lease_manager.h"
#include <algorithm>

std::pair<std::string, std::vector<std::string>> LeaseManager::grantOrGetLease(
    uint64_t handle, 
    const std::vector<std::string>& replicas
) {
    auto it = leases.find(handle);

    // If lease exists and is still valid, return it
    if (it != leases.end() && !it->second.is_expired()) {
        return {it->second.primary_addr, it->second.secondaries};
    }

    // Otherwise, grant a NEW lease
    // For simplicity, we pick the first replica as Primary
    std::string primary = replicas[0];
    std::vector<std::string> secondaries;
    for (size_t i = 1; i < replicas.size(); ++i) {
        secondaries.push_back(replicas[i]);
    }

    Lease new_lease = {
        primary,
        secondaries,
        std::chrono::steady_clock::now() + LEASE_DURATION
    };

    leases[handle] = new_lease;
    return {primary, secondaries};
}