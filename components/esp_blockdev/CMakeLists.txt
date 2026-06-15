idf_component_register(INCLUDE_DIRS include)

# Register this component's own ioctl definitions for overlap checking
idf_build_set_property(
    ESP_BLOCKDEV_IOCTL_DEF_FILES
    "${CMAKE_CURRENT_LIST_DIR}/include/esp_blockdev.h"
    APPEND
)
