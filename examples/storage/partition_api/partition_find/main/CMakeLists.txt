idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES ../partitions_example.csv)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
