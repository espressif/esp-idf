idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs "src/gpio.c"
         "src/gpio_glitch_filter_ops.c"
         "src/rtc_io.c"
         )
set(public_include "include")

if(CONFIG_SOC_DEDICATED_GPIO_SUPPORTED)
    list(APPEND srcs "src/dedic_gpio.c")
endif()

if(CONFIG_SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER)
    list(APPEND srcs "src/gpio_pin_glitch_filter.c")
endif()

if(CONFIG_SOC_GPIO_FLEX_GLITCH_FILTER_NUM GREATER 0)
    list(APPEND srcs "src/gpio_flex_glitch_filter.c")
endif()

if(CONFIG_SOC_GPIO_SUPPORT_ETM)
    list(APPEND srcs "src/gpio_etm.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES esp_pm
                       LDFRAGMENTS "linker.lf"
                      )
