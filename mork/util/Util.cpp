#include <mork/util/Util.h>

#include <algorithm>

namespace mork {

    std::string random_string( size_t length )
    {
        auto randchar = []() -> char
        {
            const char charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
                const size_t max_index = (sizeof(charset) - 1);
                return charset[ rand() % max_index ];
        };
        
        std::string str(length,0);
        std::generate_n( str.begin(), length, randchar );
        return str;
    }

    vec3d string2vec3d(const std::string& str) {
        // Assum comma-separated list of thrww numbers:
        size_t n = std::count(str.begin(), str.end(), ',');        
        if(n != 2)
            throw std::invalid_argument("Expected string with three comma separated values, got [" + str + "]");
        std::string substr;
        std::stringstream sstr(str);
       
        
        getline(sstr, substr, ',');
        double x = std::stod(substr);

        getline(sstr, substr, ',');
        double y = std::stod(substr);

        getline(sstr, substr, ',');
        double z = std::stod(substr);
        return vec3d(x, y, z);

    }


}
