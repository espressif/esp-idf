# CMake toolchain file for ULP

set(CMAKE_SYSTEM_NAME Generic)

# Compiler is only used for preprocessing
set(CMAKE_C_COMPILER "xtensa-esp32-elf-gcc")

set(CMAKE_ASM_COMPILER "esp32ulp-elf-as")
set(CMAKE_LINKER "esp32ulp-elf-ld")

if(NOT ASM_DIALECT)
    set(ASM_DIALECT "")
endif()

set(CMAKE_ASM${ASM_DIALECT}_COMPILE_OBJECT "${CMAKE_ASM${ASM_DIALECT}_COMPILER} \
                                    <DEFINES> <INCLUDES> -o <OBJECT> -c <SOURCE>")
set(CMAKE_EXE_LINKER_FLAGS "-A elf32-esp32ulp -nostdlib" CACHE STRING "ULP Linker Base Flags")
set(CMAKE_ASM_LINK_EXECUTABLE "${CMAKE_LINKER} <FLAGS> <CMAKE_ASM_LINK_FLAGS> \
                                <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
