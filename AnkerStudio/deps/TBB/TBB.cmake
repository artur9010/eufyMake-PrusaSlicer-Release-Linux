prusaslicer_add_cmake_project(
    TBB
    URL "https://github.com/oneapi-src/oneTBB/archive/refs/tags/v2021.5.0.zip"
    URL_HASH SHA256=83ea786c964a384dd72534f9854b419716f412f9d43c0be88d41874763e7bb47
    CMAKE_ARGS          
        -DTBB_BUILD_SHARED=OFF
        -DTBB_TEST=OFF
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_DEBUG_POSTFIX=_debug
        # TBB always compiles with -flto on GCC; without fat objects the static archive has
        # no usable symbol index and linking the app fails with undefined tbb::detail::r1 symbols.
        "-DCMAKE_CXX_FLAGS:STRING=-ffat-lto-objects"
)

if (MSVC)
    add_debug_dep(dep_TBB)
endif ()


