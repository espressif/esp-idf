set(PROJECT_NAME "modbus_slave")

idf_component_register(SRCS "slave.c"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
