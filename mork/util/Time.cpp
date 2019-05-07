#include "mork/util/Time.h"


#include <iomanip>

namespace mork {

    std::ostream& operator << (std::ostream& os, const std::chrono::system_clock::time_point& t) {
 
        auto tt = std::chrono::system_clock::to_time_t(t);
        // Format as ISO 8601
        os << std::put_time(std::localtime(&tt), "%Y-%m-%dT%T%z");
        return os;


    }


}
