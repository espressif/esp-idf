# CMake toolchain file for ULP
set(CMAKE_SYSTEM_NAME Generic)

# Compiler is only used for preprocessing
set(CMAKE_C_COMPILER "xtensa-esp32-elf-gcc")
set(CMAKE_CXX_COMPILER "xtensa-esp32-elf-g++")

set(CMAKE_ASM_COMPILER "esp32ulp-elf-as")
set(CMAKE_OBJCOPY "esp32ulp-elf-objcopy")
set(CMAKE_LINKER "esp32ulp-elf-ld")

# Native CMake v2 ULP projects include cmakev2/idf.cmake before project(),
# making idf_project_init visible when this toolchain is loaded. Legacy child
# projects, even when launched by a CMake v2 parent, include the old IDF CMake
# entrypoint and keep the historical explicit preprocessing path.
set(ULP_PREPROCESS_FSM_ASM_IN_TOOLCHAIN OFF)
if(IDF_BUILD_V2 AND COMMAND idf_project_init)
    set(ULP_PREPROCESS_FSM_ASM_IN_TOOLCHAIN ON)
endif()

if(ULP_PREPROCESS_FSM_ASM_IN_TOOLCHAIN)
    set(CMAKE_ASM${ASM_DIALECT}_COMPILE_OBJECT
        "${CMAKE_COMMAND} \
        -DULP_FSM_PREPROCESSOR=${CMAKE_C_COMPILER} \
        -DULP_FSM_ASSEMBLER=${CMAKE_ASM${ASM_DIALECT}_COMPILER} \
        -DULP_FSM_TARGET=esp32 \
        -DULP_FSM_SOURCE=<SOURCE> \
        -DULP_FSM_OBJECT=<OBJECT> \
        -P ${CMAKE_CURRENT_LIST_DIR}/preprocess_fsm_asm.cmake -- <DEFINES> <INCLUDES>")
else()
    set(CMAKE_ASM${ASM_DIALECT}_COMPILE_OBJECT "${CMAKE_ASM${ASM_DIALECT}_COMPILER} \
                                        --mcpu=esp32 <DEFINES> <INCLUDES> -o <OBJECT> -c <SOURCE>")
endif()
set(CMAKE_EXE_LINKER_FLAGS "-A elf32-esp32ulp -nostdlib" CACHE STRING "ULP Linker Base Flags")
set(CMAKE_ASM_LINK_EXECUTABLE "${CMAKE_LINKER} <FLAGS> <CMAKE_ASM_LINK_FLAGS> \
                                <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
