idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "esp32c2")
    # IDF-5046
        return()
endif()

idf_component_register(SRC_DIRS "."
                    PRIV_REQUIRES cmock test_utils mdns)
