idf_build_get_property(target IDF_TARGET)

if(EXISTS "${COMPONENT_DIR}/${target}")
    include(${COMPONENT_DIR}/${target}/sources.cmake)
    spaces2list(EFUSE_SOC_SRCS)
    set(include_dirs include ${target}/include)
    add_prefix(srcs "${target}/" ${EFUSE_SOC_SRCS})
    list(APPEND srcs "src/${target}/esp_efuse_api.c"
                     "src/${target}/esp_efuse_fields.c"
                     "src/${target}/esp_efuse_utility.c")
endif()

list(APPEND srcs "src/esp_efuse_api.c"
                 "src/esp_efuse_fields.c"
                 "src/esp_efuse_utility.c")

idf_component_register(SRCS "${srcs}"
                    PRIV_REQUIRES bootloader_support soc spi_flash
                    INCLUDE_DIRS "${include_dirs}"
                    PRIV_INCLUDE_DIRS private_include)

if(target)
    set(TOOL_TARGET -t ${target})
endif()

set(GEN_EFUSE_TABLE_ARG ${TOOL_TARGET} --max_blk_len ${CONFIG_EFUSE_MAX_BLK_LEN})

idf_build_get_property(python PYTHON)

###################
# Make common files esp_efuse_table.c and include/esp_efuse_table.h files.
set(EFUSE_COMMON_TABLE_CSV_PATH "${COMPONENT_DIR}/${target}/esp_efuse_table.csv")

add_custom_target(efuse_common_table COMMAND "${python}" "${CMAKE_CURRENT_SOURCE_DIR}/efuse_table_gen.py" ${EFUSE_COMMON_TABLE_CSV_PATH} ${GEN_EFUSE_TABLE_ARG})

###################
# Make custom files project/main/esp_efuse_custom_table.c and project/main/include/esp_efuse_custom_table.h files.
# Path to CSV file is relative to project path for custom CSV files.
if(${CONFIG_EFUSE_CUSTOM_TABLE})
    # Custom filename expands any path relative to the project
    idf_build_get_property(project_dir PROJECT_DIR)
    get_filename_component(EFUSE_CUSTOM_TABLE_CSV_PATH "${CONFIG_EFUSE_CUSTOM_TABLE_FILENAME}" ABSOLUTE BASE_DIR "${project_dir}")
    add_custom_target(efuse_custom_table COMMAND "${python}" "${CMAKE_CURRENT_SOURCE_DIR}/efuse_table_gen.py" ${EFUSE_COMMON_TABLE_CSV_PATH} ${EFUSE_CUSTOM_TABLE_CSV_PATH} ${GEN_EFUSE_TABLE_ARG})
else()
    add_custom_target(efuse_custom_table COMMAND)
endif()#if(${CONFIG_EFUSE_CUSTOM_TABLE})

add_custom_target(show_efuse_table COMMAND "${python}" "${CMAKE_CURRENT_SOURCE_DIR}/efuse_table_gen.py" ${EFUSE_COMMON_TABLE_CSV_PATH} ${EFUSE_CUSTOM_TABLE_CSV_PATH} ${GEN_EFUSE_TABLE_ARG} "--info")

###################
# Generates files for unit test. This command is run manually.
set(EFUSE_TEST_TABLE_CSV_PATH "${COMPONENT_DIR}/test/esp_efuse_test_table.csv")
add_custom_target(efuse_test_table COMMAND "${python}" "${CMAKE_CURRENT_SOURCE_DIR}/efuse_table_gen.py" ${EFUSE_TEST_TABLE_CSV_PATH} ${GEN_EFUSE_TABLE_ARG})
