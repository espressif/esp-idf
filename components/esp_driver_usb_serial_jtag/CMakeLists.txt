idf_build_get_property(target IDF_TARGET)

set(srcs)
set(include "include")

# USB Serial JTAG related source files
if(CONFIG_SOC_USB_SERIAL_JTAG_SUPPORTED)
    list(APPEND srcs "src/usb_serial_jtag.c"
                     "src/usb_serial_jtag_connection_monitor.c")
endif()

if(${target} STREQUAL "linux")
    set(priv_requires esp_ringbuf esp_timer)
else()
    set(priv_requires esp_driver_gpio esp_ringbuf esp_pm esp_timer)
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${include}
                    PRIV_REQUIRES "${priv_requires}"
                    LDFRAGMENTS "linker.lf"
                    )

if(CONFIG_VFS_SUPPORT_IO AND CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED)
    target_link_libraries(${COMPONENT_LIB} PUBLIC idf::vfs)
    target_sources(${COMPONENT_LIB} PRIVATE "src/usb_serial_jtag_vfs.c")
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u usb_serial_jtag_vfs_include_dev_init")
endif()

# If system needs to monitor USJ connection status, then usb_serial_jtag_connection_monitor object file has to be linked
# to the binary, to allow tick hook to be registered
if(CONFIG_USJ_NO_AUTO_LS_ON_CONNECTION OR CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u usb_serial_jtag_connection_monitor_include")
endif()
