set(srcdirs . param_test)
if(IDF_TARGET STREQUAL "esp32")
    list(APPEND srcdirs "esp32")
endif()

idf_component_register(SRC_DIRS ${srcdirs}
                       INCLUDE_DIRS include param_test/include
                       REQUIRES unity test_utils driver nvs_flash
                       )
