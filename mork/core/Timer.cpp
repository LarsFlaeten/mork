#include "mork/core/Timer.h"

#include <algorithm>
#include "date.h"

using namespace date;

namespace mork
{

Timer::Timer()
{
    numCycles = 0;
    running = false;
    minDuration = 1e9;
    maxDuration = 0.0;
    lastDuration = std::chrono::duration<double>::zero();
    totalDuration = std::chrono::duration<double>::zero();
}

Timer::~Timer()
{
}

void Timer::start()
{
    running = true;
    numCycles++;
    t = std::chrono::steady_clock::now();
    return;
}

double Timer::end()
{
    lastDuration = std::chrono::steady_clock::now() - t;
    totalDuration += lastDuration;
    minDuration = std::min(lastDuration.count(), minDuration);
    maxDuration = std::max(lastDuration.count(), maxDuration);
    running = false;
    return lastDuration.count();
}

void Timer::reset()
{
    start();
    numCycles = 0;
    running = false;
    minDuration = 1e9;
    maxDuration = 0.0;
    lastDuration = std::chrono::duration<double>::zero();
    totalDuration = std::chrono::duration<double>::zero();
}

int Timer::getNumCycles() const
{
    return numCycles;
}

double Timer::getTime()
{
    if(!running)
        start();

    std::chrono::duration<double> duration = std::chrono::steady_clock::now() - t;
    return duration.count();    
}

double Timer::getDuration()
{
    return lastDuration.count();
}

double Timer::getAvgTime()
{
    if (numCycles == 0) {
        return 0.0;
    }
    if (running) {
        end();
    }
    return totalDuration.count() / numCycles;
}

double Timer::getMinDuration() const
{
    return minDuration;
}

double Timer::getMaxDuration() const
{
    return maxDuration;
}

std::string Timer::getDateTimeString()
{
    auto now = std::chrono::system_clock::now();
    auto today = date::floor<date::days>(now);

    std::stringstream ss;
    ss << today << "T" << date::make_time(now - today) << "Z";
    return ss.str();
}

}
