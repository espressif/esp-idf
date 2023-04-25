# CMake toolchain file for ULP LP core
set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER "riscv32-esp-elf-gcc")
set(CMAKE_CXX_COMPILER "riscv32-esp-elf-g++")
set(CMAKE_ASM_COMPILER "riscv32-esp-elf-gcc")

set(CMAKE_C_FLAGS "-Os -march=rv32imac_zicsr_zifencei -mdiv -fdata-sections -ffunction-sections"
    CACHE STRING "C Compiler Base Flags")
set(CMAKE_CXX_FLAGS "-Os -march=rv32imac_zicsr_zifencei -mdiv -fdata-sections -ffunction-sections"
    CACHE STRING "C++ Compiler Base Flags")
set(CMAKE_ASM_FLAGS "-march=rv32imac_zicsr_zifencei -x assembler-with-cpp"
    CACHE STRING "Assembler Base Flags")
set(CMAKE_EXE_LINKER_FLAGS "-march=rv32imac_zicsr_zifencei --specs=nano.specs --specs=nosys.specs"
    CACHE STRING "Linker Base Flags")
