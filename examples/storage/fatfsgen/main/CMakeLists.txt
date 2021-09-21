idf_component_register(SRCS "fatfsgen_example_main.c"
                    INCLUDE_DIRS ".")

# Create a FATFS image from the contents of the 'fatfs_image' directory
# that fits the partition named 'storage'. FLASH_IN_PROJECT indicates that
# the generated image should be flashed when the entire project is flashed to
# the target with 'idf.py -p PORT flash'.
fatfs_create_partition_image(storage ../fatfs_image FLASH_IN_PROJECT)
