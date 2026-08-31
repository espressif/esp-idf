idf_component_register(SRCS "some_funcs.c"
                    INCLUDE_DIRS ".")

set_source_files_properties(some_funcs.c
    PROPERTIES COMPILE_FLAGS
    --coverage)
