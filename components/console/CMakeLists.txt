idf_component_register(SRCS "commands.c"
                            "split_argv.c"
                            "argtable3/argtable3.c"
                            "linenoise/linenoise.c"
                    INCLUDE_DIRS "."
                    REQUIRES vfs)
