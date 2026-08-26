idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    idf_component_register(INCLUDE_DIRS include
                           SRCS linux/tapio.c linux_connect.c lwip/tapif.c
                           PRIV_REQUIRES esp_netif lwip)
else()
    message(FATAL_ERROR "This component is currently only supported for linux target")
endif()
