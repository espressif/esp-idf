idf_component_register(SRCS "test_perfmon_main.c" "test_perfmon.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES perfmon xtensa unity
                    WHOLE_ARCHIVE)
