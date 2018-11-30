set(COMPONENT_SRCS "Partition.cpp"
                   "SPI_Flash.cpp"
                   "WL_Ext_Perf.cpp"
                   "WL_Ext_Safe.cpp"
                   "WL_Flash.cpp"
                   "crc32.cpp"
                   "wear_levelling.cpp")
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_PRIV_INCLUDEDIRS private_include)
set(COMPONENT_REQUIRES spi_flash)
register_component()
