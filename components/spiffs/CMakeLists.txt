idf_build_get_property(target IDF_TARGET)

set(original_srcs "spiffs/src/spiffs_cache.c"
                  "spiffs/src/spiffs_check.c"
                  "spiffs/src/spiffs_gc.c"
                  "spiffs/src/spiffs_hydrogen.c"
                  "spiffs/src/spiffs_nucleus.c")

list(APPEND srcs "spiffs_api.c" ${original_srcs})

if(NOT ${target} STREQUAL "linux")
    list(APPEND pr bootloader_support esptool_py vfs)
    list(APPEND srcs "esp_spiffs.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS "." "spiffs/src"
                       REQUIRES esp_partition
                       PRIV_REQUIRES ${pr} spi_flash)

# Upstream SPIFFS code uses format specifiers in debug logging macros inconsistently
list(TRANSFORM original_srcs PREPEND "${CMAKE_CURRENT_LIST_DIR}/")
set_source_files_properties(${original_srcs} PROPERTIES COMPILE_FLAGS -Wno-format)

get_source_file_property(nucleus_cc_flags "${CMAKE_CURRENT_LIST_DIR}/spiffs/src/spiffs_nucleus.c" COMPILE_FLAGS)
set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/spiffs/src/spiffs_nucleus.c" PROPERTIES COMPILE_FLAGS "${nucleus_cc_flags} -Wno-tautological-compare")
