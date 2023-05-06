#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <csignal>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

bool g_is_running = true;

void signal_handler(int signum) {
    if (signum == SIGINT || signum == SIGQUIT) {
        g_is_running = false;
    }
}

void thread_print_id(const std::string& name) {
    prctl(PR_SET_NAME, name.c_str());
    while (g_is_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        auto pid = pthread_self();
        auto thread_id = std::this_thread::get_id();
        auto tid = syscall(__NR_gettid);
        printf("==== %s pid: %lld, thread_id: %lld, tid: %d\n", name.c_str(), pid, thread_id, tid);
        // std::cout << "==== " << name << ", pid: " << pid
            // << ", thread_id: " << thread_id << ", tid: " << tid << std::endl;
    }
    printf("Thread %s exit\n", name.c_str());
}

int main(int argc, char** argv) {
    signal(SIGINT, signal_handler);
    std::vector<std::thread> threads;
    threads.emplace_back(thread_print_id, "thread_1");
    threads.emplace_back(thread_print_id, "thread_2");
    threads.emplace_back(thread_print_id, "thread_3");

    for (auto& t : threads) {
        t.join();
    }
    return 0;
}
