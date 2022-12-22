set(PROJECT_NAME "modbus_master")

idf_component_register(SRCS "master.c"
                        INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
