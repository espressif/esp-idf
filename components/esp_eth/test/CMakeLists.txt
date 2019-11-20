idf_build_get_property(target IDF_TARGET)
# Currently we only have unit test for esp32
if(${target} STREQUAL "esp32")
    idf_component_register(SRC_DIRS .
                           INCLUDE_DIRS .
                           PRIV_REQUIRES unity test_utils esp_eth)
endif()
