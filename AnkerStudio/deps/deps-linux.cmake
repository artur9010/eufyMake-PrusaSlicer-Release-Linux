set(DEP_CMAKE_OPTS "-DCMAKE_POSITION_INDEPENDENT_CODE=ON")

include("deps-unix-common.cmake")

# Use the system libcurl (and with it the system OpenSSL 3) instead of building curl 7.75
# against the bundled, end-of-life OpenSSL 1.1.0. Every current distro and the Flatpak
# GNOME SDK ship both. dep_OpenSSL is only built as a dependency of dep_CURL.
find_package(CURL QUIET)
if (NOT CURL_FOUND)
    message(WARNING "No CURL dev package found in system, building static curl and OpenSSL 1.1. You should install the system package.")
endif ()

# Some Linuxes may have very old libpng, so it's best to bundle it instead of relying on the system version.
# find_package(PNG QUIET)
# if (NOT PNG_FOUND)
#     message(WARNING "No PNG dev package found in system, building static library. You should install the system package.")
# endif ()
