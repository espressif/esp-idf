idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    # TODO: Remove this once all components have their requirements explicitly defined
    # and using set(COMPONENTS main).
    idf_component_register()
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
                       REQUIRES esp_hal_gpio
                       LDFRAGMENTS "linker.lf"
                      )

# Only the glitch filter drivers use esp_pm, to hold a PM lock while enabled.
if(CONFIG_PM_ENABLE AND
   (CONFIG_SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER OR CONFIG_SOC_GPIO_FLEX_GLITCH_FILTER_NUM GREATER 0))
    if(IDF_BUILD_V2)
        idf_component_include(esp_pm)
        target_link_libraries(${COMPONENT_LIB} PRIVATE idf::esp_pm)
    else()
        # V1 cannot add a requirement conditionally, because requirements are
        # resolved before Kconfig is available. Linking the already-present
        # component is enough: a central component keeps esp_pm in the build.
        idf_component_optional_requires(PRIVATE esp_pm)
    endif()
endif()
