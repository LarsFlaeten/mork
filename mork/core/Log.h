#ifndef _MORK_LOG_H_
#define _MORK_LOG_H_

#include <iostream>
#include <mutex>
#include <string>
#include <fstream>
#include <sstream>
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
        return !!(out << std::forward<T>(value));
    }

    template<typename First, typename ... Rest>
    bool print(std::ostream& out, First&& first, Rest&& ... rest)
    {
        return !!(out << std::forward<First>(first)) && print(out, std::forward<Rest>(rest)...);
    }


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
                 
            std::lock_guard<std::mutex> lck(logger_mtx);
            
            std::string dt = Timer::getDateTimeString();

            std::ostringstream oss;
 
            bool OK = print(oss, dt, " ", name, " ", std::forward<Args>(args)...);
            file << oss.str() << std::endl;
            //{
            //print(oss, dt, " ", name, " ", std::forward<Args>(args)...);
            _last = oss.str();
            //    std::cout << oss.str();
            if (!OK) {
                print(std::cerr, name, "-- Error writing to log file. --");
                print(std::cerr, oss.str());
            }
            //}
            return OK;
        }

        const std::string& last() const {return _last;}

    private:
 
        std::mutex logger_mtx;

        std::string _last;
        std::string name;
        std::ostream& file;
    };

    //inline std::ofstream info_out("info.log");
    inline log_stream info_logger("INFO", std::cout);
    //inline std::ofstream warn_out("warn.log");
    inline log_stream warn_logger("WARNING", std::cout);
    //inline std::ofstream error_out("error.log");
    inline log_stream error_logger("ERROR", std::cerr);
    //inline std::ofstream debug_out("debug.log");
    inline log_stream debug_logger("DEBUG", std::cout);





}
#endif
