cmake_minimum_required(VERSION 3.16)

include(${IDF_PATH}/tools/cmake/idf.cmake)
project(${ULP_APP_NAME} ASM C)
add_executable(${ULP_APP_NAME})
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

option(ULP_COCPU_IS_RISCV "Use RISC-V based ULP" OFF)
option(ULP_COCPU_IS_LP_CORE "Use RISC-V based LP Core" OFF)

message(STATUS "Building ULP app ${ULP_APP_NAME}")

# Check the supported assembler version
if(NOT (ULP_COCPU_IS_RISCV OR ULP_COCPU_IS_LP_CORE))
    check_expected_tool_version("esp32ulp-elf" ${CMAKE_ASM_COMPILER})
endif()


set(ULP_MAP_GEN ${PYTHON} ${IDF_PATH}/components/ulp/esp32ulp_mapgen.py)
get_filename_component(sdkconfig_dir ${SDKCONFIG_HEADER} DIRECTORY)

foreach(include ${COMPONENT_INCLUDES})
    list(APPEND component_includes -I${include})
endforeach()

list(APPEND ULP_PREPROCESSOR_ARGS ${component_includes})
list(APPEND ULP_PREPROCESSOR_ARGS -I${COMPONENT_DIR})
list(APPEND ULP_PREPROCESSOR_ARGS -I${sdkconfig_dir})

target_include_directories(${ULP_APP_NAME} PRIVATE ${COMPONENT_INCLUDES})

list(APPEND ULP_PREPROCESSOR_ARGS -D__ASSEMBLER__)

# Pre-process the linker script
if(ULP_COCPU_IS_RISCV)
    set(ULP_LD_TEMPLATE ${IDF_PATH}/components/ulp/ld/ulp_riscv.ld)
elseif(ULP_COCPU_IS_LP_CORE)
    set(ULP_LD_TEMPLATE ${IDF_PATH}/components/ulp/ld/lp_core_riscv.ld)
else()
    set(ULP_LD_TEMPLATE ${IDF_PATH}/components/ulp/ld/ulp_fsm.ld)
endif()

get_filename_component(ULP_LD_SCRIPT ${ULP_LD_TEMPLATE} NAME)
add_custom_command(OUTPUT ${ULP_LD_SCRIPT}
                   COMMAND ${CMAKE_C_COMPILER} -E -P -xc -o ${ULP_LD_SCRIPT} ${ULP_PREPROCESSOR_ARGS} ${ULP_LD_TEMPLATE}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   MAIN_DEPENDENCY ${ULP_LD_TEMPLATE}
                   DEPENDS ${SDKCONFIG_HEADER}
                   COMMENT "Generating ${ULP_LD_SCRIPT} linker script..."
                   VERBATIM)
add_custom_target(ld_script DEPENDS ${ULP_LD_SCRIPT})
add_dependencies(${ULP_APP_NAME} ld_script)
target_link_options(${ULP_APP_NAME} PRIVATE SHELL:-T ${CMAKE_CURRENT_BINARY_DIR}/${ULP_LD_SCRIPT})

# To avoid warning "Manually-specified variables were not used by the project"
set(bypassWarning "${IDF_TARGET}")
if(ULP_COCPU_IS_RISCV)
    #risc-v ulp uses extra files for building:
    list(APPEND ULP_S_SOURCES
        "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/start.S"
        "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_adc.c"
        "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_lock.c"
        "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_uart.c"
        "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_print.c"
        "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_i2c.c"
        "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_utils.c"
        "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_touch.c")

    target_link_options(${ULP_APP_NAME} PRIVATE "-nostartfiles")
    target_link_options(${ULP_APP_NAME} PRIVATE -Wl,--gc-sections)
    target_link_options(${ULP_APP_NAME} PRIVATE -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}.map)
    target_sources(${ULP_APP_NAME} PRIVATE ${ULP_S_SOURCES})
    #Makes the csr utillies for riscv visible:
    target_include_directories(${ULP_APP_NAME} PRIVATE "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/include"
                                                       "${IDF_PATH}/components/ulp/ulp_riscv/shared/include")
    target_link_options(${ULP_APP_NAME} PRIVATE SHELL:-T ${IDF_PATH}/components/ulp/ld/${IDF_TARGET}.peripherals.ld)
    target_link_options(${ULP_APP_NAME} PRIVATE "-Wl,--no-warn-rwx-segments")
    target_compile_definitions(${ULP_APP_NAME} PRIVATE IS_ULP_COCPU)
    target_compile_definitions(${ULP_APP_NAME} PRIVATE ULP_RISCV_REGISTER_OPS)

