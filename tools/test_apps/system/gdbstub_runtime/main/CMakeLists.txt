idf_component_register(SRCS "test_app_main.c"
                       INCLUDE_DIRS ""
                       REQUIRES esp_gdbstub)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-unused-label")
