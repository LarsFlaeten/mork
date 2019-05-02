#ifndef _MORK_UTIL_H_
#define _MORK_UTIL_H_

#include <string>
#include "mork/math/vec3.h"

namespace mork {

std::string random_string( size_t length );

// COnverts a string on the form ##, ##, ## to a vec3d.
// Will throw exception on any parse error
vec3d string2vec3d(const std::string& str);



}
#endif
