idf_component_register(SRCS "esp_littlefs_example.c"
                    INCLUDE_DIRS "."
                    )

# Note: you must have a partition named the first argument (here it's "littlefs")
# in your partition table csv file.
if(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    littlefs_create_partition_image(storage ../flash_data FLASH_IN_PROJECT)
else()
    fail_at_build_time(littlefs "Windows does not support LittleFS partition generation")
endif()
