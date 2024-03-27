set(PROJECT_NAME "modbus_tcp_slave")

idf_component_register(SRCS "tcp_slave.c"
                        INCLUDE_DIRS ".")
