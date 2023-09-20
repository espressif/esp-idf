set(srcs "test_app_main.c"
         "src/ieee802154_cmd.c")

set(include "."
            "include")

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include
                       PRIV_REQUIRES ieee802154 console fatfs nvs_flash esp_phy
                       WHOLE_ARCHIVE)
