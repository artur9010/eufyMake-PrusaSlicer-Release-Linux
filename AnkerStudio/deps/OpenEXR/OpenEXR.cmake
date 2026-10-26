# 2.5.5 relies on <cstdint> arriving transitively, which GCC 13+ headers no longer do.
# cl.exe has no -include (it would treat "cstdint" as a source file).
set(_openexr_cxx_flags "")
if (NOT MSVC)
    set(_openexr_cxx_flags "-DCMAKE_CXX_FLAGS:STRING=-include cstdint")
endif ()

prusaslicer_add_cmake_project(OpenEXR
    # GIT_REPOSITORY https://github.com/openexr/openexr.git
    URL https://github.com/AcademySoftwareFoundation/openexr/archive/refs/tags/v2.5.5.zip
    URL_HASH SHA256=0307a3d7e1fa1e77e9d84d7e9a8694583fbbbfd50bdc6884e2c96b8ef6b902de
    DEPENDS ${ZLIB_PKG}
    GIT_TAG v2.5.5
    CMAKE_ARGS
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DBUILD_TESTING=OFF 
        -DPYILMBASE_ENABLE:BOOL=OFF 
        -DOPENEXR_VIEWERS_ENABLE:BOOL=OFF
        -DOPENEXR_BUILD_UTILS:BOOL=OFF
        ${_openexr_cxx_flags}
)

if (MSVC)
    add_debug_dep(dep_OpenEXR)
endif ()