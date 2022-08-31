cmake_minimum_required(VERSION 3.16)

include(${IDF_PATH}/tools/cmake/idf.cmake)
project(${ULP_APP_NAME} ASM C)

option(ULP_COCPU_IS_RISCV "Use RISC-V based ULP" OFF)

message(STATUS "Building ULP app ${ULP_APP_NAME}")

if(ULP_COCPU_IS_RISCV)
    set(ULP_LD_TEMPLATE ${IDF_PATH}/components/ulp/ld/ulp_riscv.ld)
else()
    check_expected_tool_version("esp32ulp-elf" ${CMAKE_ASM_COMPILER})
    set(ULP_LD_TEMPLATE ${IDF_PATH}/components/ulp/ld/ulp_fsm.ld)
endif()


set(ULP_MAP_GEN ${PYTHON} ${IDF_PATH}/components/ulp/esp32ulp_mapgen.py)
get_filename_component(sdkconfig_dir ${SDKCONFIG_HEADER} DIRECTORY)

foreach(include ${COMPONENT_INCLUDES})
    list(APPEND component_includes -I${include})
endforeach()

list(APPEND ULP_PREPROCESSOR_ARGS ${component_includes})
list(APPEND ULP_PREPROCESSOR_ARGS -I${COMPONENT_DIR})
list(APPEND ULP_PREPROCESSOR_ARGS -I${sdkconfig_dir})

include_directories(${COMPONENT_INCLUDES})

list(APPEND ULP_PREPROCESSOR_ARGS -D__ASSEMBLER__)

# Preprocess linker script, pre-linking
get_filename_component(ULP_LD_SCRIPT ${ULP_LD_TEMPLATE} NAME)
add_custom_command(OUTPUT ${ULP_LD_SCRIPT}
                   COMMAND ${CMAKE_C_COMPILER} -E -P -xc -o ${ULP_LD_SCRIPT} ${ULP_PREPROCESSOR_ARGS} ${ULP_LD_TEMPLATE}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS ${ULP_LD_TEMPLATE} ${SDKCONFIG_HEADER}
                   VERBATIM)
add_custom_target(${ULP_APP_NAME}_ld_script
                  DEPENDS ${ULP_LD_SCRIPT}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# To avoid warning "Manually-specified variables were not used by the project"
set(bypassWarning "${IDF_TARGET}")
if(ULP_COCPU_IS_RISCV)
    #risc-v ulp uses extra files for building:
    list(APPEND ULP_S_SOURCES
        "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/start.S"
        "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_adc.c"
        "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_uart.c"
        "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_print.c"
        "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_utils.c")

    #dummy loop to force pre-processed linker file generation:
    foreach(ulp_s_source ${ULP_S_SOURCES})
        set(noop ${ulp_s_source})

        add_custom_command(OUTPUT ${noop}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND cmake -E echo
            DEPENDS ${ULP_LD_SCRIPT}
            )

        set_source_files_properties(${noop} PROPERTIES NOOP_PROPERTY ${ULP_LD_SCRIPT})
    endforeach()

    #creates the executable:
    add_executable(${ULP_APP_NAME} ${ULP_S_SOURCES})
    set(DUMP_SYMBOL_ARGS -g)
    set(MAP_GEN_EXTRA_ARGS --riscv)
    set(EXTRA_LINKER_ARGS "-nostartfiles")
    list(APPEND EXTRA_LINKER_ARGS "-Wl,--gc-sections")
    list(APPEND EXTRA_LINKER_ARGS "-Wl,-Map=\"${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}.map\"")
    #Makes the csr utillies for riscv visible:
    target_include_directories(${ULP_APP_NAME} PRIVATE "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/include")
    target_link_libraries(${ULP_APP_NAME} "-T \"${IDF_PATH}/components/ulp/ld/${IDF_TARGET}.periperals.ld\"")
    target_compile_definitions(${ULP_APP_NAME} PRIVATE IS_ULP_COCPU)

else()

    foreach(ulp_s_source ${ULP_S_SOURCES})
        get_filename_component(ulp_ps_source ${ulp_s_source} NAME_WE)
        set(ulp_ps_output ${CMAKE_CURRENT_BINARY_DIR}/${ulp_ps_source}.ulp.S)
        # Generate preprocessed assembly files.
        add_custom_command(OUTPUT ${ulp_ps_output}
                           WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                           COMMAND ${CMAKE_C_COMPILER} -E -P -xc ${ULP_PREPROCESSOR_ARGS}
                           -o ${ulp_ps_output} ${ulp_s_source}
                           DEPENDS ${ulp_s_source} ${ULP_LD_SCRIPT}
                           VERBATIM)
        # During assembly file compilation, output listing files as well.
        set_source_files_properties(${ulp_ps_output}
                                    PROPERTIES COMPILE_FLAGS
                                    "-al=${CMAKE_CURRENT_BINARY_DIR}/${ulp_ps_source}.lst")
        list(APPEND ULP_PS_SOURCES ${ulp_ps_output})
    endforeach()

    # Create an executable
    add_executable(${ULP_APP_NAME} ${ULP_PS_SOURCES})
    set(DUMP_SYMBOL_ARGS -g -f posix)
    set(MAP_GEN_EXTRA_ARGS .)
    set(EXTRA_LINKER_ARGS "-Map=\"${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}.map\"")

endif()

# Dump the list of global symbols in a convenient format
add_custom_command(OUTPUT ${ULP_APP_NAME}.sym
                   COMMAND ${CMAKE_NM} ${DUMP_SYMBOL_ARGS} $<TARGET_FILE:${ULP_APP_NAME}> > ${ULP_APP_NAME}.sym
                   DEPENDS ${ULP_APP_NAME}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Dump the binary for inclusion into the project
add_custom_command(OUTPUT ${ULP_APP_NAME}.bin
                   COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${ULP_APP_NAME}> ${ULP_APP_NAME}.bin
                   DEPENDS ${ULP_APP_NAME}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(OUTPUT ${ULP_APP_NAME}.ld ${ULP_APP_NAME}.h
                   COMMAND ${ULP_MAP_GEN} ${MAP_GEN_EXTRA_ARGS} -s ${ULP_APP_NAME}.sym -o ${ULP_APP_NAME}
                   DEPENDS ${ULP_APP_NAME}.sym
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Building the component separately from the project should result in
# ULP files being built.
add_custom_target(build
                DEPENDS ${ULP_APP_NAME} ${ULP_APP_NAME}.bin ${ULP_APP_NAME}.sym
                        ${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}.ld
                        ${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}.h
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${ULP_APP_NAME} "-T\"${CMAKE_CURRENT_BINARY_DIR}/${ULP_LD_SCRIPT}\"")
target_link_libraries(${ULP_APP_NAME} ${EXTRA_LINKER_ARGS})
set_target_properties(${ULP_APP_NAME} PROPERTIES LINK_DEPENDS ${ULP_LD_SCRIPT})
