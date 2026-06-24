set(srcs "power_management_example_main.c")
set(includes ".")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES esp_pm
                       INCLUDE_DIRS ${includes})
