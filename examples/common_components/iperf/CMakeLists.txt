idf_component_register(SRCS "iperf.c"
                    INCLUDE_DIRS "include"
                    REQUIRES lwip
                    PRIV_REQUIRES esp_timer)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
