idf_build_get_property(target IDF_TARGET)

set(srcs)
set(public_include "include")
if(CONFIG_SOC_SYSTIMER_SUPPORTED)
    list(APPEND public_include "${target}/include")
endif()

if(CONFIG_SOC_SYSTIMER_SUPPORTED)
    if(CONFIG_HAL_SYSTIMER_USE_ROM_IMPL)
        list(APPEND srcs "rom_patch.c")
        # ESP32C2 revision < 2.0 needs additional ROM patches
        if(CONFIG_IDF_TARGET_ESP32C2 AND (CONFIG_ESP32C2_REV_MIN_FULL LESS 200))
            list(APPEND srcs "esp32c2/rom_patch_rev1.c")
        endif()
    else()
        list(APPEND srcs "systimer_hal.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES soc hal esp_rom
                       LDFRAGMENTS linker.lf)

# Link the ROM SYSTIMER HAL implementation linker script if selected
if(CONFIG_HAL_SYSTIMER_USE_ROM_IMPL)
    target_linker_script(${COMPONENT_LIB} INTERFACE "${target}/rom.systimer.ld")
endif()
