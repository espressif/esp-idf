if(IDF_TARGET STREQUAL "esp32")
    idf_component_register(SRC_DIRS .
                        PRIV_INCLUDE_DIRS .
                        PRIV_REQUIRES unity test_utils nvs_flash ulp esp_common
                        )
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u ld_include_test_dport_xt_highint5")
endif()

