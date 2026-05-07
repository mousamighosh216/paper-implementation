#include "../../storage/oplog/log_manager.h"
#include "../../core/serialization/codec.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <filesystem>

void run_write_test(const std::string& path) {
    LogManager log(path);
    std::vector<std::string> files = {"/data/video.mp4", "/docs/resume.pdf", "/logs/system.log"};

    std::cout << "Writing 3 log entries..." << std::endl;
    for (const auto& f : files) {
        Serializer s;
        s.write_string(f);
        assert(log.append(OpType::CREATE_FILE, s.buffer));
    }
    std::cout << "Done. Program 'crashing' now..." << std::endl;
}

void run_recovery_test(const std::string& path) {
    LogManager log(path);
    std::cout << "Recovering from log..." << std::endl;

    auto entries = log.replay();
    std::cout << "Recovered count: "
          << entries.size()
          << "\n";

    std::vector<std::string> expected = {"/data/video.mp4", "/docs/resume.pdf", "/logs/system.log"};
    
    for (size_t i = 0; i < entries.size(); ++i) {
        Deserializer d(entries[i].payload);
        std::string filename = d.read_string();
        std::cout << "Recovered: " << filename << std::endl;
        assert(filename == expected[i]);
        assert(entries[i].type == OpType::CREATE_FILE);
    }
    std::cout << "✅ Recovery Success! Persistence Verified." << std::endl;
}

int main(int argc, char* argv[]) {
    std::string log_file = "test_oplog.bin";

    if (argc > 1 && std::string(argv[1]) == "--recover") {
        run_recovery_test(log_file);
        std::filesystem::remove(log_file); // Cleanup after success
    } else {
        run_write_test(log_file);
        std::cout << "\nRun the test again with --recover to verify persistence!" << std::endl;
    }
    
    return 0;
}