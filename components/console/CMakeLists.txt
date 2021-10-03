idf_component_register(SRCS "commands.c"
                            "esp_console_repl.c"
                            "split_argv.c"
                            "argtable3/argtable3.c"
                            "linenoise/linenoise.c"
                    INCLUDE_DIRS "."
                    REQUIRES vfs
                    PRIV_REQUIRES driver)

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
    set_source_files_properties(argtable3/argtable3.c PROPERTIES COMPILE_FLAGS -Wno-clobbered)
endif()
