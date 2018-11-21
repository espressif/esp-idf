set(SOC_NAME ${IDF_TARGET})

if(EXISTS "${COMPONENT_PATH}/${SOC_NAME}")
    include(${COMPONENT_PATH}/${SOC_NAME}/sources.cmake)
    spaces2list(EFUSE_SOC_SRCS)
    add_prefix(COMPONENT_SRCS "${SOC_NAME}/" ${EFUSE_SOC_SRCS})
    set(COMPONENT_ADD_INCLUDEDIRS include
                                  ${SOC_NAME}/include)
endif()

list(APPEND COMPONENT_SRCS "src/esp_efuse_api.c"
                           "src/esp_efuse_fields.c"
                           "src/esp_efuse_utility.c")

set(COMPONENT_REQUIRES)
set(COMPONENT_PRIV_REQUIRES bootloader_support)
register_component()

set(GEN_EFUSE_TABLE_ARG --coding_scheme ${CONFIG_EFUSE_CODE_SCHEME})

if(CONFIG_EFUSE_CUSTOM_TABLE_USE_BLK1)
	list(APPEND GEN_EFUSE_TABLE_ARG --custom_table_use_BLK1)
endif()

if(CONFIG_EFUSE_CUSTOM_TABLE_USE_BLK2)
	list(APPEND GEN_EFUSE_TABLE_ARG --custom_table_use_BLK2)
endif()

if(CONFIG_EFUSE_COMMON_TABLE_FIX_SIZE_BLK1_BLK2_DEP_ON_CODE_SCHEME)
	list(APPEND GEN_EFUSE_TABLE_ARG --common_table_fix_size_secure_key_and_encrypt_key)
endif()

###################
# Make common files esp_efuse_table.c and include/esp_efuse_table.h files.
set(EFUSE_COMMON_TABLE_CSV_PATH "${COMPONENT_PATH}/${SOC_NAME}/esp_efuse_table.csv")
set(EFUSE_COMMON_TABLE_OUT_PATH "${COMPONENT_PATH}/${SOC_NAME}/esp_efuse_table.c")

###################
# Make custom files project/main/esp_efuse_custom_table.c and project/main/include/esp_efuse_custom_table.h files.
# Path to CSV file is relative to project path for custom CSV files.
if(${CONFIG_EFUSE_CUSTOM_TABLE})
    # Custom filename expands any path relative to the project
    get_filename_component(EFUSE_CUSTOM_TABLE_CSV_PATH "${CONFIG_EFUSE_CUSTOM_TABLE_FILENAME}" ABSOLUTE BASE_DIR "${PROJECT_PATH}")
    string(REPLACE ".csv" ".c" EFUSE_CUSTOM_TABLE_OUT_PATH ${EFUSE_CUSTOM_TABLE_CSV_PATH})

endif()#if(${CONFIG_EFUSE_CUSTOM_TABLE})

set(GENERATED_SRC ${EFUSE_COMMON_TABLE_OUT_PATH} ${EFUSE_CUSTOM_TABLE_OUT_PATH})
set_source_files_properties(GENERATED_SRC PROPERTIES SYMBOLIC true)
add_custom_command(
    OUTPUT GENERATED_OUTPUT
    BYPRODUCTS ${GENERATED_SRC}
    COMMAND "${PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/efuse_table_gen.py"
    ${EFUSE_COMMON_TABLE_CSV_PATH} ${EFUSE_CUSTOM_TABLE_CSV_PATH} ${GEN_EFUSE_TABLE_ARG}
    DEPENDS "${EFUSE_COMMON_TABLE_CSV_PATH}" "${EFUSE_CUSTOM_TABLE_CSV_PATH}" "${SDKCONFIG_CMAKE}"
    VERBATIM)
add_custom_target(GENERATED_SRC DEPENDS GENERATED_OUTPUT ${GENERATED_SRC})

###################
# Generates files for unit test. This command is run manually.
set(EFUSE_TEST_TABLE_CSV_PATH "${COMPONENT_PATH}/test/esp_efuse_test_table.csv")
add_custom_target(efuse_test_table COMMAND "${PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/efuse_table_gen.py" ${EFUSE_TEST_TABLE_CSV_PATH} ${GEN_EFUSE_TABLE_ARG})
