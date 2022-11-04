idf_component_register(SRCS test_wl.c
                       PRIV_INCLUDE_DIRS .
                       PRIV_REQUIRES wear_levelling unity spi_flash
                       EMBED_FILES test_partition_v1.bin
                      )
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
