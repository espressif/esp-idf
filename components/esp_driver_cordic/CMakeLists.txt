set(srcs)
set(public_include "include")
set(requires esp_hal_cordic)
set(priv_requires)

if(CONFIG_SOC_CORDIC_SUPPORTED)
    list(APPEND srcs "cordic.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES ${priv_requires}
                       REQUIRES ${requires}
                       LDFRAGMENTS linker.lf
                      )
