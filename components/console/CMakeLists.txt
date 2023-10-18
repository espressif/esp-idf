idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is currently not supported by the POSIX/Linux simulator, but we may support it in the
             # future (TODO: IDF-8103)
endif()

set(argtable_srcs argtable3/arg_cmd.c
                  argtable3/arg_date.c
                  argtable3/arg_dbl.c
                  argtable3/arg_dstr.c
                  argtable3/arg_end.c
                  argtable3/arg_file.c
                  argtable3/arg_hashtable.c
                  argtable3/arg_int.c
                  argtable3/arg_lit.c
                  argtable3/arg_rem.c
                  argtable3/arg_rex.c
                  argtable3/arg_str.c
                  argtable3/arg_utils.c
                  argtable3/argtable3.c)


idf_component_register(SRCS "commands.c"
                            "esp_console_repl.c"
                            "split_argv.c"
                            "linenoise/linenoise.c"
                            ${argtable_srcs}
                    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
                    REQUIRES vfs
                    PRIV_REQUIRES driver)
