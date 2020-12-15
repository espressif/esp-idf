idf_component_register(SRCS "mqtt_tcp_example.cpp"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-std=gnu++17")
