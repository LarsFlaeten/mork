#ifndef _MORK_TIMER_H_
#define _MORK_TIMER_H_

#include <chrono>
#include <string>

namespace mork
{

/**
 * A timer to measure time and time intervals.
 */
class Timer
{
public:
    /**
     * Creates a new timer.
     */
    Timer();

    /**
     * Destroys this timer.
     */
    virtual ~Timer();

    /**
     * Starts this timer.
     */
    virtual void start();

    /**
     * Returns the delay since the last call to #start() in seconds.
     */
    virtual double end();

    /**
     * Returns the current time since start in seconds. If timer is not running,
     * it will be started.
     */
    virtual double getTime();


    /**
     * Returns the delay recorded at the last end() call in seconds.
     */
    virtual double getDuration();

    /**
     * Returns the average delay at every call to #end() in micro seconds.
     * This won't be accurate if the timer is not stopped.
     */
    virtual double getAvgTime();

    /**
     * Returns the number of calls to start since last reset().
     */
    int getNumCycles() const;

    /**
     * Returns the lowest duration between a start() and an end() call in micro seconds.
     */
    double getMinDuration() const;

    /**
     * Returns the highest duration between a start() and an end() call in micro seconds.
     */
    double getMaxDuration() const;

    /**
     * Resets the statistics (total, average, min, and max durations).
     */
    virtual void reset();

    /**
     * Get a string based on the current date and time of the day.
     * Buffer must be of sufficient length.
     * Format YYYY.MM.DD.HH.MM.SS
     */
    static std::string getDateTimeString();

    /**
     * Get a string based on the current date.
     * Buffer must be of sufficient length.
     * Format YYYY.MM.DD
     */
    //static std::string getDateString();

    /**
     * Get a string based on the current time of the day.
     * Buffer must be of sufficient length.
     * Format HH.MM.SS
     */
    //static getTimeOfTheDayString(char* buffer, int bufSize);

protected:
    /**
     * Time of last call to #start or #reset.
     */
    std::chrono::steady_clock::time_point t;

    /**
     * The accumulated elapsed time.
     */
    std::chrono::duration<double> totalDuration;

    /**
     * Number of calls to start since last #reset() call.
     */
    int numCycles;

    /**
     * Last recorded duration recorded at #end() call.
     */
    std::chrono::duration<double> lastDuration;

    /**
     * The lowest duration between a #start() and an #end() call in micro seconds.
     */
    double minDuration;

    /**
     * The highest duration between a #start() and an #end() call in micro seconds.
     */
    double maxDuration;

    /**
     * True if the timer has a start value.
     */
    bool running;
};

}

#endif
