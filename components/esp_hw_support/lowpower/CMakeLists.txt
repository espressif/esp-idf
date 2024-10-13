if(BOOTLOADER_BUILD)
    return()
endif()

set(srcs)

if(CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP OR
    (CONFIG_SOC_CPU_IN_TOP_DOMAIN AND CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP))
    list(APPEND srcs "port/${target}/sleep_cpu.c")
    if(CONFIG_SOC_PM_CPU_RETENTION_BY_SW)
        list(APPEND srcs "port/${target}/sleep_cpu_asm.S")
        set_property(TARGET ${COMPONENT_LIB}
                    APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-u rv_core_critical_regs_save")
        set_property(TARGET ${COMPONENT_LIB}
                    APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-u rv_core_critical_regs_restore")
    endif()
endif()

if(CONFIG_SOC_PM_MMU_TABLE_RETENTION_WHEN_TOP_PD AND CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP)
    list(APPEND srcs "port/${target}/sleep_mmu.c")
endif()

if((CONFIG_SOC_PM_SUPPORT_MODEM_PD OR CONFIG_SOC_PM_SUPPORT_TOP_PD) AND CONFIG_SOC_PAU_SUPPORTED)
    list(APPEND srcs "port/${target}/sleep_clock.c")
endif()

if(CONFIG_SOC_PM_SUPPORT_PMU_MODEM_STATE)
    list(APPEND srcs "port/${target}/sleep_modem_state.c")
endif()

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
