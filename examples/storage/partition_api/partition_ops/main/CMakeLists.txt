idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_partition)

set(partition "custom_partition")
set(image_file ${CMAKE_BINARY_DIR}/custom_partition.bin)
set(custom_partition_script_py ${CMAKE_CURRENT_SOURCE_DIR}/../create_custom_partition.py)

# Generate custom_partition.bin using the Python script (output to build directory)
add_custom_command(
    OUTPUT ${image_file}
    COMMAND ${PYTHON} ${custom_partition_script_py} ${image_file}
    DEPENDS ${custom_partition_script_py}
    COMMENT "Generating custom_partition.bin"
)
add_custom_target(generate_custom_partition ALL DEPENDS ${image_file})

# Flash the generated binary to the partition named "custom_partition"
# (build target depends on generate_custom_partition to ensure the binary is created first, and FLASH_IN_PROJECT
# makes it get flashed when calling `idf.py custom_partition-flash` or just `idf.py flash`)
esp_partition_register_target(${partition} "${image_file}" DEPENDS generate_custom_partition FLASH_IN_PROJECT)
