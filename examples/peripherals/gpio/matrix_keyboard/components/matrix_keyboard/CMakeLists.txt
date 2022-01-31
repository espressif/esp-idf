set(component_srcs "src/matrix_keyboard.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS ""
                       PRIV_REQUIRES "driver"
                       REQUIRES "")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
