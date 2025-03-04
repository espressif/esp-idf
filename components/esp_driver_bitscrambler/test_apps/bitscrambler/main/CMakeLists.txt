set(srcs "test_app_main.c")

if(CONFIG_SOC_BITSCRAMBLER_SUPPORTED)
    list(APPEND srcs "test_bitscrambler.c")
endif()

set(priv_requires
        unity
        esp_driver_bitscrambler
)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES ${priv_requires}
                       WHOLE_ARCHIVE TRUE)

target_bitscrambler_add_src("timeout.bsasm")
target_bitscrambler_add_src("trivial.bsasm")
target_bitscrambler_add_src("eof_upstream.bsasm")
target_bitscrambler_add_src("eof_downstream.bsasm")
target_bitscrambler_add_src("lut32.bsasm")
target_bitscrambler_add_src("loop.bsasm")
