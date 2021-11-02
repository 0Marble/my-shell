cmake_minimum_required(VERSION 3.12)
project(my_shell)

file(GLOB SOURCES src/*.c include/*.h)
add_executable(my_shell ${SOURCES})

set(INCLUDES ${INCLUDES} include/)

target_include_directories(my_shell PUBLIC ${INCLUDES})


