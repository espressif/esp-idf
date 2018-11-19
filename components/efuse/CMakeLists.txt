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


###################
# Make common files esp_efuse_table.c and include/esp_efuse_table.h files.
# The generated files are used in the bootloader and application space.
# To generate new *.c/*.h files run the command manually "make efuse_common_table".
set(EFUSE_COMMON_TABLE_CSV_PATH "${COMPONENT_PATH}/${SOC_NAME}/esp_efuse_table.csv")
add_custom_target(efuse_common_table COMMAND "${PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/efuse_table_gen.py" ${EFUSE_COMMON_TABLE_CSV_PATH})


# custom gen header files
if(${CONFIG_EFUSE_CUSTOM_TABLE})
    # Custom filename expands any path relative to the project
    get_filename_component(EFUSE_CUSTOM_TABLE_CSV_PATH "${CONFIG_EFUSE_CUSTOM_TABLE_FILENAME}" ABSOLUTE BASE_DIR "${PROJECT_PATH}")

    add_custom_target(efuse_custom_table ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Efuse CSV ${EFUSE_CUSTOM_TABLE_CSV_PATH} does not exist. Either change efuse table file in menuconfig or create this input file."
        COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_BINARY_DIR}/CMakeCache.txt"
        COMMAND ${CMAKE_COMMAND} -P ${IDF_PATH}/tools/cmake/scripts/fail.cmake)
    
string(REPLACE ".csv" ".c" HEADER_CUSTOM ${EFUSE_CUSTOM_TABLE_CSV_PATH})
add_custom_command(OUTPUT ${HEADER_CUSTOM}
    COMMAND "${PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/efuse_table_gen.py"
    ${EFUSE_COMMON_TABLE_CSV_PATH} ${EFUSE_CUSTOM_TABLE_CSV_PATH}
    DEPENDS ${EFUSE_CUSTOM_TABLE_CSV_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/efuse_table_gen.py"
    VERBATIM)

if(EXISTS ${EFUSE_CUSTOM_TABLE_CSV_PATH})
    add_custom_target(efuse_custom_table ALL DEPENDS ${HEADER_CUSTOM} ${EFUSE_CUSTOM_TABLE_CSV_PATH})
endif()

endif()#if(${CONFIG_EFUSE_CUSTOM_TABLE})

###################
# Generates files for unit test. This command is run manually.
set(EFUSE_TEST_TABLE_CSV_PATH "${COMPONENT_PATH}/test/esp_efuse_test_table.csv")
add_custom_target(efuse_test_table COMMAND "${PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/efuse_table_gen.py" ${EFUSE_TEST_TABLE_CSV_PATH})
