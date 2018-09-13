set(COMPONENT_SRCS "decode_image.c"
                   "pretty_effect.c"
                   "spi_master_example_main.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")


set(COMPONENT_EMBED_FILES image.jpg)

register_component()
