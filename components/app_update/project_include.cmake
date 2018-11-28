
# Set empty otadata partition file for flashing, if OTA data partition in
# partition table
# (NB: because of component dependency, we know partition_table
# project_include.cmake has already been included.)

if(OTADATA_PARTITION_OFFSET AND OTADATA_PARTITION_SIZE AND IDF_BUILD_ARTIFACTS)
    set(BLANK_OTADATA_FILE "ota_data_initial.bin")
endif()
