include($ENV{IDF_PATH}/tools/cmake/deduplicate_flags.cmake)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER riscv32-esp-elf-gcc)
set(CMAKE_CXX_COMPILER riscv32-esp-elf-g++)
set(CMAKE_ASM_COMPILER riscv32-esp-elf-gcc)
set(_CMAKE_TOOLCHAIN_PREFIX riscv32-esp-elf-)

set(_CMAKE_TOOLCHAIN_COMMON_FLAGS "-march=rv32imafcb_zicsr_zifencei_zaamo_zalrsc_zcb_zcmp_zcmt_zba_zbb_zbs_xespdsp \
                                   -mabi=ilp32f \
                                   -mno-cm-popret -mno-cm-push-reverse")

remove_duplicated_flags("${_CMAKE_TOOLCHAIN_COMMON_FLAGS} -mtune=esp-base ${CMAKE_C_FLAGS}" UNIQ_CMAKE_C_FLAGS)
set(CMAKE_C_FLAGS "${UNIQ_CMAKE_C_FLAGS}" CACHE STRING "C Compiler Base Flags" FORCE)

remove_duplicated_flags("${_CMAKE_TOOLCHAIN_COMMON_FLAGS} -mtune=esp-base ${CMAKE_CXX_FLAGS}" UNIQ_CMAKE_CXX_FLAGS)
set(CMAKE_CXX_FLAGS "${UNIQ_CMAKE_CXX_FLAGS}" CACHE STRING "C++ Compiler Base Flags" FORCE)

remove_duplicated_flags("${_CMAKE_TOOLCHAIN_COMMON_FLAGS} ${CMAKE_ASM_FLAGS}" UNIQ_CMAKE_ASM_FLAGS)
set(CMAKE_ASM_FLAGS "${UNIQ_CMAKE_ASM_FLAGS}" CACHE STRING "Asm Compiler Base Flags" FORCE)

remove_duplicated_flags("-nostartfiles ${_CMAKE_TOOLCHAIN_COMMON_FLAGS} ${CMAKE_EXE_LINKER_FLAGS}"
                        UNIQ_CMAKE_SAFE_EXE_LINKER_FLAGS)
set(CMAKE_EXE_LINKER_FLAGS "${UNIQ_CMAKE_SAFE_EXE_LINKER_FLAGS}" CACHE STRING "Linker Base Flags" FORCE)
