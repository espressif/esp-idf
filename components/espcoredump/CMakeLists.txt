set(COMPONENT_PRIV_INCLUDEDIRS "include_core_dump")
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_REQUIRES)
set(COMPONENT_PRIV_REQUIRES spi_flash)
set(COMPONENT_ADD_LDFRAGMENTS linker.lf)
set(COMPONENT_SRCS "src/core_dump_common.c" 
                   "src/core_dump_flash.c"
                   "src/core_dump_port.c"
                   "src/core_dump_uart.c") 

register_component()
