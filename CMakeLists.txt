cmake_minimum_required(VERSION 3.16)

project(pimp)
enable_testing()

add_subdirectory(external/libwav)

include_directories(tests) # to include unity_config.h
add_definitions(-DUNITY_INCLUDE_CONFIG_H)
add_subdirectory(external/Unity)

add_executable(${PROJECT_NAME} src/pimp.c src/main.c)
target_link_libraries(${PROJECT_NAME} wav::wav m)

set(TEST1 test1)
add_executable(${TEST1} src/pimp.c tests/test_audiobuf.c)
target_include_directories(${TEST1} PRIVATE src)
target_include_directories(${TEST1} PRIVATE tests)
target_link_libraries(${TEST1} wav::wav m unity)
add_test(${TEST1} ./${TEST1} .)
