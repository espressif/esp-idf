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
                    INCLUDE_DIRS "."
                    REQUIRES vfs
                    PRIV_REQUIRES driver)
