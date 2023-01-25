# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)
idf_component_register(SRCS "modbus_params.c"
                    INCLUDE_DIRS "include")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
