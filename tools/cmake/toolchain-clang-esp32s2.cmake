include($ENV{IDF_PATH}/tools/cmake/utilities.cmake)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_ASM_COMPILER clang)


set(CMAKE_AR llvm-ar)
set(CMAKE_RANLIB llvm-ranlib)
set(CMAKE_OBJDUMP xtensa-esp32s2-elf-objdump)

remove_duplicated_flags("--target=xtensa-esp-elf -mcpu=esp32s2 ${CMAKE_C_FLAGS}"
                        UNIQ_CMAKE_C_FLAGS)
set(CMAKE_C_FLAGS "${UNIQ_CMAKE_C_FLAGS}"
    CACHE STRING "C Compiler Base Flags"
    FORCE)

remove_duplicated_flags("--target=xtensa-esp-elf -mcpu=esp32s2 \
                        ${CMAKE_CXX_FLAGS}"
                        UNIQ_CMAKE_CXX_FLAGS)
set(CMAKE_CXX_FLAGS "${UNIQ_CMAKE_CXX_FLAGS}"
    CACHE STRING "C++ Compiler Base Flags"
    FORCE)

remove_duplicated_flags("--target=xtensa-esp-elf -mcpu=esp32s2 -Xassembler --longcalls ${CMAKE_ASM_FLAGS}"
                        UNIQ_CMAKE_ASM_FLAGS)
set(CMAKE_ASM_FLAGS "${UNIQ_CMAKE_ASM_FLAGS}"
    CACHE STRING "Assembler Base Flags"
    FORCE)
