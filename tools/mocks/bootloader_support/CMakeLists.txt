# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.
message(STATUS "building BOOTLOADER SUPPORT MOCKS")

idf_component_get_property(original_bootloader_support_dir bootloader_support COMPONENT_OVERRIDEN_DIR)

# Note: bootloader_memory_utils.h, bootloader_util.h and esp_app_format.h are not mocked
# since they only contain inline functions which we cannot mock without changing the build system.
idf_component_mock(INCLUDE_DIRS "${original_bootloader_support_dir}/include"
                   MOCK_HEADER_FILES "${original_bootloader_support_dir}/include/bootloader_clock.h"
                   MOCK_HEADER_FILES "${original_bootloader_support_dir}/include/bootloader_common.h"
                   MOCK_HEADER_FILES "${original_bootloader_support_dir}/include/bootloader_mem.h"
                   MOCK_HEADER_FILES "${original_bootloader_support_dir}/include/bootloader_random.h"
                   MOCK_HEADER_FILES "${original_bootloader_support_dir}/include/esp_flash_encrypt.h"
                   MOCK_HEADER_FILES "${original_bootloader_support_dir}/include/esp_flash_partitions.h"
                   MOCK_HEADER_FILES "${original_bootloader_support_dir}/include/esp_image_format.h"
                   MOCK_HEADER_FILES "${original_bootloader_support_dir}/include/esp_secure_boot.h"
    )
