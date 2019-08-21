idf_component_register(SRCS "src/core_dump_common.c" 
                            "src/core_dump_flash.c"
                            "src/core_dump_port.c"
                            "src/core_dump_uart.c"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS "include_core_dump"
                    LDFRAGMENTS linker.lf
                    PRIV_REQUIRES spi_flash soc)
