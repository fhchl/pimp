cmake_minimum_required(VERSION 3.16)

project(pimp)
enable_testing()

add_subdirectory(external/libwav)

add_subdirectory(external/Unity)
target_compile_definitions(unity PUBLIC -DUNITY_INCLUDE_DOUBLE)

add_executable(${PROJECT_NAME} src/pimp.c src/main.c)
target_link_libraries(${PROJECT_NAME} wav::wav m)

set(TEST_AUDIOBUF test_AudioBuf)
add_executable(${TEST_AUDIOBUF} src/pimp.c tests/test_AudioBuf.c)
target_include_directories(${TEST_AUDIOBUF} PRIVATE src)
target_include_directories(${TEST_AUDIOBUF} PRIVATE tests)
target_link_libraries(${TEST_AUDIOBUF} wav::wav m unity)
add_test(${TEST_AUDIOBUF} ./${TEST_AUDIOBUF} .)

set(TEST_LMSFILTER test_LMSFilter)
add_executable(${TEST_LMSFILTER} src/pimp.c tests/test_LMSFilter.c)
target_include_directories(${TEST_LMSFILTER} PRIVATE src)
target_include_directories(${TEST_LMSFILTER} PRIVATE tests)
target_link_libraries(${TEST_LMSFILTER} wav::wav m unity)
add_test(${TEST_LMSFILTER} ./${TEST_LMSFILTER} .)
