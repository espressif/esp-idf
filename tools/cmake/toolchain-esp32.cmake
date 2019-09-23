set(CMAKE_SYSTEM_NAME Generic)

if(NOT (DEFINED CMAKE_C_COMPILER))
set(CMAKE_C_COMPILER xtensa-esp32-elf-gcc)
endif(NOT (DEFINED CMAKE_C_COMPILER))

if(NOT (DEFINED CMAKE_CXX_COMPILER))
set(CMAKE_CXX_COMPILER xtensa-esp32-elf-g++)
endif(NOT (DEFINED CMAKE_CXX_COMPILER))

if(NOT (DEFINED CMAKE_ASM_COMPILER))
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
endif(NOT (DEFINED CMAKE_ASM_COMPILER))

set(CMAKE_C_FLAGS "-mlongcalls -Wno-frame-address" CACHE STRING "C Compiler Base Flags")
set(CMAKE_CXX_FLAGS "-mlongcalls -Wno-frame-address" CACHE STRING "C++ Compiler Base Flags")

# Can be removed after gcc 5.2.0 support is removed (ref GCC_NOT_5_2_0)
set(CMAKE_EXE_LINKER_FLAGS "-nostdlib" CACHE STRING "Linker Base Flags")
