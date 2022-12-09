if(IDF_TARGET STREQUAL "esp32")
    idf_component_register(SRC_DIRS .
                        PRIV_INCLUDE_DIRS . ${CMAKE_CURRENT_BINARY_DIR}
                        PRIV_REQUIRES unity test_utils nvs_flash ulp esp_common
                        )

    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test_tjpgd_logo.h"
                    COMMAND xxd -i "logo.jpg" "${CMAKE_CURRENT_BINARY_DIR}/test_tjpgd_logo.h"
                    WORKING_DIRECTORY ${COMPONENT_DIR}
                    DEPENDS "${CMAKE_CURRENT_LIST_DIR}/logo.jpg")

    add_custom_target(esp32_test_logo DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/test_tjpgd_logo.h")

    add_dependencies(${COMPONENT_LIB} esp32_test_logo)

    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u ld_include_test_dport_xt_highint5")
endif()

