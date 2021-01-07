if(IDF_TARGET STREQUAL "esp32c3")
    idf_component_register(SRC_DIRS .
                        INCLUDE_DIRS . ${CMAKE_CURRENT_BINARY_DIR}
                        REQUIRES unity test_utils esp_common mbedtls
                        )

    idf_build_set_property(COMPILE_DEFINITIONS "-DESP_TIMER_DYNAMIC_OVERFLOW_VAL" APPEND)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u ld_include_test_dport_xt_highint5")
endif()
