message("using arm-linux-toolchain")

set(CMAKE_SYSTEM_NAME "Linux")
set(CMAKE_SYSTEM_PROCESSOR "arm")

set(NE10_LINUX_TARGET_ARCH "armv7")

set(CMAKE_C_COMPILER /usr/local/linaro/arm-bela-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER /usr/local/linaro/arm-bela-linux-gnueabihf/bin/arm-linux-gnueabihf-g++)

set(triple arm-linux-gnueabihf)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER_TARGET ${triple})

# for Ne10
set(GNULINUX_PLATFORM ON)