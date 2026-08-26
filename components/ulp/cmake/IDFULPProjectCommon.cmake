macro(ulp_detect_build_type)
    # Logic to determine ULP type and set reusable flags
    set(BUILD_RISCV OFF)
    set(BUILD_FSM OFF)
    set(BUILD_LP_CORE OFF)

    # If ULP_TYPE is explicitly set, use it; otherwise fall back to CONFIG checks
    if(ULP_TYPE)
        string(TOLOWER "${ULP_TYPE}" ulp_type_lower)
        if(ulp_type_lower STREQUAL "riscv")
            set(BUILD_RISCV ON)
        elseif(ulp_type_lower STREQUAL "fsm")
            set(BUILD_FSM ON)
        elseif(ulp_type_lower STREQUAL "lp_core")
            set(BUILD_LP_CORE ON)
        endif()
    elseif(CONFIG_ULP_COPROC_TYPE_RISCV)
        set(BUILD_RISCV ON)
    elseif(CONFIG_ULP_COPROC_TYPE_LP_CORE)
        set(BUILD_LP_CORE ON)
    elseif(CONFIG_ULP_COPROC_TYPE_FSM)
        set(BUILD_FSM ON)
    endif()
endmacro()

macro(ulp_apply_build_type_options)
    # CMake initializes GNU ASM definitions as --defsym during enable_language().
    # Native full-subproject FSM sources are preprocessed by the toolchain rule,
    # so use C preprocessor definitions for generated ASM rules.
    if(BUILD_FSM AND ULP_PREPROCESS_FSM_ASM_IN_TOOLCHAIN)
        set(CMAKE_ASM_DEFINE_FLAG "-D")
    endif()
endmacro()

function(ulp_add_build_binary_targets ulp_app_name)
    cmake_parse_arguments(ULP "" "PREFIX" "" ${ARGN})
    if(NOT ULP_PREFIX)
        set(ULP_PREFIX "ulp_")
    endif()

    if(ADD_PICOLIBC_SPECS)
        target_compile_options(${ulp_app_name} PRIVATE $<$<COMPILE_LANG_AND_ID:C,GNU>:-specs=picolibc.specs>)
        target_compile_options(${ulp_app_name} PRIVATE $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-specs=picolibcpp.specs>)
    endif()

    if(BUILD_LP_CORE)
        set(ULP_BASE_ADDR "0x0")
    else()
        set(ULP_BASE_ADDR "0x50000000")
    endif()

    set(ULP_MAP_GEN ${PYTHON} ${IDF_PATH}/components/ulp/esp32ulp_mapgen.py)

    # Dump the list of global symbols in a convenient format
    add_custom_command(OUTPUT ${ulp_app_name}.sym
                    COMMAND ${CMAKE_READELF} -sW $<TARGET_FILE:${ulp_app_name}> > ${ulp_app_name}.sym
                    DEPENDS ${ulp_app_name}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    # Dump the binary for inclusion into the project
    if(BUILD_LP_CORE AND CONFIG_ULP_COPROC_RUN_FROM_HP_MEM)
        # LP-core images running from HP memory use the normal ESP image format;
        # RTC-memory ULP binaries are raw object-copy output.
        add_custom_command(OUTPUT ${ulp_app_name}.bin
                        COMMAND ${PYTHON} -m esptool --chip ${IDF_TARGET} elf2image
                                --output ${ulp_app_name}.bin $<TARGET_FILE:${ulp_app_name}>
                        DEPENDS ${ulp_app_name}
                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    else()
        add_custom_command(OUTPUT ${ulp_app_name}.bin
                        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${ulp_app_name}> ${ulp_app_name}.bin
                        DEPENDS ${ulp_app_name}
                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endif()

    add_custom_command(OUTPUT ${ulp_app_name}.ld ${ulp_app_name}.h
                    COMMAND ${ULP_MAP_GEN} -s ${ulp_app_name}.sym -o ${ulp_app_name}
                            --base ${ULP_BASE_ADDR} --prefix ${ULP_PREFIX}
                            --target ${IDF_TARGET}
                    DEPENDS ${ulp_app_name}.sym
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_custom_target(${ulp_app_name}_binary_artifacts
                    DEPENDS ${ulp_app_name} ${ulp_app_name}.bin ${ulp_app_name}.sym
                            ${CMAKE_CURRENT_BINARY_DIR}/${ulp_app_name}.ld
                            ${CMAKE_CURRENT_BINARY_DIR}/${ulp_app_name}.h
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    # Building the component separately from the project should result in all
    # ULP files being built. Multiple ULP executables in one child project all
    # contribute their artifact targets here.
    if(NOT TARGET build)
        add_custom_target(build)
    endif()
    add_dependencies(build ${ulp_app_name}_binary_artifacts)
endfunction()
