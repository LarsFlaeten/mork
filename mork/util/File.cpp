#include "mork/util/File.h"
#include "mork/core/Log.h"

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#warn THIS CODE IS NOT TESTED ON WINDOWS
#endif


namespace mork {


    std::chrono::system_clock::time_point getLastModifiedTime(const std::string& filename) {

		struct stat result;
		if(stat(filename.c_str(), &result)==0)
		{
    		auto mod_time = result.st_mtime;
            return std::chrono::system_clock::from_time_t(mod_time);    		
		} else {
			error_logger("File ", filename, " was not found");
			throw std::runtime_error(error_logger.last());
	

		}


    }



}
