if(CONFIG_OPENTHREAD_STATE_INDICATOR_ENABLE)
    set(srcs "ot_led_strip.c")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES led_strip openthread)
