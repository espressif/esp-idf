set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER riscv32-esp-elf-gcc)
set(CMAKE_CXX_COMPILER riscv32-esp-elf-g++)
set(CMAKE_ASM_COMPILER riscv32-esp-elf-gcc)

list(APPEND compile_options "-march=rv32imc")

# Option `--specs` must only be defined ONCE in the final linker command, else GCC will complain:
# "attempt to rename spec 'link_gcc_c_sequence' to already defined spec 'nosys_link_gcc_c_sequence'"
# so unset `link_options` first.
unset(link_options)
list(APPEND link_options "-nostartfiles" "-march=rv32imc" "--specs=nosys.specs")
