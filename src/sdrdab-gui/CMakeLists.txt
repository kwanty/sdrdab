file( GLOB_RECURSE SDRDAB_GUI_SOURCES "*.cc" )
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB_PKG glib-2.0)
if (GLIB_PKG_FOUND)
    message(Found glib-2.0)
include_directories(${GLIB_PKG_INCLUDE_DIRS})
ENDIF()



find_package(Qt5Widgets)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
add_executable("sdrdab-gui" ${SDRDAB_GUI_SOURCES})
target_link_libraries("sdrdab-gui" Qt5::Widgets)

target_link_libraries("sdrdab-gui" sdrdab)
