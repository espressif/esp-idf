idf_component_register(SRCS "test_app_main.c" "test_console.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity console
                       WHOLE_ARCHIVE)