elseif(ULP_COCPU_IS_LP_CORE)
    list(APPEND ULP_S_SOURCES
    "${IDF_PATH}/components/ulp/lp_core/lp_core/start.S"
    "${IDF_PATH}/components/ulp/lp_core/lp_core/vector.S"
    "${IDF_PATH}/components/ulp/lp_core/shared/ulp_lp_core_memory_shared.c"
    "${IDF_PATH}/components/ulp/lp_core/shared/ulp_lp_core_lp_timer_shared.c"
    "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_startup.c"
    "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_utils.c"
    "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_i2c.c"
    "${IDF_PATH}/components/hal/uart_hal_iram.c"
    "${IDF_PATH}/components/hal/uart_hal.c"
    "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_uart.c"
    "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_print.c")

    target_link_options(${ULP_APP_NAME} PRIVATE "-nostartfiles")
    target_link_options(${ULP_APP_NAME} PRIVATE "-Wl,--no-warn-rwx-segments")
    target_link_options(${ULP_APP_NAME} PRIVATE -Wl,--gc-sections)
    target_link_options(${ULP_APP_NAME} PRIVATE -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}.map)
    target_link_options(${ULP_APP_NAME}
        PRIVATE SHELL:-T ${IDF_PATH}/components/soc/${IDF_TARGET}/ld/${IDF_TARGET}.peripherals.ld)
    target_sources(${ULP_APP_NAME} PRIVATE ${ULP_S_SOURCES})
    target_include_directories(${ULP_APP_NAME} PRIVATE "${IDF_PATH}/components/ulp/lp_core/lp_core/include"
                                                       "${IDF_PATH}/components/ulp/lp_core/shared/include")
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
                           DEPENDS ${ulp_s_source}
                           VERBATIM)
        # During assembly file compilation, output listing files as well.
        set_source_files_properties(${ulp_ps_output}
                                    PROPERTIES COMPILE_FLAGS
                                    "-al=${CMAKE_CURRENT_BINARY_DIR}/${ulp_ps_source}.lst")
        list(APPEND ULP_PS_SOURCES ${ulp_ps_output})
    endforeach()

    target_link_options(${ULP_APP_NAME} PRIVATE -Map=${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}.map)
    target_sources(${ULP_APP_NAME} PRIVATE ${ULP_PS_SOURCES})

endif()

if(ULP_COCPU_IS_LP_CORE)
    set(ULP_BASE_ADDR "0x0")
else()
    set(ULP_BASE_ADDR "0x50000000")
endif()

# Dump the list of global symbols in a convenient format
add_custom_command(OUTPUT ${ULP_APP_NAME}.sym
                   COMMAND ${CMAKE_NM} -f posix -g $<TARGET_FILE:${ULP_APP_NAME}> > ${ULP_APP_NAME}.sym
                   DEPENDS ${ULP_APP_NAME}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Dump the binary for inclusion into the project
add_custom_command(OUTPUT ${ULP_APP_NAME}.bin
                   COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${ULP_APP_NAME}> ${ULP_APP_NAME}.bin
                   DEPENDS ${ULP_APP_NAME}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(OUTPUT ${ULP_APP_NAME}.ld ${ULP_APP_NAME}.h
                   COMMAND ${ULP_MAP_GEN} -s ${ULP_APP_NAME}.sym -o ${ULP_APP_NAME} --base ${ULP_BASE_ADDR}
                   DEPENDS ${ULP_APP_NAME}.sym
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Building the component separately from the project should result in
# ULP files being built.
add_custom_target(build
                DEPENDS ${ULP_APP_NAME} ${ULP_APP_NAME}.bin ${ULP_APP_NAME}.sym
                        ${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}.ld
                        ${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}.h
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
