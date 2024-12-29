idf_component_register(SRCS "bootloader_flash_qio_custom.c"
                            "bootloader_flash_unlock_custom.c"
                       REQUIRES bootloader_support spi_flash
                       INCLUDE_DIRS ""
                       WHOLE_ARCHIVE
                       )

# The symbol should be linked to cover weak symbol. Please don't call the function directly.
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u bootloader_flash_unlock")
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u bootloader_flash_qe_support_list")
