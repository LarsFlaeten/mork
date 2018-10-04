#ifndef _MORK_LOG_H_
#define _MORK_LOG_H_

#include <iostream>
#include <mutex>
#include <string>
#include <fstream>
#include <string_view>
#include <iomanip>

#include "Timer.h"

namespace mork
{

// Simple logging interface, based on:
// https://stackoverflow.com/questions/48561227/c-logging-to-console-and-log-file-simultaneously

    inline bool print(std::ostream& out) {
        return !!(out << std::endl);
    }

    template<typename T>
    bool print(std::ostream& out, T&& value)
    {
        return !!(out << std::forward<T>(value) << std::endl);
    }

    template<typename First, typename ... Rest>
    bool print(std::ostream& out, First&& first, Rest&& ... rest)
    {
        return !!(out << std::forward<First>(first)) && print(out, std::forward<Rest>(rest)...);
    }

    inline std::mutex logger_mtx;


	class log_stream {
    public:
        log_stream(const std::string& str, std::ostream& ifile)
            : name(str)
            , file(ifile)
        {
            std::string s{ "[" };
            name = s + name + "] ";
        }

        template <typename... Args>
        bool operator() (Args&&... args) {
            std::string dt = Timer::getDateTimeString();

            bool OK = print(file, dt, " ", name, " ", std::forward<Args>(args)...);
            {
                std::lock_guard<std::mutex> lck(logger_mtx);
                print(std::cout, name, std::forward<Args>(args)...);
                if (!OK) {
                    print(std::cout, name, "-- Error writing to log file. --");
                }
            }
            return OK;
        }

    private:
        std::string name;
        std::ostream& file;
    };

    inline std::ofstream info_out("info.log");
    inline log_stream info_logger("INFO", info_out);
    inline std::ofstream warn_out("warn.log");
    inline log_stream warn_logger("WARNING", warn_out);
    inline std::ofstream error_out("info.log");
    inline log_stream error_logger("ERROR", error_out);
    inline std::ofstream debug_out("debug.log");
    inline log_stream debug_logger("DEBUG", info_out);





}
#endif
