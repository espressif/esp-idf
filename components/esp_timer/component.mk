#
# Component Makefile
#

ifdef CONFIG_IDF_TARGET_ESP32
    COMPONENT_SRCDIRS := src
    COMPONENT_PRIV_INCLUDEDIRS := private_include

    ifdef CONFIG_ESP_TIMER_IMPL_FRC2
        # FRC2(legacy) timer is suppoted in esp32
        COMPONENT_OBJEXCLUDE := src/esp_timer_impl_lac.o
    endif

    ifdef CONFIG_ESP_TIMER_IMPL_TG0_LAC
        # TG0_LAC timer is suppoted in esp32
        COMPONENT_OBJEXCLUDE := src/esp_timer_impl_frc_legacy.o
    endif

    COMPONENT_OBJEXCLUDE += src/esp_timer_impl_systimer.o
else
    $(error esp_timer is only supported by the Make build system for esp32 chip. For other chips, use the Cmake build system)
endif
