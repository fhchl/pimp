cmake_minimum_required(VERSION 3.16)

project(pimp)
enable_testing()

add_subdirectory(external/libwav)

add_subdirectory(external/Unity)
target_compile_definitions(unity PUBLIC -DUNITY_INCLUDE_DOUBLE)

add_library(pocketfft STATIC external/pocketfft/pocketfft.c)

add_library(${PROJECT_NAME} src/pimp.c)
target_link_libraries(${PROJECT_NAME} m)

set(TEST_AUDIOBUF test_AudioBuf)
add_executable(${TEST_AUDIOBUF} src/pimp.c tests/${TEST_AUDIOBUF}.c)
target_include_directories(${TEST_AUDIOBUF}
    PRIVATE src
    PRIVATE tests)
target_link_libraries(${TEST_AUDIOBUF} wav::wav m unity)
add_test(${TEST_AUDIOBUF} ./${TEST_AUDIOBUF} .)

set(TEST_TFEST test_TFest)
add_executable(${TEST_TFEST} src/pimp.c tests/${TEST_TFEST}.c)
target_include_directories(${TEST_TFEST}
    PRIVATE src
    PRIVATE tests)
target_link_libraries(${TEST_TFEST} wav::wav m unity)
add_test(${TEST_TFEST} ./${TEST_TFEST} .)

set(TEST_LMSFILTER test_LMSFilter)
add_executable(${TEST_LMSFILTER} src/pimp.c tests/${TEST_LMSFILTER}.c)
target_include_directories(${TEST_LMSFILTER}
    PRIVATE src
    PRIVATE tests)
target_link_libraries(${TEST_LMSFILTER} wav::wav m unity)
add_test(${TEST_LMSFILTER} ./${TEST_LMSFILTER} .)

set(TEST_RLSFILTER test_RLSFilter)
add_executable(${TEST_RLSFILTER} src/pimp.c tests/${TEST_RLSFILTER}.c)
target_include_directories(${TEST_RLSFILTER}
    PRIVATE src
    PRIVATE tests)
target_link_libraries(${TEST_RLSFILTER} wav::wav m unity)
add_test(${TEST_RLSFILTER} ./${TEST_RLSFILTER} .)

# set(TEST_POCKETFFT test_pocketfft)
# add_executable(${TEST_POCKETFFT} external/pocketfft/ffttest.c)
# target_include_directories(${TEST_POCKETFFT} PRIVATE external/pocketfft)
# target_link_libraries(${TEST_POCKETFFT} pocketfft m)
# add_test(${TEST_POCKETFFT} ./${TEST_POCKETFFT} .)


