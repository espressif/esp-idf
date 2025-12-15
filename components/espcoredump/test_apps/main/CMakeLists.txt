idf_component_register(SRCS "test_coredump_main.c"
                            "test_sections.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity espcoredump
                       WHOLE_ARCHIVE)
