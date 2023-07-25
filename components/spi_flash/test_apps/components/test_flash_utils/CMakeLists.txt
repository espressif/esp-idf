set(srcs "test_flash_utils.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include
                       REQUIRES esp_partition)
