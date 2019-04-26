set(COMPONENT_SRCS "spiffsgen_example_main.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()

# Create a SPIFFS image from the contents of the 'spiffs_image' directory
# that fits the partition named 'storage'. FLASH_IN_PROJECT indicates that
# the generated image should be flashed when the entire project is flashed to
# the target with 'idf.py flash'. 
spiffs_create_partition_image(storage ../spiffs_image FLASH_IN_PROJECT)
