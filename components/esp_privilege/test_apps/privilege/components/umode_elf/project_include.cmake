# umode_elf_add_program(<name> SRCS <files>... [INCLUDE_DIRS <dirs>...] [COMPONENT <component library>])
#
# Build U-mode program sources into an ELF file that umode_elf_load() can load, and embed it into a
# component as binary data. The file is available as _binary_<name>_elf_start and
# _binary_<name>_elf_end. The component defaults to the one that calls the function.
#
# The program is an executable target of its own, built with the project's toolchain, for the
# integer RV32IMC instruction set, without a C library or ESP-IDF, as a position-independent
# shared object:
# - user/umode_start.S is linked in as the entry point _start. It calls
#   int user_main(uint32_t arg0, uint32_t arg1), which the program defines, and ends the run with
#   the value that user_main returns. Programs include umode_app.h for umode_exit().
# - Hidden visibility makes every reference inside the program relative, so the only relocations
#   are R_RISCV_RELATIVE and the program needs no symbol from the application. The build fails if
#   the file has any other relocation.
# - user/umode.ld starts each segment on a page of its own, so that each can be mapped with its own
#   permissions, and keeps relocated data in the writable segment.
# - Without linker relaxation gp is never used, so the program runs with gp and tp set to 0.
function(umode_elf_add_program name)
    cmake_parse_arguments(arg "" "COMPONENT" "SRCS;INCLUDE_DIRS" ${ARGN})
    if(NOT arg_SRCS)
        message(FATAL_ERROR "umode_elf_add_program(${name}): SRCS is required")
    endif()
    if(NOT arg_COMPONENT)
        if(NOT COMPONENT_LIB)
            message(FATAL_ERROR "umode_elf_add_program(${name}): COMPONENT is required outside of a component")
        endif()
        set(arg_COMPONENT ${COMPONENT_LIB})
    endif()

    set(user_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/user")
    set(target "${name}_umode_elf")
    add_executable(${target} "${user_dir}/umode_start.S" ${arg_SRCS})

    # The target inherits the toolchain flags (CMAKE_<LANG>_FLAGS, CMAKE_EXE_LINKER_FLAGS), which select the
    # compiler target, and the compile options of the project (optimization level, debug info, C standard,
    # warnings), which it keeps. Drop what the component directory adds for ESP-IDF libraries: the program
    # must not see ESP-IDF headers, or link ESP-IDF libraries or their linker scripts.
    set_target_properties(${target} PROPERTIES
        COMPILE_DEFINITIONS ""
        INCLUDE_DIRECTORIES ""
        LINK_LIBRARIES ""
        LINK_OPTIONS ""
        SUFFIX ".elf"
        OUTPUT_NAME "${name}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

    # Only for esp_privilege_abi.h, which does not depend on ESP-IDF
    idf_component_get_property(privilege_dir esp_privilege COMPONENT_DIR)
    target_include_directories(${target} PRIVATE "${user_dir}" ${arg_INCLUDE_DIRS} "${privilege_dir}/include")

    # These come after the toolchain and project flags and override them. The program uses the base integer
    # ISA, not the chip's: domains do not set up the FPU or the Zcmt jump table (jvt).
    set(arch_options -march=rv32imc_zicsr_zifencei -mabi=ilp32)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        # The toolchain has no libraries for this ISA, and the program does not link any
        list(APPEND arch_options -Wno-missing-multilib)
    endif()
    target_compile_options(${target} PRIVATE ${arch_options}
        -ffreestanding -fno-builtin -fPIC -fvisibility=hidden -mno-relax -msmall-data-limit=0
        # The stack protector and LTO would need symbols or plugins from outside the program
        -fno-stack-protector -fno-lto -fno-unwind-tables -fno-asynchronous-unwind-tables)
    set(linker_script "${user_dir}/umode.ld")
    target_link_options(${target} PRIVATE ${arch_options}
        # -shared to the linker directly: the clang driver ignores it for bare-metal targets
        -fPIC -nostdlib -nostartfiles -Wl,-shared -T "${linker_script}"
        -Wl,--no-relax -Wl,-z,max-page-size=0x1000 -Wl,--build-id=none)
    # The check runs when the program is linked, so link again when the check changes
    set(check_script "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_relocations.cmake")
    set_property(TARGET ${target} APPEND PROPERTY LINK_DEPENDS "${linker_script}" "${check_script}")

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -D "READELF=${CMAKE_READELF}" -D "ELF=$<TARGET_FILE:${target}>"
                -P "${check_script}"
        VERBATIM)

    # The executable target writes the file, not a custom command
    set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/${name}.elf" PROPERTIES GENERATED TRUE)
    target_add_binary_data(${arg_COMPONENT} "${CMAKE_CURRENT_BINARY_DIR}/${name}.elf" BINARY
                           RENAME_TO ${name}_elf ALIGN 4 DEPENDS ${target})
endfunction()
