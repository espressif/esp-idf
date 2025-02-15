add_library(stub_spi_flash STATIC flash_ops.c)
target_include_directories(stub_spi_flash INTERFACE .)
add_library(stub::spi_flash ALIAS stub_spi_flash)
