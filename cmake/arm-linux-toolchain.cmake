message("using arm-linux-toolchain")

# use qemu to run tests when cross-compiling
set(CMAKE_CROSSCOMPILING_EMULATOR qemu-arm -L/usr/arm-linux-gnueabihf)

set(CMAKE_SYSTEM_NAME "Linux")
set(CMAKE_SYSTEM_PROCESSOR "arm")

set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

set(triple arm-linux-gnueabihf)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER_TARGET ${triple})

# for Ne10
set(NE10_LINUX_TARGET_ARCH "armv7")
set(GNULINUX_PLATFORM ON)