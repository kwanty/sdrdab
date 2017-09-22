cmake_minimum_required(VERSION 2.8.12.2)
project(DAB-Decoder-2016)


include("cmake/Modules/FindSSE.cmake")
if(CXX_SSE3_FOUND)
    add_definitions( -DSSE3_FOUND=1 )
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse3 -O3" )
endif()

# Not used anymore, was used for testing cross-compilation
#if(NOT CROSS_COMPILE)
#    include("cmake/Modules/FindSSE.cmake")
#    add_definitions( -march=native )
#
#    if(CXX_SSE3_FOUND)
#        add_definitions( -DSSE3_FOUND=1 )
#        SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse3 -O3" )
#    endif()
#endif()
#
#if(CROSS_COMPILE)
#    if(SBC_RPI3)
##        add_definitions( -march=armv8-a )
#        add_definitions( -march=native )
#        add_definitions( -O0 )
#        add_definitions( -mno-sse )
#    endif()
#endif()

if(PROFILING)
    add_definitions(-pg)
    set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

add_subdirectory(src/sdrdab)
add_subdirectory(src/sdrdab-cli)

if(MAKE_TESTS)
    add_subdirectory(test)
endif()
