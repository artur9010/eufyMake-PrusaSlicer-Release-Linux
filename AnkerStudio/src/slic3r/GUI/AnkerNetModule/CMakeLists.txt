# Native, open-source replacement for the proprietary libAnkerNet plugin.
# Built as its own shared library so it satisfies AnkerNetModuleManager's
# boost::dll plugin-loading contract exactly like the real dylib would.

add_library(AnkerNetNative SHARED
    AnkerNetNative.hpp
    AnkerNetNative.cpp
    DeviceObjectNative.hpp
    AnkerMqtt.hpp
    AnkerMqtt.cpp
    AnkerPppp.hpp
    AnkerPppp.cpp
    AnkerH264Decoder.hpp
)

# libcurl (HTTP device list), boost (JSON/filesystem), OpenSSL (TLS + AES for MQTT).
target_link_libraries(AnkerNetNative PRIVATE libcurl boost_libs libopenssl)
if (WIN32)
    # Winsock for the MQTT (TCP) and PPPP (UDP) sockets.
    target_link_libraries(AnkerNetNative PRIVATE ws2_32)
endif ()

# H.264 decode of the camera stream: VideoToolbox on macOS, FFmpeg elsewhere.
if (APPLE)
    target_sources(AnkerNetNative PRIVATE AnkerH264Decoder.mm)
    target_link_libraries(AnkerNetNative PRIVATE
        "-framework VideoToolbox"
        "-framework CoreMedia"
        "-framework CoreVideo"
        "-framework CoreFoundation")
elseif (WIN32)
    # No pkg-config on Windows: locate the FFmpeg development files (headers + import
    # libraries, e.g. from vcpkg) through CMAKE_PREFIX_PATH. The DLLs are shipped
    # next to the executable.
    find_path(FFMPEG_INCLUDE_DIR libavcodec/avcodec.h REQUIRED)
    find_library(AVCODEC_LIBRARY avcodec REQUIRED)
    find_library(AVUTIL_LIBRARY avutil REQUIRED)
    find_library(SWSCALE_LIBRARY swscale REQUIRED)
    target_sources(AnkerNetNative PRIVATE AnkerH264Decoder_ffmpeg.cpp)
    target_include_directories(AnkerNetNative PRIVATE ${FFMPEG_INCLUDE_DIR})
    target_link_libraries(AnkerNetNative PRIVATE
        ${AVCODEC_LIBRARY} ${AVUTIL_LIBRARY} ${SWSCALE_LIBRARY})
else ()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET libavcodec libavutil libswscale)
    target_sources(AnkerNetNative PRIVATE AnkerH264Decoder_ffmpeg.cpp)
    target_link_libraries(AnkerNetNative PRIVATE PkgConfig::FFMPEG)
endif ()

if (APPLE)
    set(_anker_net_prefix "lib")
    set(_anker_net_suffix ".dylib")
elseif (WIN32)
    # AnkerNetModuleManager looks for "AnkerNet.dll" on Windows.
    set(_anker_net_prefix "")
    set(_anker_net_suffix ".dll")
else ()
    set(_anker_net_prefix "lib")
    set(_anker_net_suffix ".so")
endif ()

set_target_properties(AnkerNetNative PROPERTIES
    OUTPUT_NAME "AnkerNet"
    PREFIX "${_anker_net_prefix}"
    SUFFIX "${_anker_net_suffix}"
    # Debug-mode AnkerNetModuleManager::GetDefaultDir() resolves to the directory
    # containing the running executable (boost::dll::program_location().parent_path()).
    # The eufyStudio target (defined in src/CMakeLists.txt) lands at
    # ${CMAKE_BINARY_DIR}/src/<Config>/ under the Xcode generator's default
    # per-source-directory output layout; match it explicitly here since this
    # target lives in a nested subdirectory and would otherwise default to its
    # own (wrong) nested output path.
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src"
)

if (NOT APPLE AND NOT WIN32)
    # AnkerNetModuleManager looks for the bundled plugin in <prefix>/lib next to <prefix>/bin.
    install(TARGETS AnkerNetNative LIBRARY DESTINATION lib)
endif ()
