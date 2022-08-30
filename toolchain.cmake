# CMake system name must be something like "Linux".
# This is important for cross-compiling.

set( CMAKE_SYSTEM_NAME Linux )
set( CMAKE_SYSTEM_PROCESSOR arm )
set( CMAKE_C_COMPILER arm-oe-linux-gnueabi-gcc )
set( CMAKE_CXX_COMPILER arm-oe-linux-gnueabi-g++ )
set( CMAKE_ASM_COMPILER arm-oe-linux-gnueabi-gcc )
set( CMAKE_AR arm-oe-linux-gnueabi-ar CACHE FILEPATH "Archiver" )
set( CMAKE_C_FLAGS " -march=armv7ve -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7 -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed  --sysroot=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot  -O2 -pipe -g -feliminate-unused-debug-types -fdebug-prefix-map=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0=/usr/src/debug/helloworld/0.1-r0 -fdebug-prefix-map=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot= -fdebug-prefix-map=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot-native=   -march=armv7ve -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7 -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed  --sysroot=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot" CACHE STRING "CFLAGS" )
set( CMAKE_CXX_FLAGS " -march=armv7ve -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7 -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed  --sysroot=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot  -O2 -pipe -g -feliminate-unused-debug-types -fdebug-prefix-map=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0=/usr/src/debug/helloworld/0.1-r0 -fdebug-prefix-map=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot= -fdebug-prefix-map=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot-native=  -fvisibility-inlines-hidden  -march=armv7ve -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7 -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed  --sysroot=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot" CACHE STRING "CXXFLAGS" )
set( CMAKE_ASM_FLAGS " -march=armv7ve -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7 -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed  --sysroot=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot  -O2 -pipe -g -feliminate-unused-debug-types -fdebug-prefix-map=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0=/usr/src/debug/helloworld/0.1-r0 -fdebug-prefix-map=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot= -fdebug-prefix-map=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot-native=   -march=armv7ve -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7 -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed  --sysroot=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot" CACHE STRING "ASM FLAGS" )
set( CMAKE_C_FLAGS_RELEASE "-DNDEBUG" CACHE STRING "Additional CFLAGS for release" )
set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG" CACHE STRING "Additional CXXFLAGS for release" )
set( CMAKE_ASM_FLAGS_RELEASE "-DNDEBUG" CACHE STRING "Additional ASM FLAGS for release" )
set( CMAKE_C_LINK_FLAGS " -march=armv7ve -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7 -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed  --sysroot=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot  -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed" CACHE STRING "LDFLAGS" )
set( CMAKE_CXX_LINK_FLAGS " -march=armv7ve -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7 -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed  --sysroot=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot  -O2 -pipe -g -feliminate-unused-debug-types -fdebug-prefix-map=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0=/usr/src/debug/helloworld/0.1-r0 -fdebug-prefix-map=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot= -fdebug-prefix-map=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot-native=  -fvisibility-inlines-hidden  -march=armv7ve -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7 -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed  --sysroot=/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed" CACHE STRING "LDFLAGS" )

# only search in the paths provided so cmake doesnt pick
# up libraries and tools from the native build machine
set( CMAKE_FIND_ROOT_PATH /home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot /home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot-native    )
set( CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )

# Use qt.conf settings
set( ENV{QT_CONF_PATH} /home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/qt.conf )

# We need to set the rpath to the correct directory as cmake does not provide any
# directory as rpath by default
set( CMAKE_INSTALL_RPATH  )

# Use native cmake modules
list(APPEND CMAKE_MODULE_PATH "/home/arar/orangepiyocto/orange-pi-distro/build/tmp-glibc/work/cortexa7hf-neon-vfpv4-oe-linux-gnueabi/helloworld/0.1-r0/recipe-sysroot/usr/share/cmake/Modules/")

# add for non /usr/lib libdir, e.g. /usr/lib64
set( CMAKE_LIBRARY_PATH /usr/lib /lib)

