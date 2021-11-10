#include "Timer.h"
#include <iostream>

Timer::Timer() : s(), e(), pausing()
{
    name = "Timer misc";
};
Timer::Timer(const std::string name) : s(), e(), pausing(), name(name) {

}
void Timer::Start() { s = getNow(); }
void Timer::Start(const std::string& name)
{
    ReName(name);
    Start();
}
;
void Timer::Stop()
{
    e = getNow();
    if (pausing.size() % 2 != 0)
        pausing.push_back(e);
};
bool Timer::Pause()
{
    if (pausing.size() % 2 == 0) {
        pausing.push_back(getNow());
        return true;
    }
    return false;
};
bool Timer::Resume()
{
    if (pausing.size() % 2 != 0) {
        pausing.push_back(getNow());
        return true;
    }
    return false;
};
void Timer::Log(bool frames) {
    Stop();
    std::string units = frames ? " Frames" : " Milliseconds";
    std::cout << name << ": " << std::to_string(GetDuration(frames)) << units << std::endl;
};
const float& Timer::GetDuration(bool frames)
{
    auto elementAt = [](std::list<std::chrono::time_point<std::chrono::high_resolution_clock>>& l, int i) {
        auto s = l.begin();
        std::advance(s, i);
        return *s;
    };

    float d = calcDurration(s, e) / 1000.0f;
    for (short i = 0; i < pausing.size(); i += 2)
        d -= calcDurration(elementAt(pausing, i), elementAt(pausing, i + 1)) / 1000.0f;
    if (frames)
        d = (d / 1000.0f) * 60.0f;
    return d;
};
void Timer::ReName(const std::string& name) {
    this->name = name;
}
const std::chrono::time_point<std::chrono::high_resolution_clock> Timer::getNow()
{
    return std::chrono::high_resolution_clock::now();
};
float Timer::calcDurration(const std::chrono::time_point<std::chrono::high_resolution_clock>& s, const std::chrono::time_point<std::chrono::high_resolution_clock>& e) {
    return std::chrono::duration_cast<std::chrono::microseconds>(e - s).count();
}