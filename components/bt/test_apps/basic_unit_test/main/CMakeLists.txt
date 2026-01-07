idf_component_register(SRCS "test_bt_main.c"
                            "test_bt_common.c"
                            "test_smp.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity bt
                       WHOLE_ARCHIVE)

# Add private include directories from bt component to access internal headers
idf_component_get_property(bt_dir bt COMPONENT_DIR)
target_include_directories(${COMPONENT_LIB} PRIVATE ${bt_dir}/host/bluedroid/stack/smp/include)
target_include_directories(${COMPONENT_LIB} PRIVATE ${bt_dir}/host/bluedroid/common/include)
target_include_directories(${COMPONENT_LIB} PRIVATE ${bt_dir}/host/bluedroid/stack/include)
target_include_directories(${COMPONENT_LIB} PRIVATE ${bt_dir}/common/include)
