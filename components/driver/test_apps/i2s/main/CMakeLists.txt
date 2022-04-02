set(srcs "test_app_main.c")

if(CONFIG_SOC_I2S_SUPPORTED)
    list(APPEND srcs "test_i2s.c"
                     "test_i2s_legacy.c")
endif()


idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES driver hal soc unity)

if(CONFIG_SOC_I2S_SUPPORTED)
    target_link_libraries(${COMPONENT_LIB} INTERFACE
                          "-u test_app_include_i2s"
                          "-u test_app_include_i2s_legacy")
endif()
