ADD_DEFINITIONS(-std=c++98)

cmake_minimum_required(VERSION 2.8.12.2)
file( GLOB CLI_SOURCES "*.cc" )
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
add_executable("sdrdab-cli" ${CLI_SOURCES})

target_include_directories("sdrdab" "PUBLIC" "../sdrdab/")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB_PKG glib-2.0)
if (GLIB_PKG_FOUND)
    message(Found glib-2.0)
include_directories(${GLIB_PKG_INCLUDE_DIRS})
ENDIF()

target_link_libraries("sdrdab-cli" "sdrdab")
target_link_libraries("sdrdab-cli" "gstreamer-1.0")
target_link_libraries("sdrdab-cli" "gobject-2.0")
target_link_libraries("sdrdab-cli" "gstapp-1.0")
target_link_libraries("sdrdab-cli" "readline")

if(APPLE)
    ADD_LIBRARY(libargp STATIC IMPORTED)
    SET_TARGET_PROPERTIES(libargp PROPERTIES
            IMPORTED_LOCATION /usr/local/opt/argp-standalone/lib/libargp.a)
    TARGET_LINK_LIBRARIES("sdrdab-cli" "libargp")

    set(CMAKE_LIBRARY_PATH /usr/local/lib/ ${CMAKE_LIBRARY_PATH})   #important, ignore unused prompt
    set(CMAKE_CXX_FLAGS "-L/usr/local/opt/readline/lib")            #important, ignore unused prompt
endif()

add_dependencies("sdrdab-cli" "sdrdab")
