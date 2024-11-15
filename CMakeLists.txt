cmake_minimum_required(VERSION 3.27)
project(OptiFOL)

set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)
include_directories(${GTKMM_INCLUDE_DIRS})
add_definitions(${GTKMM_CFLAGS_OTHER})

add_executable(OptiFOL main.cpp)
target_link_libraries(OptiFOL ${GTKMM_LIBRARIES})

