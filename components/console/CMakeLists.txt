idf_build_get_property(target IDF_TARGET)

set(srcs "commands.c"
         "esp_console_common.c"
         "split_argv.c"
         "linenoise/linenoise.c")

if(${target} STREQUAL "linux")
    list(APPEND srcs "esp_console_repl_linux.c")
    set(requires "")
else()
    list(APPEND srcs "esp_console_repl_chip.c")
    set(requires vfs esp_vfs_console)
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


idf_component_register(SRCS ${srcs}
                            ${argtable_srcs}
                    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
                    PRIV_INCLUDE_DIRS private_include
                    REQUIRES "${requires}"
                    PRIV_REQUIRES esp_driver_uart
                                  esp_driver_usb_serial_jtag
                    )

if(CONFIG_COMPILER_STATIC_ANALYZER AND CMAKE_C_COMPILER_ID STREQUAL "GNU") # TODO IDF-10085
    target_compile_options(${COMPONENT_LIB} PRIVATE "-fno-analyzer")
endif()
