cmake_minimum_required(VERSION 3.16)

project(pimp)

# main library
add_library(${PROJECT_NAME} src/pimp.c src/fft.c)
include_directories(src)
link_libraries(m)

target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic)

# build options

if(NOT DEFINED FFTLIB)
    set(FFTLIB "none" CACHE STRING "fft library")
endif()

if(NOT DEFINED BUILD_TESTS)
    set(BUILD_TESTS TRUE CACHE BOOL "build tests")
endif()

if(NOT DEFINED TEST_FFTS)
    set(TEST_FFTS TRUE CACHE BOOL "build fft tests")
endif()


# compile time definitions
if(FFTLIB STREQUAL "pocketfft")
    message(STATUS "compiling with FFTLIB=${FFTLIB}")
    add_library(pocketfft STATIC external/pocketfft/pocketfft.c)
    target_compile_definitions(${PROJECT_NAME} PUBLIC PIMP_WITH_POCKETFFT)
    target_link_libraries(${PROJECT_NAME} pocketfft)
    target_include_directories(${PROJECT_NAME} PUBLIC external/pocketfft)
elseif(FFTLIB STREQUAL "ne10")
    message(STATUS "compiling with FFTLIB=${FFTLIB}")
    target_compile_definitions(${PROJECT_NAME} PUBLIC PIMP_WITH_NE10)
    add_subdirectory(external/Ne10)
    target_link_libraries(${PROJECT_NAME} NE_10)
else()
    message(STATUS "compiling witout FFT (for that set FFTLIB)")
    set(TEST_FFTS OFF)
endif()

if(NOT DEFINED USE_DOUBLE)
    message(STATUS "compiling with pfloat == float")
else()
    message(STATUS "compiling with pfloat == double")
    target_compile_definitions(${PROJECT_NAME} PUBLIC USE_DOUBLE)
endif()

# tests
if(BUILD_TESTS)
    enable_testing()

    add_subdirectory(external/Unity)
    target_compile_definitions(unity PUBLIC -DUNITY_INCLUDE_DOUBLE)

    add_subdirectory(external/libwav)

    set(TEST_AUDIOBUF test_AudioBuf)
    add_executable(${TEST_AUDIOBUF} tests/${TEST_AUDIOBUF}.c)
    target_include_directories(${TEST_AUDIOBUF}
        PRIVATE src
        PRIVATE tests)
    target_link_libraries(${TEST_AUDIOBUF} ${PROJECT_NAME} wav::wav unity)
    add_test(${TEST_AUDIOBUF} ./${TEST_AUDIOBUF} .)

    set(TEST_TFEST test_TFest)
    add_executable(${TEST_TFEST} tests/${TEST_TFEST}.c)
    target_include_directories(${TEST_TFEST}
        PRIVATE src
        PRIVATE tests)
    target_link_libraries(${TEST_TFEST} ${PROJECT_NAME} wav::wav unity)
    add_test(${TEST_TFEST} ./${TEST_TFEST} .)

    set(TEST_LMSFILTER test_LMSFilter)
    add_executable(${TEST_LMSFILTER} tests/${TEST_LMSFILTER}.c)
    target_include_directories(${TEST_LMSFILTER}
        PRIVATE src
        PRIVATE tests)
    target_link_libraries(${TEST_LMSFILTER} ${PROJECT_NAME} wav::wav unity)
    add_test(${TEST_LMSFILTER} ./${TEST_LMSFILTER} .)

    set(TEST_RLSFILTER test_RLSFilter)
    add_executable(${TEST_RLSFILTER} tests/${TEST_RLSFILTER}.c)
    target_include_directories(${TEST_RLSFILTER}
        PRIVATE src
        PRIVATE tests)
    target_link_libraries(${TEST_RLSFILTER} ${PROJECT_NAME} wav::wav unity)
    add_test(${TEST_RLSFILTER} ./${TEST_RLSFILTER} .)

    if(TEST_FFTS)
        set(TEST_BLOCKLMSFILTER test_BlockLMSFilter)
        add_executable(${TEST_BLOCKLMSFILTER} tests/${TEST_BLOCKLMSFILTER}.c)
        target_include_directories(${TEST_BLOCKLMSFILTER}
            PRIVATE src
            PRIVATE tests)
        target_link_libraries(${TEST_BLOCKLMSFILTER} ${PROJECT_NAME} wav::wav unity)
        add_test(${TEST_BLOCKLMSFILTER} ./${TEST_BLOCKLMSFILTER} .)

        set(TEST_FFT test_fft)
        add_executable(${TEST_FFT} tests/test_fft.c)
        target_include_directories(${TEST_FFT}
            PRIVATE src
            PRIVATE tests)
        target_link_libraries(${TEST_FFT} ${PROJECT_NAME} unity)
        add_test(${TEST_FFT} ./${TEST_FFT} .)
        endif()

        if (PIMP_WITH_POCKETFFT)
            set(TEST_POCKETFFT test_pocketfft)
            add_executable(${TEST_POCKETFFT} external/pocketfft/ffttest.c)
            target_link_libraries(${TEST_POCKETFFT} pocketfft m)
            add_test(${TEST_POCKETFFT} ./${TEST_POCKETFFT} .)
        endif()
    endif()