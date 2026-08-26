idf_component_register(SRCS "fatfsgen_example_main.c"
                    PRIV_REQUIRES vfs fatfs
                    INCLUDE_DIRS ".")

# Create a FATFS image from the contents of the 'fatfs_long_name_image' directory
# that fits the partition named 'storage'. FLASH_IN_PROJECT indicates that
# the generated image should be flashed when the entire project is flashed to
# the target with 'idf.py -p PORT flash'.
# If read-only mode is set (CONFIG_EXAMPLE_FATFS_MODE_READ_ONLY)
# the generated image will be raw without wear levelling support.
# Otherwise it will support wear levelling and thus enable read-write mounting of the image in the device.

set(image ../fatfs_image)

if(CONFIG_EXAMPLE_FATFS_MODE_READ_ONLY)
    fatfs_create_rawflash_image(storage ${image} FLASH_IN_PROJECT PRESERVE_TIME)
else()
    fatfs_create_spiflash_image(storage ${image} FLASH_IN_PROJECT PRESERVE_TIME)
endif()
