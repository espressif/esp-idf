# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)


include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(mqtt_ssl_ds)

# Flash the custom partition named `esp_secure_cert`.
set(partition esp_secure_cert)
idf_build_get_property(project_dir PROJECT_DIR)
set(image_file ${project_dir}/esp_secure_cert_data/${partition}.bin)
partition_table_get_partition_info(offset "--partition-name ${partition}" "offset")
esptool_py_flash_target_image(flash "${partition}" "${offset}" "${image_file}")

target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "main/mosquitto.org.crt" TEXT)
