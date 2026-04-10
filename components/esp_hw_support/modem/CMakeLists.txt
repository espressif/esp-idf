idf_build_get_property(target IDF_TARGET)
idf_build_get_property(non_os_build NON_OS_BUILD)

if(non_os_build OR NOT CONFIG_SOC_MODEM_CLOCK_SUPPORTED)
    return()
endif()

set(srcs)

if(CONFIG_SOC_MODEM_SUPPORT_ETM)
    list(APPEND srcs "modem_etm.c")
endif()

if(CONFIG_SOC_MODEM_CLOCK_SUPPORTED)
    list(APPEND srcs "modem_clock.c")
    list(APPEND srcs "port/${target}/modem_clock_impl.c")
endif()

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")
target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
