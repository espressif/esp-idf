idf_component_register(SRCS "i80_controller_example_main.c" "lvgl_port.c" "lvgl_demo_ui.c"
                       PRIV_REQUIRES esp_lcd esp_psram esp_timer
                       INCLUDE_DIRS ".")

# Locate the LVGL component so we can invoke its image converter script.
idf_component_get_property(lvgl_dir lvgl__lvgl COMPONENT_DIR)
idf_build_get_property(python PYTHON)

# The source PNG images are kept under assets. During the build we
# convert them into LVGL binary images (LZ4-compressed) using the LVGLImage.py
# script shipped with the LVGL component. The generated .bin files are written to
# the build directory (instead of the project tree) so the repository stays
# clean, and are then packed into the LittleFS partition image.
set(IMAGE_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/assets")
set(IMAGE_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/images/binary")

file(MAKE_DIRECTORY "${IMAGE_BIN_DIR}")
file(GLOB IMAGE_PNGS "${IMAGE_SRC_DIR}/*.png")

set(IMAGE_BINS)
foreach(png ${IMAGE_PNGS})
    get_filename_component(name "${png}" NAME_WE)
    set(bin "${IMAGE_BIN_DIR}/${name}.bin")
    list(APPEND IMAGE_BINS "${bin}")
    add_custom_command(
        OUTPUT "${bin}"
        COMMAND ${python} "${lvgl_dir}/scripts/LVGLImage.py"
                --ofmt BIN --cf RGB565A8 --compress LZ4
                -o "${IMAGE_BIN_DIR}" "${png}"
        DEPENDS "${png}"
        COMMENT "Generate LVGL binary image ${name}.bin"
        VERBATIM)
endforeach()

if(IMAGE_BINS)
    add_custom_target(lvgl_image_bins ALL DEPENDS ${IMAGE_BINS})
endif()

# Pack the generated binary images into a LittleFS partition image and flash it
# to the "storage" partition (see partitions.csv). The partition
# image generation is made to depend on the LVGLImage.py conversion step so the
# .bin files are guaranteed to be present before the LittleFS image is built.
if(IMAGE_BINS)
    littlefs_create_partition_image(storage "${IMAGE_BIN_DIR}" FLASH_IN_PROJECT DEPENDS lvgl_image_bins)
else()
    littlefs_create_partition_image(storage "${IMAGE_BIN_DIR}" FLASH_IN_PROJECT)
endif()
