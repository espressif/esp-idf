if(IDF_TARGET STREQUAL "esp32s2")
idf_component_register(SRCS "waterproof_example_main.c"
                       INCLUDE_DIRS ".")
else()
    message(FATAL_ERROR "Touch element waterproof example only available on esp32s2 now")
endif()
