cmake_minimum_required(VERSION 3.10)

project(MyProgram C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address,undefined")

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
 
add_executable(app main.c manager.h manager.c)

target_link_libraries(app ${CURSES_LIBRARIES})