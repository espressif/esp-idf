set(COMPONENT_SRCS "gcov_example.c"
                   "gcov_example_func.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()

set_source_files_properties(gcov_example.c
    PROPERTIES COMPILE_FLAGS
    --coverage)
