#ifndef _MORK_TIME_H_
#define _MORK_TIME_H_

#include <iostream>
#include <chrono>


namespace mork {

    std::ostream& operator << (std::ostream& os, const std::chrono::system_clock::time_point& t);

}

#endif

