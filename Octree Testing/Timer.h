#pragma once
#include <string>
#include <chrono>
#include <list>

class Timer {
public:
    Timer(const std::string name);
    Timer();
    void start();
    void start(const std::string& name);
    void stop();
    bool pause();
    bool resume();
    void log(bool frames = 0);
    const float& getDuration(bool frames = 0);
    void reName(const std::string& name);
private:
    std::string name;
    static const std::chrono::time_point<std::chrono::high_resolution_clock> getNow();
    static float calcDurration(const std::chrono::time_point<std::chrono::high_resolution_clock>& s, const std::chrono::time_point<std::chrono::high_resolution_clock>& e);
    std::chrono::time_point<std::chrono::high_resolution_clock> s, e;
    std::list<std::chrono::time_point<std::chrono::high_resolution_clock>> pausing;
};