# LittleFS blockdev mount requires ESP-IDF 6.0+ (esp_blockdev).
if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_LESS "6.0")
    message(WARNING
        "examples/storage/spi_nand_flash/littlefs requires ESP-IDF 6.0 or later "
        "(current: ${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR})")
    message(FATAL_ERROR
        "Please upgrade ESP-IDF to 6.0 or later to build this example")
endif()

idf_component_register(SRCS "spi_nand_flash_littlefs_example_main.c"
                       INCLUDE_DIRS ".")
