cmake_minimum_required(VERSION 3.16)

project(pimp)

add_subdirectory(external/libwav)
add_executable(${PROJECT_NAME} src/main.c)
target_link_libraries(${PROJECT_NAME} wav::wav m)