# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

idf_build_get_property(target IDF_TARGET)
idf_build_get_property(sdkconfig_header SDKCONFIG_HEADER)
idf_build_get_property(config_dir CONFIG_DIR)

# -------------------------------- esp_tee.ld --------------------------------

set(ld_input  "${CMAKE_CURRENT_LIST_DIR}/${target}/esp_tee.ld.in")
set(ld_output "${CMAKE_CURRENT_BINARY_DIR}/ld/esp_tee.ld")

target_linker_script(${COMPONENT_LIB} INTERFACE "${ld_output}")

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/ld")

# Preprocess esp_tee.ld.in linker script to include configuration, becomes esp_tee.ld
add_custom_command(
    OUTPUT ${ld_output}
    COMMAND "${CMAKE_C_COMPILER}" -C -P -x c -E -o ${ld_output} -I ${config_dir}
            -I "${CMAKE_CURRENT_LIST_DIR}" ${ld_input}
    MAIN_DEPENDENCY ${ld_input}
    DEPENDS ${sdkconfig_header}
    COMMENT "Generating esp_tee.ld linker script..."
    VERBATIM)

add_custom_target(esp_tee_ld DEPENDS ${ld_output})
add_dependencies(${COMPONENT_LIB} esp_tee_ld)
