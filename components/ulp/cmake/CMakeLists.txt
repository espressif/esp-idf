cmake_minimum_required(VERSION 3.5)

include(${IDF_PATH}/tools/cmake/utilities.cmake)
project(${ULP_APP_NAME} ASM C)

set(version_pattern "[a-z0-9\.-]+")

# Check assembler version
execute_process(
    COMMAND ${CMAKE_ASM_COMPILER} --version
    OUTPUT_VARIABLE as_output
    ERROR_QUIET)

string(REGEX MATCH "\\(GNU Binutils\\) (${version_pattern})" as_version ${as_output})
set(as_version ${CMAKE_MATCH_1})

message(STATUS "Building ULP app ${ULP_APP_NAME}")
message(STATUS "ULP assembler version: ${as_version}")

# Check the supported assembler version
file(STRINGS ${IDF_PATH}/components/ulp/toolchain_ulp_version.mk version_file_contents)
string(REGEX MATCH "SUPPORTED_ULP_ASSEMBLER_VERSION = (${version_pattern})" as_supported_version ${version_file_contents})
set(as_supported_version ${CMAKE_MATCH_1})

if(NOT as_version STREQUAL as_supported_version)
    message(WARNING "WARNING: ULP assembler version ${as_version} is not supported. Expected to see version: \
                    ${as_supported_version}. Please check ESP-IDF ULP setup instructions and update \
                    the toolchain, or proceed at your own risk.")
endif()

set(ULP_MAP_GEN ${PYTHON} ${IDF_PATH}/components/ulp/esp32ulp_mapgen.py)
set(ULP_LD_TEMPLATE ${IDF_PATH}/components/ulp/ld/esp32.ulp.ld)

get_filename_component(sdkconfig_dir ${SDKCONFIG_HEADER} DIRECTORY)

foreach(include ${COMPONENT_INCLUDES})
    list(APPEND component_includes -I${include})
endforeach()

list(APPEND ULP_PREPROCESSOR_ARGS ${component_includes})
list(APPEND ULP_PREPROCESSOR_ARGS -I${COMPONENT_DIR})
list(APPEND ULP_PREPROCESSOR_ARGS -I${sdkconfig_dir})

include_directories(${component_includes})

list(APPEND ULP_PREPROCESSOR_ARGS -D__ASSEMBLER__)

# Preprocess linker script, pre-linking
get_filename_component(ULP_LD_SCRIPT ${ULP_LD_TEMPLATE} NAME)
add_custom_command( OUTPUT ${ULP_LD_SCRIPT}
                    COMMAND ${CMAKE_C_COMPILER} -E -P -xc -o ${ULP_LD_SCRIPT} ${ULP_PREPROCESSOR_ARGS} ${ULP_LD_TEMPLATE}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    DEPENDS ${ULP_LD_TEMPLATE} ${SDKCONFIG_HEADER}
                    VERBATIM)
add_custom_target(${ULP_APP_NAME}_ld_script
                    DEPENDS ${ULP_LD_SCRIPT}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

foreach(ulp_s_source ${ULP_S_SOURCES})
    get_filename_component(ulp_ps_source ${ulp_s_source} NAME_WE)
    set(ulp_ps_output ${CMAKE_CURRENT_BINARY_DIR}/${ulp_ps_source}.ulp.S)
    # Generate preprocessed assembly files.
    add_custom_command( OUTPUT ${ulp_ps_output}
                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                        COMMAND ${CMAKE_C_COMPILER} -E -P -xc ${ULP_PREPROCESSOR_ARGS} -o ${ulp_ps_output} ${ulp_s_source}
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

# Dump the list of global symbols in a convenient format
add_custom_command( OUTPUT ${ULP_APP_NAME}.sym
                    COMMAND ${CMAKE_NM} -g -f posix $<TARGET_FILE:${ULP_APP_NAME}> > ${ULP_APP_NAME}.sym
                    DEPENDS ${ULP_APP_NAME}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Dump the binary for inclusion into the project
add_custom_command( OUTPUT ${ULP_APP_NAME}.bin
                    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${ULP_APP_NAME}> ${ULP_APP_NAME}.bin
                    DEPENDS ${ULP_APP_NAME}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command( OUTPUT ${ULP_APP_NAME}.ld ${ULP_APP_NAME}.h
                    COMMAND ${ULP_MAP_GEN} -s ${ULP_APP_NAME}.sym -o ${ULP_APP_NAME}
                    DEPENDS ${ULP_APP_NAME}.sym
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Building the component separately from the project should result in
# ULP files being built.
add_custom_target(build
                DEPENDS ${ULP_APP_NAME} ${ULP_APP_NAME}.bin ${ULP_APP_NAME}.sym
                        ${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}.ld
                        ${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}.h
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${ULP_APP_NAME} -T${CMAKE_CURRENT_BINARY_DIR}/${ULP_LD_SCRIPT} -Map=${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}.map)
