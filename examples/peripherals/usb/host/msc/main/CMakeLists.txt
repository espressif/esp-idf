idf_component_register(SRCS "msc_example_main.c"
                    INCLUDE_DIRS ""
                    REQUIRES usb msc fatfs vfs)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
