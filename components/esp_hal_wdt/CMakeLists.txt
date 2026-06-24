idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_include "include" "${target}/include")

if(CONFIG_SOC_WDT_SUPPORTED)
    list(APPEND srcs "${target}/mwdt_periph.c")
endif()

if(CONFIG_SOC_WDT_SUPPORTED)
    if(CONFIG_HAL_WDT_USE_ROM_IMPL)
        list(APPEND srcs "rom_patch.c")
    else()
        list(APPEND srcs "wdt_hal_iram.c")
    endif()
endif()

if(CONFIG_SOC_XT_WDT_SUPPORTED)
    list(APPEND srcs "xt_wdt_hal.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES soc hal esp_rom esp_hal_timg
                       LDFRAGMENTS linker.lf)

# Link the ROM WDT HAL implementation linker script if selected
if(CONFIG_HAL_WDT_USE_ROM_IMPL)
    target_linker_script(${COMPONENT_LIB} INTERFACE "${target}/rom.wdt.ld")
endif()
