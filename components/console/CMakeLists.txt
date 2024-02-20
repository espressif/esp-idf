idf_build_get_property(target IDF_TARGET)

set(srcs "commands.c"
         "esp_console_common.c"
         "split_argv.c"
         "linenoise/linenoise.c")

if(${target} STREQUAL "linux")
    list(APPEND srcs "esp_console_repl_linux.c")
else()
    list(APPEND srcs "esp_console_repl_chip.c")
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
                    REQUIRES vfs
                    PRIV_REQUIRES esp_driver_uart
                                  esp_driver_usb_serial_jtag
                    )

if(${target} STREQUAL "linux")
    # link bsd library for strlcpy
    find_library(LIB_BSD bsd)
    if(LIB_BSD)
        target_link_libraries(${COMPONENT_LIB} PRIVATE ${LIB_BSD})
    elseif(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        message(WARNING "Missing LIBBSD library. Install libbsd-dev package and/or check linker directories.")
    endif()
endif()
