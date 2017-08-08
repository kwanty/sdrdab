cmake_minimum_required(VERSION 2.8.12.2)
project(DAB-Decoder-2016)
include("cmake/Modules/FindSSE.cmake")
if(CXX_SSE3_FOUND)
    add_definitions( -DSSE3_FOUND=1 )
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse3 -O3" )
endif()

if(PROFILING)
    add_definitions(-pg)
    set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

add_subdirectory(src/rtlsdr)
add_subdirectory(src/rscode)
add_subdirectory(src/sdrdab)
add_subdirectory(src/sdrdab-cli)

# comment next line if you don't want to build Qt GUI
add_subdirectory(src/sdrdab-gui)

if(MAKE_TESTS)
    add_subdirectory(test)
endif()
