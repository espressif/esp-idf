set(srcs "test_app_main.c"
         "test_temperature_sensor.c")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES driver unity)

target_link_libraries(${COMPONENT_LIB} INTERFACE "-u test_app_include_temperature_sensor")
