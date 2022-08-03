set(srcs "test_app_main.c"
         "test_i2s.c"
         "test_i2s_iram.c")

idf_component_register(SRCS ${srcs}
                       WHOLE_ARCHIVE)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
