idf_component_register(SRCS "test_fatfs.cpp" "test_fatfs_vfs.cpp"
                       REQUIRES fatfs vfs
                       WHOLE_ARCHIVE
                       )

# Currently 'main' for IDF_TARGET=linux is defined in freertos component.
# Since we are using a freertos mock here, need to let Catch2 provide 'main'.
target_link_libraries(${COMPONENT_LIB} PRIVATE Catch2WithMain)
