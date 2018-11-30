set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER xtensa-esp32-elf-gcc)
set(CMAKE_CXX_COMPILER xtensa-esp32-elf-g++)
set(CMAKE_ASM_COMPILER xtensa-esp32-elf-gcc)

set(CMAKE_EXE_LINKER_FLAGS "-nostdlib" CACHE STRING "Linker Base Flags")
set(CMAKE_C_FLAGS "-mlongcalls" CACHE STRING "C Compiler Base Flags")
set(CMAKE_CXX_FLAGS "-mlongcalls" CACHE STRING "C++ Compiler Base Flags")
