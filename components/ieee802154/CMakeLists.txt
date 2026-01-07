idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs "")
set(include "include")
set(private_include "")

if(CONFIG_IEEE802154_ENABLED)
    list(APPEND srcs "esp_ieee802154.c"
        "driver/esp_ieee802154_ack.c"
        "driver/esp_ieee802154_dev.c"
        "driver/esp_ieee802154_event.c"
        "driver/esp_ieee802154_frame.c"
        "driver/esp_ieee802154_pib.c"
        "driver/esp_ieee802154_util.c"
        "driver/esp_ieee802154_sec.c"
        "driver/esp_ieee802154_timer.c")
    list(APPEND private_include "private_include")

    if(CONFIG_IEEE802154_TEST)
        list(REMOVE_ITEM private_include "private_include")
        list(APPEND include "private_include")
    endif()

endif()

if(CONFIG_IEEE802154_DEBUG)
    list(APPEND srcs "driver/esp_ieee802154_debug.c")
endif()

idf_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS "${include}"
    PRIV_INCLUDE_DIRS "${private_include}"
    LDFRAGMENTS linker.lf
    REQUIRES esp_coex
    PRIV_REQUIRES esp_phy esp_timer soc hal esp_pm esp_hal_ieee802154
)
