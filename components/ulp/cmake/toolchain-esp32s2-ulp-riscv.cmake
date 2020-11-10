# CMake toolchain file for ULP-RISC-V
set(CMAKE_SYSTEM_NAME Generic)

# Not best solution, needs to figure why the compiler detection
# fails in CI
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

set(CMAKE_C_COMPILER "riscv-none-embed-gcc")
set(CMAKE_ASM_COMPILER "riscv-none-embed-gcc")

if(NOT ASM_DIALECT)
    set(ASM_DIALECT "")
endif()

set(CMAKE_C_FLAGS "-Os -march=rv32imc -mdiv -fdata-sections -ffunction-sections")
set(CMAKE_ASM_FLAGS "-march=rv32imc -mdiv -x assembler-with-cpp -fdata-sections -ffunction-sections")
set(CMAKE_EXE_LINKER_FLAGS "-march=rv32imc --specs=nano.specs --specs=nosys.specs")
