idf_build_get_property(target IDF_TARGET)

set(srcs "test_ringbuf_main.c"
         "test_ringbuf_common.c")

set(priv_requires esp_ringbuf spi_flash unity)

if(NOT ${target} STREQUAL "linux")
    list(APPEND srcs "test_ringbuf_target.c")
    list(APPEND priv_requires esp_driver_gptimer)
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES ${priv_requires}
                       WHOLE_ARCHIVE)
