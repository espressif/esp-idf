idf_build_get_property(non_os_build NON_OS_BUILD)

if(non_os_build OR NOT CONFIG_SOC_LIGHT_SLEEP_SUPPORTED)
    return()
endif()

set(srcs)

if(CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP OR
    (CONFIG_ESP32P4_SELECTS_REV_LESS_V3 AND CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP))
    list(APPEND srcs "port/${target}/sleep_cpu.c")
    if(CONFIG_PM_CPU_RETENTION_DYNAMIC)
        list(APPEND srcs "port/${target}/sleep_cpu_dynamic.c")
    elseif(CONFIG_PM_CPU_RETENTION_STATIC)
        list(APPEND srcs "port/${target}/sleep_cpu_static.c")
    endif()
    if(CONFIG_SOC_PM_CPU_RETENTION_BY_SW)
        list(APPEND srcs "port/${target}/sleep_cpu_asm.S")
        set_property(TARGET ${COMPONENT_LIB}
                    APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-u rv_core_critical_regs_save")
        set_property(TARGET ${COMPONENT_LIB}
                    APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-u rv_core_critical_regs_restore")
    endif()
endif()

if(CONFIG_P4_REV3_MSPI_CRASH_AFTER_POWER_UP_WORKAROUND)
    list(APPEND srcs "port/esp32p4/p4_rev3_mspi_workaround.S")
    set_property(TARGET ${COMPONENT_LIB}
                APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-u p4_rev3_mspi_workaround")
endif()

if((CONFIG_SOC_PM_SUPPORT_MODEM_PD OR CONFIG_SOC_PM_SUPPORT_TOP_PD) AND CONFIG_SOC_PAU_SUPPORTED)
    list(APPEND srcs "port/${target}/sleep_clock.c")
endif()

if(CONFIG_SOC_PM_SUPPORT_PMU_MODEM_STATE)
    list(APPEND srcs "port/${target}/sleep_modem_state.c")
endif()

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
