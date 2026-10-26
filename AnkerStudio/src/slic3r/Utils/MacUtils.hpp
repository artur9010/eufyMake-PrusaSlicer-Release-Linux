#ifndef slic3r_MacUtils_hpp_
#define slic3r_MacUtils_hpp_

#include <string>

namespace Slic3r {

#ifdef __APPLE__
// implemented in MacUtils.mm
std::string get_downloads_path_mac();
#endif //__APPLE__

} //namespace Slic3r
#endif
