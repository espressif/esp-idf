idf_build_get_property(target IDF_TARGET)

set(srcs)
set(include "include")

# I3C related source files.
if(CONFIG_I3C_MASTER_ENABLED)
    list(APPEND srcs "i3c_master.c"
        )
endif()

if(${target} STREQUAL "linux")
    set(priv_requires "")
else()
    set(priv_requires esp_driver_gpio esp_pm esp_mm)
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${include}
                    PRIV_REQUIRES "${priv_requires}"
                    LDFRAGMENTS "linker.lf"
                    )
