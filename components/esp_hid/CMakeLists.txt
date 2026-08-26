idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs "src/esp_hidd.c"
         "src/esp_hidh.c"
         "src/esp_hid_common.c")

set(include_dirs "include")
set(priv_include_dirs "private")

if(CONFIG_BT_ENABLED)
    if(CONFIG_BT_BLUEDROID_ENABLED)
        list(APPEND srcs
             "src/ble_hidd.c"
             "src/ble_hidh.c"
             "src/bt_hidh.c"
             "src/bt_hidd.c")
    endif()
    if(CONFIG_BT_NIMBLE_ENABLED)
        list(APPEND srcs
                "src/nimble_hidd.c"
                "src/nimble_hidh.c")
    endif()
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}"
                       PRIV_INCLUDE_DIRS "${priv_include_dirs}"
                       REQUIRES esp_event bt)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
