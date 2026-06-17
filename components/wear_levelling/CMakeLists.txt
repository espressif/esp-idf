idf_component_register(SRCS "Partition.cpp"
                            "BDL_Access.cpp"
                            "SPI_Flash.cpp"
                            "WL_Ext_Perf.cpp"
                            "WL_Ext_Safe.cpp"
                            "WL_Flash.cpp"
                            "crc32.cpp"
                            "wear_levelling.cpp"
                            "wl_blockdev.cpp"
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS private_include
                    REQUIRES esp_partition esp_blockdev
                    PRIV_REQUIRES spi_flash)
