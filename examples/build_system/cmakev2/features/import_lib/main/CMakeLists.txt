idf_component_register(SRCS "import_lib_example_main.cpp"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES tinyxml2 fatfs)

# Create a FAT filesystem image from the contents of data/ subdirectory,
# The image will be flashed into the 'storage' partition when 'idf.py flash' is used.
fatfs_create_spiflash_image(storage data FLASH_IN_PROJECT)
