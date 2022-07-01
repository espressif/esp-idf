# CMake toolchain file for ULP

set(CMAKE_SYSTEM_NAME Generic)

# Compiler is only used for preprocessing
# The S2 and S3 ULP are the same, so we use the same toolchain
set(CMAKE_C_COMPILER "xtensa-esp32s2-elf-gcc")

set(CMAKE_ASM_COMPILER "esp32s2ulp-elf-as")
set(CMAKE_LINKER "esp32s2ulp-elf-ld")

set(CMAKE_ASM${ASM_DIALECT}_COMPILE_OBJECT "${CMAKE_ASM${ASM_DIALECT}_COMPILER} \
                                    <DEFINES> <INCLUDES> -o <OBJECT> -c <SOURCE>")
set(CMAKE_EXE_LINKER_FLAGS "-A elf32-esp32s2ulp -nostdlib" CACHE STRING "ULP Linker Base Flags")
set(CMAKE_ASM_LINK_EXECUTABLE "${CMAKE_LINKER} <FLAGS> <CMAKE_ASM_LINK_FLAGS> \
                                <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
