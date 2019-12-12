add_library(stub_esp32 STATIC system_api.c flash_ops.c cpu_start.c)
target_include_directories(stub_esp32 PUBLIC .)
add_library(stub::esp32 ALIAS stub_esp32)

target_link_libraries(stub_esp32 stub::spi_flash)