set(priv_req mbedtls)
set(priv_inc_dir "src/util")
set(requires http_parser esp_event)
if(NOT ${IDF_TARGET} STREQUAL "linux")
    list(APPEND priv_req lwip esp_timer)
    list(APPEND priv_inc_dir "src/port/esp32")
else()
    list(APPEND priv_inc_dir "src/port/linux")
    list(APPEND priv_req pthread)
endif()

idf_component_register(SRCS "src/httpd_main.c"
                            "src/httpd_parse.c"
                            "src/httpd_sess.c"
                            "src/httpd_txrx.c"
                            "src/httpd_uri.c"
                            "src/httpd_ws.c"
                            "src/util/ctrl_sock.c"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS ${priv_inc_dir}
                    REQUIRES ${requires}
                    PRIV_REQUIRES ${priv_req})

if(${IDF_TARGET} STREQUAL "linux")
    find_library(LIB_BSD bsd)
    if(LIB_BSD)
        target_link_libraries(${COMPONENT_LIB} PRIVATE ${LIB_BSD})
    elseif(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        message(WARNING "Missing LIBBSD library. Install libbsd-dev package and/or check linker directories.")
    endif()
endif()
