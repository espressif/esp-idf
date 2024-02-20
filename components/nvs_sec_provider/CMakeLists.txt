idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

idf_component_register(SRCS "nvs_sec_provider.c"
                       INCLUDE_DIRS include
                       PRIV_REQUIRES bootloader_support efuse esp_partition nvs_flash)

# NOTE: In a case where only the default NVS partition is to be encrypted
# and no custom NVS partitions exist, `nvs_flash_init` is the only API that
# needs to be called. No API from the `nvs_sec_provider` component is called
# as neither the `nvs_flash` component nor the user app depends on it.
# Thus, the symbols from this component are not placed in the .map file and
# hence the constructor, which initialises the encryption scheme for the default
# NVS partition, never executes. The following is a workaround for the same.
target_link_libraries(${COMPONENT_LIB} PRIVATE "-u nvs_sec_provider_include_impl")
