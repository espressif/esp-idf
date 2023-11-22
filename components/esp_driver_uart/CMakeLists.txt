set(srcs)
set(public_include "include")
if(CONFIG_SOC_UART_SUPPORTED)
    list(APPEND srcs "src/uart.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES esp_pm esp_driver_gpio esp_ringbuf
                       LDFRAGMENTS "linker.lf"
                      )
