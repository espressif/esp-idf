set(srcs)
set(public_include "include")
if(CONFIG_SOC_PCNT_SUPPORTED)
    list(APPEND srcs "src/pulse_cnt.c")
endif()

if(BOOTLOADER_BUILD)
    # Bootloader shall NOT depend on the drivers
    idf_component_register()
else()
    idf_component_register(SRCS ${srcs}
                           INCLUDE_DIRS ${public_include}
                           PRIV_REQUIRES "esp_pm"
                                         "driver" # will be replaced by esp_driver_gpio
                           LDFRAGMENTS "linker.lf"
                          )
endif()
