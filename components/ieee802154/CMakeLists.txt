idf_build_get_property(idf_target IDF_TARGET)

set(srcs "")
set(include "include")
set(private_include "")

if(CONFIG_IEEE802154_ENABLED)
    list(APPEND srcs "esp_ieee802154.c"
        "driver/esp_ieee802154_ack.c"
        "driver/esp_ieee802154_dev.c"
        "driver/esp_ieee802154_frame.c"
        "driver/esp_ieee802154_pib.c"
        "driver/esp_ieee802154_util.c"
        "driver/esp_ieee802154_sec.c"
        "driver/esp_ieee802154_timer.c")
    list(APPEND private_include "private_include")
endif()

idf_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS "${include}"
    PRIV_INCLUDE_DIRS "${private_include}"
    LDFRAGMENTS linker.lf
    PRIV_REQUIRES esp_phy driver esp_timer esp_coex soc hal
)
