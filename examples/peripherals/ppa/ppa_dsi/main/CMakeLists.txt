set(srcs "ppa_dsi_main.c")

if(CONFIG_EXAMPLE_SOURCE_IMAGE_FORMAT_RGB565)
    list(APPEND srcs "image.c")
endif()

set(embed_files)

if(CONFIG_EXAMPLE_SOURCE_IMAGE_FORMAT_JPEG)
    list(APPEND embed_files "image.jpg")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       REQUIRES esp_mm esp_driver_ppa dsi_init
                       EMBED_FILES ${embed_files}
                       )
