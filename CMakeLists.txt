cmake_minimum_required(VERSION 3.16)

project(pimp)

# main library
add_library(${PROJECT_NAME} src/pimp.c src/fft.c)
include_directories(src)
link_libraries(m)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PUBLIC /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic)
endif()


# build options
if(NOT DEFINED FFTLIB)
    set(FFTLIB "" CACHE STRING "select fft lib")
endif()

if(NOT DEFINED BUILD_TESTS)
    set(BUILD_TESTS OFF CACHE BOOL "build tests")
endif()

if(NOT DEFINED DTYPE)
    set(DTYPE "double" CACHE STRING "compile for floats or doubles")
endif()

if(NOT DEFINED TEST_FFTS)
    set(TEST_FFTS ON CACHE BOOL "build FFT methods")
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
    add_subdirectory(external/Ne10)
    target_compile_definitions(${PROJECT_NAME} PUBLIC PIMP_WITH_NE10)
    target_compile_definitions(NE10
        PUBLIC  GNULINUX_PLATFORM=ON
        PUBLIC NE10_LINUX_TARGET_ARCH=aarch64)
else()
    message(STATUS "compiling witout FFT (for that set FFTLIB)")
    set(TEST_FFTS OFF)
endif()

if(DTYPE STREQUAL "double" OR DTYPE STREQUAL "float")
    message(STATUS "compiling with DTYPE=${DTYPE}")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DTYPE=${DTYPE})
else()
    message(FATAL_ERROR "DTYPE=${DTYPE} but must be float or double")
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
    target_link_libraries(${TEST_AUDIOBUF} pimp wav::wav m unity)
    add_test(${TEST_AUDIOBUF} ./${TEST_AUDIOBUF} .)

    set(TEST_TFEST test_TFest)
    add_executable(${TEST_TFEST} tests/${TEST_TFEST}.c)
    target_include_directories(${TEST_TFEST}
        PRIVATE src
        PRIVATE tests)
    target_link_libraries(${TEST_TFEST} pimp wav::wav m unity)
    add_test(${TEST_TFEST} ./${TEST_TFEST} .)

    set(TEST_LMSFILTER test_LMSFilter)
    add_executable(${TEST_LMSFILTER} tests/${TEST_LMSFILTER}.c)
    target_include_directories(${TEST_LMSFILTER}
        PRIVATE src
        PRIVATE tests)
    target_link_libraries(${TEST_LMSFILTER} pimp wav::wav m unity)
    add_test(${TEST_LMSFILTER} ./${TEST_LMSFILTER} .)

    set(TEST_RLSFILTER test_RLSFilter)
    add_executable(${TEST_RLSFILTER} tests/${TEST_RLSFILTER}.c)
    target_include_directories(${TEST_RLSFILTER}
        PRIVATE src
        PRIVATE tests)
    target_link_libraries(${TEST_RLSFILTER} pimp wav::wav m unity)
    add_test(${TEST_RLSFILTER} ./${TEST_RLSFILTER} .)

    if(TEST_FFTS)
        set(TEST_BLOCKLMSFILTER test_BlockLMSFilter)
        add_executable(${TEST_BLOCKLMSFILTER} tests/${TEST_BLOCKLMSFILTER}.c)
        target_include_directories(${TEST_BLOCKLMSFILTER}
            PRIVATE src
            PRIVATE tests)
        target_link_libraries(${TEST_BLOCKLMSFILTER} pimp wav::wav m unity)
        add_test(${TEST_BLOCKLMSFILTER} ./${TEST_BLOCKLMSFILTER} .)

        set(TEST_FFT test_fft)
        add_executable(${TEST_FFT} tests/test_fft.c)
        target_include_directories(${TEST_FFT}
            PRIVATE src
            PRIVATE tests)
        target_link_libraries(${TEST_FFT} pimp m unity)
        add_test(${TEST_FFT} ./${TEST_FFT} .)
        endif()

        # set(TEST_POCKETFFT test_pocketfft)
        # add_executable(${TEST_POCKETFFT} external/pocketfft/ffttest.c)
        # target_link_libraries(${TEST_POCKETFFT} pocketfft m)
        # add_test(${TEST_POCKETFFT} ./${TEST_POCKETFFT} .)
    endif()