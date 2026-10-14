idf_component_register(SRCS "test_bt_main.c"
                            "test_bt_common.c"
                            "test_smp.c"
                            "test_tinycrypt_ecc.c"
                            "test_osal.c"
                            "test_osi_event.c"
                            "test_prf_task.c"
                            "test_c2h_credit.c"
                            "test_packet_fragmenter.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity bt
                       WHOLE_ARCHIVE)

# Add private include directories from bt component to access internal headers
idf_component_get_property(bt_dir bt COMPONENT_DIR)
target_include_directories(${COMPONENT_LIB} PRIVATE ${bt_dir}/host/bluedroid/stack/smp/include)
target_include_directories(${COMPONENT_LIB} PRIVATE ${bt_dir}/host/bluedroid/common/include)
target_include_directories(${COMPONENT_LIB} PRIVATE ${bt_dir}/host/bluedroid/stack/include)
target_include_directories(${COMPONENT_LIB} PRIVATE ${bt_dir}/host/bluedroid/hci/include)
target_include_directories(${COMPONENT_LIB} PRIVATE ${bt_dir}/host/bluedroid/device/include)
target_include_directories(${COMPONENT_LIB} PRIVATE ${bt_dir}/common/include)
target_include_directories(${COMPONENT_LIB} PRIVATE ${bt_dir}/common/btc/include)
target_include_directories(${COMPONENT_LIB} PRIVATE ${bt_dir}/common/tinycrypt/include)
