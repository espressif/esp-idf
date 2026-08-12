idf_component_register(SRCS "ppa_freetype_example_main.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_hw_support esp_mm esp_psram esp_driver_ppa mbedtls)

# Download the Font Awesome regular font (OTF) from upstream and pack it into a
# "littlefs" directory in the build directory, then create a "fonts" partition
# image from it.
#
# The example displays Font Awesome icons. Font Awesome 7.3.1 ships its Free fonts
# both as WOFF2 (webfonts/) and as OpenType (otfs/). The `espressif/freetype`
# component is built without WOFF2/brotli support, so a WOFF2 file cannot be
# decoded directly; the OpenType (OTF) flavour, on the other hand, is fully
# supported by FreeType. We therefore download the OTF variant from a fixed
# release tag at build time instead of committing a font binary into the tree.
set(FONT_AWESOME_VERSION "7.3.1")
string(CONCAT URL
       "https://github.com/FortAwesome/Font-Awesome/raw/refs/tags/${FONT_AWESOME_VERSION}/"
       "otfs/Font%20Awesome%207%20Free-Regular-400.otf")
set(FILE "fa-regular-400.otf")
set(LITTLEFS_DIR "${CMAKE_BINARY_DIR}/littlefs")
set(FONT_PATH "${LITTLEFS_DIR}/${FILE}")
file(MAKE_DIRECTORY "${LITTLEFS_DIR}")
if(NOT EXISTS "${FONT_PATH}")
    file(DOWNLOAD "${URL}" "${FONT_PATH}" STATUS DOWNLOAD_STATUS SHOW_PROGRESS)
    list(GET DOWNLOAD_STATUS 0 DOWNLOAD_STATUS_CODE)
    list(GET DOWNLOAD_STATUS 1 DOWNLOAD_STATUS_MESSAGE)
    if(NOT DOWNLOAD_STATUS_CODE EQUAL 0)
        message(FATAL_ERROR "Failed to download ${URL}: ${DOWNLOAD_STATUS_MESSAGE}")
    endif()
endif()

# Create a partition named "fonts" with the example font
littlefs_create_partition_image(fonts "${LITTLEFS_DIR}" FLASH_IN_PROJECT)
