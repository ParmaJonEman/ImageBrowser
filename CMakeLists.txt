# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
find_package(OpenCV REQUIRED PATHS "C:/OpenCV-MinGW-Build-OpenCV-4.5.5-x64")
include_directories( ${OpenCV_INCLUDE_DIRS} )
project ("browser")
add_executable (browser "browser.cpp" browser.h)
target_link_libraries(browser ${OpenCV_LIBS})
if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET browser PROPERTY CXX_STANDARD 20)
endif()

