idf_component_register(SRCS "test_fatfs_bdl.cpp"
                       REQUIRES fatfs vfs esp_blockdev esp_blockdev_util wear_levelling
                       WHOLE_ARCHIVE
                       )

target_link_libraries(${COMPONENT_LIB} PRIVATE Catch2WithMain)
