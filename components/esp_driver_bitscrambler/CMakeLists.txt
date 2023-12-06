idf_build_get_property(target IDF_TARGET)

set(srcs)
set(include_dirs)
set(priv_requires)

set(my_priv_requires "soc" "hal" "esp_hw_support" "esp_mm")

if(CONFIG_SOC_BITSCRAMBLER_SUPPORTED)
    list(APPEND srcs "bitscrambler.c" "bitscrambler_loopback.c")
    list(APPEND include_dirs "include")
endif()

# Note that (according to the docs) "The values of REQUIRES and PRIV_REQUIRES
# should not depend on any configuration choices (CONFIG_xxx macros)." We work
# around that by setting the actual priv_requires value in the target checks,
# rather than make it depend on CONFIG_SOC_BITSCRAMBLER_SUPPORTED.

if(target STREQUAL "esp32p4")
    list(APPEND srcs "bitscrambler_esp32p4.c")
    set(priv_requires ${my_priv_requires})
endif()


idf_component_register(SRCS ${srcs}
                    PRIV_REQUIRES ${priv_requires}
                    INCLUDE_DIRS ${include_dirs}
                    PRIV_INCLUDE_DIRS "priv_include"
                    )
