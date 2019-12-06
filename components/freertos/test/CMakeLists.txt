set(srcdirs .)

if(IDF_TARGET STREQUAL "esp32")
    list(APPEND srcdirs esp32)
endif()


idf_component_register(SRC_DIRS ${src_dirs}
                       INCLUDE_DIRS .
                       REQUIRES unity test_utils
                      )