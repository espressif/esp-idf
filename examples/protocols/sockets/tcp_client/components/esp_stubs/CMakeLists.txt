if(${IDF_TARGET} STREQUAL "linux")
    idf_component_register(SRCS
                        esp_stubs/esp_stubs.c
                        INCLUDE_DIRS . include/stubs
                        REQUIRES main)
endif()
