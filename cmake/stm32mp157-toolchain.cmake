set(CMAKE_SYSTEM_NAME    Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(SDK_ROOT    "/home/syuan/STM32MPU_workspace/STM32MP157-DK2_v25.06.11/SDK")
set(SDK_SYSROOT "${SDK_ROOT}/sysroots/cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi")
set(SDK_TOOLS   "${SDK_ROOT}/sysroots/x86_64-ostl_sdk-linux/usr/bin/arm-ostl-linux-gnueabi")
set(TC_PREFIX   "arm-ostl-linux-gnueabi")

set(CMAKE_C_COMPILER   "${SDK_TOOLS}/${TC_PREFIX}-gcc")
set(CMAKE_CXX_COMPILER "${SDK_TOOLS}/${TC_PREFIX}-g++")
set(CMAKE_AR           "${SDK_TOOLS}/${TC_PREFIX}-ar"    CACHE FILEPATH "")
set(CMAKE_STRIP        "${SDK_TOOLS}/${TC_PREFIX}-strip"  CACHE FILEPATH "")

set(CMAKE_SYSROOT        "${SDK_SYSROOT}")
set(CMAKE_FIND_ROOT_PATH "${SDK_SYSROOT}")

# Cortex-A7 armhf flags
set(ARCH_FLAGS "-mthumb -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7")
set(TIME_FLAGS "-D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64")

set(CMAKE_C_FLAGS_INIT   "${ARCH_FLAGS} ${TIME_FLAGS} --sysroot=${CMAKE_SYSROOT}")
set(CMAKE_CXX_FLAGS_INIT "${ARCH_FLAGS} ${TIME_FLAGS} --sysroot=${CMAKE_SYSROOT}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "--sysroot=${CMAKE_SYSROOT}")

# pkg-config must search inside sysroot
set(ENV{PKG_CONFIG_SYSROOT_DIR} "${SDK_SYSROOT}")
set(ENV{PKG_CONFIG_PATH}
    "${SDK_SYSROOT}/usr/lib/pkgconfig:${SDK_SYSROOT}/usr/share/pkgconfig")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
