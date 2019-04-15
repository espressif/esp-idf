set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER xtensa-esp32-elf-gcc)
set(CMAKE_CXX_COMPILER xtensa-esp32-elf-g++)
set(CMAKE_ASM_COMPILER xtensa-esp32-elf-gcc)

set(CMAKE_C_FLAGS "-mlongcalls -Wno-frame-address" CACHE STRING "C Compiler Base Flags")
set(CMAKE_CXX_FLAGS "-mlongcalls -Wno-frame-address" CACHE STRING "C++ Compiler Base Flags")

# Can be removed after gcc 5.2.0 support is removed (ref GCC_NOT_5_2_0)
set(CMAKE_EXE_LINKER_FLAGS "-nostdlib" CACHE STRING "Linker Base Flags")
