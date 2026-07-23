# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
set(srcs "test_app_main.c"
         "test_uhci.c")

# The cache-safe case needs uhci_receive() in IRAM, only build it when that path is enabled
if(CONFIG_UHCI_ISR_CACHE_SAFE)
    list(APPEND srcs "test_uhci_cache_safe.c")
endif()

idf_component_register(
    SRCS ${srcs}
    REQUIRES esp_driver_uart unity test_utils esp_psram
    WHOLE_ARCHIVE
)
