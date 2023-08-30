#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>

// UTC timestamp + random number(0 ~ 100000) + counter
std::string GenerateUniqueSeqId() {
    static std::atomic<int64_t> s_counter(0);
    static std::random_device s_seeder;
    static std::mt19937 s_engine(static_cast<std::mt19937::result_type>(
        s_seeder.entropy() ? s_seeder() : std::time(nullptr)));
    static std::uniform_int_distribution<int> s_dist(0, 100000);
    auto tsMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    std::stringstream ss;
    ss << tsMs << s_dist(s_engine);
    std::cout << "=====\n" << ss.str() << std::endl;
    auto hashNum1 = std::hash<std::string>{}(ss.str());
    std::cout << hashNum1 << "  ";
    ss.str("");
    ss << ++s_counter;
    auto hashNum2 = std::hash<std::string>{}(ss.str());
    std::cout << hashNum2 << std::endl;
    ss.str("");
    ss << std::hex << hashNum1 << hashNum2;
    std::cout << ss.str() << std::endl;
    return "";
}

int main(int argc, char** argv) {
    while (true) {
        GenerateUniqueSeqId();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // sleep(1);
    }
    return 0;
}
