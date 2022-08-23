if( IDF_TARGET STREQUAL "esp32s2" )
    idf_component_register(SRCS "esp32s2/test_memprot_main.c" "esp32s2/test_panic.c"
                    INCLUDE_DIRS "")
elseif( IDF_TARGET STREQUAL "esp32c3" )
    idf_component_register(SRCS "esp32c3/test_memprot_main.c" "esp32c3/test_panic.c" "esp32c3/return_from_panic.S"
                    INCLUDE_DIRS "")
elseif( IDF_TARGET STREQUAL "esp32s3" )
    idf_component_register(SRCS "esp32s3/test_memprot_main.c" "esp32s3/test_panic.c"
                    INCLUDE_DIRS "")
endif()
