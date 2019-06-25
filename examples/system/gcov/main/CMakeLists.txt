idf_component_register(SRCS "gcov_example.c"
                            "gcov_example_func.c"
                    INCLUDE_DIRS ".")

set_source_files_properties(gcov_example.c
    PROPERTIES COMPILE_FLAGS
    --coverage)
