idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    set(ldfragments)
else()
    set(ldfragments common.lf soc.lf linker.lf)
endif()

set(srcs "src/esp_err_to_name.c")

idf_component_register(SRCS "${srcs}"
                        INCLUDE_DIRS include
                        LDFRAGMENTS ${ldfragments})

# Note: LINK_INTERFACE_MULTIPLICITY is needed to break circular static-library
# dependencies in the component graph. The core cycle is:
#   esp_system -> bootloader_support -> efuse -> esp_system
# (efuse registers an ESP_SYSTEM_INIT_FN and calls esp_restart(), both of which
# pull in esp_system).  Several more cycles exist through esp_partition, esp_mm,
# and mbedtls.  All cycle-causing deps are already PRIV_REQUIRES, so the
# multiplicity property on this universal dependency is the established fix.
set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY LINK_INTERFACE_MULTIPLICITY 4)

if(CONFIG_IDF_TARGET_LINUX AND NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    # On Linux, provide a linker script that collects specific sections entries (e.g., .esp_err_msg_tbl)
    # and place them in the final binary, since the linker script used for Linux build doesn't have the usual
    # memory layout and section placement as in embedded targets.
    target_link_options(${COMPONENT_LIB} INTERFACE "-Wl,-T,${CMAKE_CURRENT_SOURCE_DIR}/linux/sections.ld")
endif()

# Register error codes defined in esp_common's own headers
idf_define_esp_err_codes(HEADERS include/esp_err.h)
