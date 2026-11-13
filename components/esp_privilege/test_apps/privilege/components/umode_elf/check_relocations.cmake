# Fail if a U-mode program is not a shared object, or has a relocation that umode_elf_load() cannot apply.
# Run with -D READELF=<readelf> -D ELF=<file>.
execute_process(COMMAND "${READELF}" -h -l -r -W "${ELF}" OUTPUT_VARIABLE output RESULT_VARIABLE result)
if(NOT result EQUAL 0)
    file(REMOVE "${ELF}")
    message(FATAL_ERROR "${READELF} failed on ${ELF}")
endif()
# Without a dynamic section, a file has no relocations at all, and the check below would pass
if(NOT output MATCHES "Type:[ \t]+DYN" OR NOT output MATCHES "\n[ \t]*DYNAMIC[ \t]")
    file(REMOVE "${ELF}")
    message(FATAL_ERROR "${ELF} is not a shared object with a dynamic section. Link it with -shared.")
endif()
string(REGEX MATCHALL "R_RISCV_[A-Z0-9_]+" types "${output}")
list(REMOVE_ITEM types R_RISCV_RELATIVE R_RISCV_NONE)
if(types)
    list(REMOVE_DUPLICATES types)
    file(REMOVE "${ELF}")
    message(FATAL_ERROR "${ELF} has relocations of types ${types}. U-mode programs can only use "
                        "R_RISCV_RELATIVE: declare functions and variables static, or give them hidden "
                        "visibility, and do not call functions that are not part of the program.")
endif()
