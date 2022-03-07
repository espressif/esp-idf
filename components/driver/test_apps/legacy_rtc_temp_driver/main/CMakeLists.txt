set(srcs "test_app_main.c"
         "test_rtc_temp_driver.c")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES driver unity)

target_link_libraries(${COMPONENT_LIB} INTERFACE "-u test_app_include_rtc_temp_driver")
