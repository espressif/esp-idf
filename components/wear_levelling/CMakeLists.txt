idf_component_register(SRCS "Partition.cpp"
                            "SPI_Flash.cpp"
                            "WL_Ext_Perf.cpp"
                            "WL_Ext_Safe.cpp"
                            "WL_Flash.cpp"
                            "crc32.cpp"
                            "wear_levelling.cpp"
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS private_include
                    REQUIRES esp_partition
                    PRIV_REQUIRES spi_flash)

if(CONFIG_COMPILER_STATIC_ANALYZER AND CMAKE_C_COMPILER_ID STREQUAL "GNU") # TODO IDF-10089
    target_compile_options(${COMPONENT_LIB} PUBLIC -fno-analyzer)
endif()
