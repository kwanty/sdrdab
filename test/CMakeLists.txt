add_subdirectory(gtest)

ADD_DEFINITIONS(-std=c++98)
ADD_DEFINITIONS(-Werror)
ADD_DEFINITIONS(-DGOOGLE_UNIT_TEST)

cmake_minimum_required(VERSION 2.8.12.2)

enable_testing()
include_directories(
  ${gtest_SOURCE_DIR}/include
  ${gtest_SOURCE_DIR})
link_directories(${gtest_BINARY_DIR}/src)
file( GLOB SDRDAB_UNITTESTS_SOURCES "*.cc" )

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
add_executable("sdrdab-ut" ${SDRDAB_UNITTESTS_SOURCES})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB_PKG glib-2.0)
if (GLIB_PKG_FOUND)
    message(Found glib-2.0)
    include_directories(${GLIB_PKG_INCLUDE_DIRS})
ENDIF()

target_include_directories("sdrdab-ut" PUBLIC ".")
target_include_directories("sdrdab-ut" "PRIVATE" "${SDRDAB_SOURCES}/src/sdrdab/")#TODO
target_include_directories("sdrdab-ut" "PUBLIC" "/usr/include/gstreamer-1.0/") #TODO
target_include_directories("sdrdab-ut" "PUBLIC" "/usr/lib/gstreamer-1.0/include") #TODO ARCH_COMPAT
target_include_directories("sdrdab-ut" "PUBLIC" "/usr/local/lib/gstreamer-1.0/include/") #TODO MAC_COMPAT
target_include_directories("sdrdab-ut" "PUBLIC" "/usr/lib/x86_64-linux-gnu/gstreamer-1.0/include/") #TODO UBUNTU_COMPAT
target_include_directories("sdrdab-ut" "PUBLIC" "/usr/local/include/gstreamer-1.0") #TODO MAC_COMPAT
target_include_directories("sdrdab-ut" "PUBLIC" "/usr/lib/x86_64-linux-gnu/gstreamer-1.0/include/") #TODO UBUNTU_COMPAT

target_link_libraries("sdrdab-ut" "gstreamer-1.0")
target_link_libraries("sdrdab-ut" "gobject-2.0")
target_link_libraries("sdrdab-ut" "gstapp-1.0")

target_link_libraries(sdrdab-ut sdrdab)
target_link_libraries(sdrdab-ut gtest_main)
