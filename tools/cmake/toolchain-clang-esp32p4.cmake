include($ENV{IDF_PATH}/tools/cmake/utilities.cmake)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_ASM_COMPILER clang)
set(CMAKE_LINKER riscv32-esp-elf-clang-ld)

set(CMAKE_AR llvm-ar)
set(CMAKE_RANLIB llvm-ranlib)
set(CMAKE_OBJDUMP riscv32-esp-elf-clang-objdump)

remove_duplicated_flags("--target=riscv32-esp-elf -march=rv32imafc_zicsr_zifencei_xesppie -mabi=ilp32f \
                        ${CMAKE_C_FLAGS}"
                        UNIQ_CMAKE_C_FLAGS)
set(CMAKE_C_FLAGS "${UNIQ_CMAKE_C_FLAGS}"
    CACHE STRING "C Compiler Base Flags"
    FORCE)

remove_duplicated_flags("--target=riscv32-esp-elf -march=rv32imafc_zicsr_zifencei_xesppie -mabi=ilp32f \
                        ${CMAKE_CXX_FLAGS}"
                        UNIQ_CMAKE_CXX_FLAGS)
set(CMAKE_CXX_FLAGS "${UNIQ_CMAKE_CXX_FLAGS}"
    CACHE STRING "C++ Compiler Base Flags"
    FORCE)

remove_duplicated_flags("--target=riscv32-esp-elf -march=rv32imafc_zicsr_zifencei_xesppie -mabi=ilp32f \
                        ${CMAKE_ASM_FLAGS}"
                        UNIQ_CMAKE_ASM_FLAGS)
set(CMAKE_ASM_FLAGS "${UNIQ_CMAKE_ASM_FLAGS}"
    CACHE STRING "Assembler Base Flags"
    FORCE)

remove_duplicated_flags("--ld-path=riscv32-esp-elf-clang-ld -z noexecstack \
                        ${CMAKE_EXE_LINKER_FLAGS}"
                        UNIQ_CMAKE_EXE_LINKER_FLAGS)
set(CMAKE_EXE_LINKER_FLAGS "${UNIQ_CMAKE_EXE_LINKER_FLAGS}"
    CACHE STRING "Linker Base Flags"
    FORCE)
