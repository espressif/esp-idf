set(srcs "test_app_main.c"
         "test_gpio.c")

set(include_tests "-u test_app_include_gpio")

if(CONFIG_SOC_DEDICATED_GPIO_SUPPORTED)
    list(APPEND srcs "test_dedicated_gpio.c")
    list(APPEND include_tests "-u test_app_include_dedicated_gpio")
endif()

if(CONFIG_SOC_SIGMADELTA_SUPPORTED)
    list(APPEND srcs "test_sigmadelta.c")
    list(APPEND include_tests "-u test_app_include_sigmadelta")
endif()

idf_component_register(SRCS ${srcs})

target_link_libraries(${COMPONENT_LIB} INTERFACE ${include_tests})
