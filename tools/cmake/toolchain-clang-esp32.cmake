include($ENV{IDF_PATH}/tools/cmake/utilities.cmake)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_ASM_COMPILER clang)

set(CMAKE_AR xtensa-esp32-elf-ar)
set(CMAKE_RANLIB xtensa-esp32-elf-gcc-ranlib)
set(CMAKE_OBJDUMP xtensa-esp32-elf-objdump)

# -freestanding is a hack to force Clang to use its own stdatomic.h,
# without falling back to the (incompatible) GCC stdatomic.h
# https://github.com/espressif/llvm-project/blob/d9341b81/clang/lib/Headers/stdatomic.h#L13-L18
remove_duplicated_flags("--target=xtensa -mcpu=esp32 -ffreestanding ${CMAKE_C_FLAGS}"
                        UNIQ_CMAKE_C_FLAGS)
set(CMAKE_C_FLAGS "${UNIQ_CMAKE_C_FLAGS}"
    CACHE STRING "C Compiler Base Flags"
    FORCE)

remove_duplicated_flags("--target=xtensa -mcpu=esp32 -ffreestanding ${CMAKE_CXX_FLAGS}"
                        UNIQ_CMAKE_CXX_FLAGS)
set(CMAKE_CXX_FLAGS "${UNIQ_CMAKE_CXX_FLAGS}"
    CACHE STRING "C++ Compiler Base Flags"
    FORCE)

remove_duplicated_flags("--target=xtensa -mcpu=esp32 ${CMAKE_ASM_FLAGS}" UNIQ_CMAKE_ASM_FLAGS)
set(CMAKE_ASM_FLAGS "${UNIQ_CMAKE_ASM_FLAGS}"
    CACHE STRING "Assembler Base Flags"
    FORCE)
