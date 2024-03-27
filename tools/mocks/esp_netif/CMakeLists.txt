# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.
message(STATUS "building ESP NETIF MOCKS")

idf_component_get_property(original_esp_netif_dir esp_netif COMPONENT_OVERRIDEN_DIR)

set(include_dirs
    "include"
    "${original_esp_netif_dir}/include")

idf_component_mock(INCLUDE_DIRS ${include_dirs}
    REQUIRES esp_event
    MOCK_HEADER_FILES ${original_esp_netif_dir}/include/esp_netif.h)
