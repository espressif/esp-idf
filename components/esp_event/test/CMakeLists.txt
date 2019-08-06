set(srcdirs .)
if(IDF_TARGET STREQUAL "esp32")
    list(APPEND srcdirs "esp32")
endif()

idf_component_register(SRC_DIRS ${srcdirs}
                       PRIV_INCLUDE_DIRS . ../private_include
                       PRIV_REQUIRES unity test_utils esp_event driver)
