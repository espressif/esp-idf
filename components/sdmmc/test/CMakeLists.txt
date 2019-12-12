if(IDF_TARGET STREQUAL "esp32")
    idf_component_register(SRC_DIRS "."
                        INCLUDE_DIRS "."
                        REQUIRES unity sdmmc)
endif()