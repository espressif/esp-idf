set(srcs "essl.c" "essl_sdio.c" "essl_spi.c")
set(include "include")
set(priv_include "." "include/esp_serial_slave_link")
set(priv_req idf::sdmmc idf::driver)

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC ${include})
target_include_directories(${COMPONENT_LIB} PRIVATE ${priv_include})
target_link_libraries(${COMPONENT_LIB} PRIVATE ${priv_req})
