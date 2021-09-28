cmake_minimum_required(VERSION 3.16)

project(pimp)

# main library
add_library(${PROJECT_NAME} src/pimp.c src/fft.c)
include_directories(src)
link_libraries(m)

target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic)

# build options

if(NOT DEFINED PIMP_FFTLIB)
    set(PIMP_FFTLIB "none" CACHE STRING "fft library")
endif()

if(NOT DEFINED PIMP_BUILD_TESTS)
    set(PIMP_BUILD_TESTS ON CACHE BOOL "build tests")
endif()

if(NOT DEFINED PIMP_TEST_FFTS)
    set(PIMP_TEST_FFTS ON CACHE BOOL "build fft tests")
endif()

if(NOT DEFINED PIMP_USE_DOUBLE)
    set(PIMP_USE_DOUBLE ON CACHE BOOL "compile for float or double")
endif()

# compile time definitions
if(PIMP_FFTLIB STREQUAL "pocketfft")
    message(STATUS "compiling with PIMP_FFTLIB=${PIMP_FFTLIB}")
    target_compile_definitions(${PROJECT_NAME} PUBLIC PIMP_WITH_POCKETFFT)
    add_library(pocketfft STATIC external/pocketfft/pocketfft.c)
    target_link_libraries(${PROJECT_NAME} pocketfft)
    target_include_directories(${PROJECT_NAME} PUBLIC external/pocketfft)
    set(PIMP_USE_DOUBLE ON)
elseif(PIMP_FFTLIB STREQUAL "ne10")
    message(STATUS "compiling with PIMP_FFTLIB=${PIMP_FFTLIB}")
    target_compile_definitions(${PROJECT_NAME} PUBLIC PIMP_WITH_NE10)
    add_subdirectory(external/Ne10)
    target_link_libraries(${PROJECT_NAME} NE10)
    set(PIMP_USE_DOUBLE OFF)
else()
    message(STATUS "compiling witout FFT (for that set PIMP_FFTLIB)")
    set(PIMP_TEST_FFTS OFF)
endif()

if(NOT PIMP_USE_DOUBLE)
target_compile_definitions(${PROJECT_NAME} PUBLIC PIMP_USE_DOUBLE=0)
    message(STATUS "compiling with pfloat == float")
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC PIMP_USE_DOUBLE=1)
    message(STATUS "compiling with pfloat == double")
endif()


# tests
if(PIMP_BUILD_TESTS)
    enable_testing()

    add_subdirectory(external/Unity)
    target_compile_definitions(unity PUBLIC -DUNITY_INCLUDE_DOUBLE)

    add_subdirectory(external/libwav)

    link_libraries(unity wav::wav ${PROJECT_NAME})

    set(TEST_AUDIOBUF test_AudioBuf)
    add_executable(${TEST_AUDIOBUF} tests/${TEST_AUDIOBUF}.c)
    target_include_directories(${TEST_AUDIOBUF}
        PRIVATE src
        PRIVATE tests)
    target_link_libraries(${TEST_AUDIOBUF} ${PROJECT_NAME} wav::wav)
    add_test(${TEST_AUDIOBUF} ./${TEST_AUDIOBUF} .)

    set(TEST_TFEST test_TFest)
    add_executable(${TEST_TFEST} tests/${TEST_TFEST}.c)
    target_include_directories(${TEST_TFEST}
        PRIVATE src
        PRIVATE tests)
    target_link_libraries(${TEST_TFEST} ${PROJECT_NAME} wav::wav)
    add_test(${TEST_TFEST} ./${TEST_TFEST} .)

    set(TEST_LMSFILTER test_LMSFilter)
    add_executable(${TEST_LMSFILTER} tests/${TEST_LMSFILTER}.c)
    target_include_directories(${TEST_LMSFILTER}
        PRIVATE src
        PRIVATE tests)
    target_link_libraries(${TEST_LMSFILTER} ${PROJECT_NAME} wav::wav)
    add_test(${TEST_LMSFILTER} ./${TEST_LMSFILTER} .)

    set(TEST_RLSFILTER test_RLSFilter)
    add_executable(${TEST_RLSFILTER} tests/${TEST_RLSFILTER}.c)
    target_include_directories(${TEST_RLSFILTER}
        PRIVATE src
        PRIVATE tests)
    target_link_libraries(${TEST_RLSFILTER} ${PROJECT_NAME} wav::wav)
    add_test(${TEST_RLSFILTER} ./${TEST_RLSFILTER} .)

    if(PIMP_TEST_FFTS)
        set(TEST_BLOCKLMSFILTER test_BlockLMSFilter)
        add_executable(${TEST_BLOCKLMSFILTER} tests/${TEST_BLOCKLMSFILTER}.c)
        target_include_directories(${TEST_BLOCKLMSFILTER}
            PRIVATE src
            PRIVATE tests)
        target_link_libraries(${TEST_BLOCKLMSFILTER} ${PROJECT_NAME} wav::wav)
        add_test(${TEST_BLOCKLMSFILTER} ./${TEST_BLOCKLMSFILTER} .)

        set(TEST_FFT test_fft)
        add_executable(${TEST_FFT} tests/test_fft.c)
        target_include_directories(${TEST_FFT}
            PRIVATE src
            PRIVATE tests)
        target_link_libraries(${TEST_FFT} ${PROJECT_NAME})
        add_test(${TEST_FFT} ./${TEST_FFT} .)
        endif()

        if (PIMP_WITH_POCKETFFT)
            set(TEST_POCKETFFT test_pocketfft)
            add_executable(${TEST_POCKETFFT} external/pocketfft/ffttest.c)
            target_link_libraries(${TEST_POCKETFFT} pocketfft)
            add_test(${TEST_POCKETFFT} ./${TEST_POCKETFFT} .)
        endif()
    endif()