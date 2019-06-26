set(COMPONENT_SRCS "pretty_effect.c"
                   "spi_master_example_main.c")

#only esp32 has enough memory to do jpeg decoding
if (CONFIG_IDF_TARGET_ESP32)
    list(APPEND COMPONENT_SRCS "decode_image.c")
endif()

set(COMPONENT_ADD_INCLUDEDIRS ".")


set(COMPONENT_EMBED_FILES image.jpg)

register_component()
