idf_build_get_property(target IDF_TARGET)

if(CONFIG_EFUSE_VIRTUAL)
    message(STATUS "Efuse virtual mode is enabled. If Secure boot or Flash encryption is on"
        " it does not provide any security. FOR TESTING ONLY!")
endif()

if(EXISTS "${COMPONENT_DIR}/${target}")
    include(${COMPONENT_DIR}/${target}/sources.cmake)
    spaces2list(EFUSE_SOC_SRCS)
    set(include_dirs include ${target}/include)
    set(private_include private_include ${target}/private_include)
    add_prefix(srcs "${target}/" ${EFUSE_SOC_SRCS})
endif()

if(CONFIG_SOC_EFUSE_KEY_PURPOSE_FIELD)
    set(type "with_key_purposes")
else()
    if(CONFIG_SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK)
        set(type "without_key_purposes/one_key_block")
    else()
        set(type "without_key_purposes/three_key_blocks")
    endif()
endif()

list(APPEND srcs "src/esp_efuse_api.c"
                 "src/esp_efuse_fields.c"
                 "src/esp_efuse_utility.c"
                 "src/efuse_controller/keys/${type}/esp_efuse_api_key.c")

if(BOOTLOADER_BUILD)
    idf_component_register(SRCS "${srcs}"
                    PRIV_REQUIRES bootloader_support soc spi_flash
                    INCLUDE_DIRS "${include_dirs}"
                    PRIV_INCLUDE_DIRS "${private_include}")
else()
    list(APPEND srcs "src/esp_efuse_startup.c")
    if(${target} STREQUAL "linux")
        set(priv_requires soc spi_flash esp_system esp_partition)
    else()
        set(priv_requires bootloader_support soc spi_flash esp_system esp_partition esp_app_format)
    endif()
    idf_component_register(SRCS "${srcs}"
                    PRIV_REQUIRES "${priv_requires}"
                    INCLUDE_DIRS "${include_dirs}"
                    PRIV_INCLUDE_DIRS "${private_include}")
    if(NOT ${target} STREQUAL "linux")
        # Forces the linker to include esp_efuse_startup.c
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_efuse_startup_include_func")
    endif()
endif()

if(target)
    set(TOOL_TARGET -t ${target})
endif()

set(GEN_EFUSE_TABLE_ARG ${TOOL_TARGET} --max_blk_len ${CONFIG_EFUSE_MAX_BLK_LEN})

idf_build_get_property(python PYTHON)

###################
# Make common files esp_efuse_table.c and include/esp_efuse_table.h files.
set(EFUSE_COMMON_TABLE_CSV_PATH "${COMPONENT_DIR}/${target}/esp_efuse_table.csv")

add_custom_target(efuse-common-table COMMAND "${python}"
                 "${CMAKE_CURRENT_SOURCE_DIR}/efuse_table_gen.py"
                 ${EFUSE_COMMON_TABLE_CSV_PATH} ${GEN_EFUSE_TABLE_ARG})
add_deprecated_target_alias(efuse_common_table efuse-common-table)

###################
# Make custom files project/main/esp_efuse_custom_table.c and project/main/include/esp_efuse_custom_table.h files.
# Path to CSV file is relative to project path for custom CSV files.
if(${CONFIG_EFUSE_CUSTOM_TABLE})
    # Custom filename expands any path relative to the project
    idf_build_get_property(project_dir PROJECT_DIR)
    get_filename_component(EFUSE_CUSTOM_TABLE_CSV_PATH "${CONFIG_EFUSE_CUSTOM_TABLE_FILENAME}"
                           ABSOLUTE BASE_DIR "${project_dir}")
    add_custom_target(efuse-custom-table COMMAND "${python}" "${CMAKE_CURRENT_SOURCE_DIR}/efuse_table_gen.py"
                           ${EFUSE_COMMON_TABLE_CSV_PATH} ${EFUSE_CUSTOM_TABLE_CSV_PATH} ${GEN_EFUSE_TABLE_ARG})
    add_deprecated_target_alias(efuse_custom_table efuse-custom-table)
else()
    add_custom_target(efuse-custom-table COMMAND)
    add_deprecated_target_alias(efuse_custom_table efuse-custom-table)
endif()#if(${CONFIG_EFUSE_CUSTOM_TABLE})

add_custom_target(show-efuse-table COMMAND "${python}"
                  "${CMAKE_CURRENT_SOURCE_DIR}/efuse_table_gen.py"
                  ${EFUSE_COMMON_TABLE_CSV_PATH} ${EFUSE_CUSTOM_TABLE_CSV_PATH} ${GEN_EFUSE_TABLE_ARG} "--info")
add_deprecated_target_alias(show_efuse_table show-efuse-table)

###################
# Generates files for unit test. This command is run manually.
set(EFUSE_TEST_TABLE_CSV_PATH "${COMPONENT_DIR}/test/esp_efuse_test_table.csv")
add_custom_target(efuse_test_table COMMAND "${python}"
                  "${CMAKE_CURRENT_SOURCE_DIR}/efuse_table_gen.py"
                  ${EFUSE_TEST_TABLE_CSV_PATH} ${GEN_EFUSE_TABLE_ARG})
