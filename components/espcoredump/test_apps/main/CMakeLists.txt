set(priv_includes "")
set(SRCS "test_coredump_main.c"
         "test_sections.c")
idf_component_get_property(espcoredump_dir espcoredump COMPONENT_DIR)
list(APPEND priv_includes "${espcoredump_dir}/include_core_dump")
idf_component_register(SRCS ${SRCS}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity
                       PRIV_INCLUDE_DIRS ${priv_includes}
                       REQUIRES mbedtls espcoredump
                       WHOLE_ARCHIVE)
