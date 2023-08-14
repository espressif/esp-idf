idf_component_register(SRCS "nvsgen_example_main.c"
                    INCLUDE_DIRS ".")

# Create a NVS image from the contents of the `nvs_data` CSV file
# that fits the partition named 'nvs'. FLASH_IN_PROJECT indicates that
# the generated image should be flashed when the entire project is flashed to
# the target with 'idf.py -p PORT flash'.
nvs_create_partition_image(nvs ../nvs_data.csv FLASH_IN_PROJECT)
