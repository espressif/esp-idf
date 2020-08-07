idf_component_register(SRCS "test_main.c"
                       INCLUDE_DIRS ".")

if(NOT CONFIG_RSOURCE_EXTRA_CONFIG)
    message(FATAL_ERROR "RSOURCE config not included")
endif()
