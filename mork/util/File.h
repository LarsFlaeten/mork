#ifndef _MORK_FILE_H_
#define _MORK_FILE_H_

#include <chrono>
#include <string>


namespace mork {

    std::chrono::system_clock::time_point getLastModifiedTime(const std::string& path);





}


#endif
