set(COMPONENT_SRCS "ble_hidd_demo_main.c"
                   "esp_hidd_prf_api.c"
                   "hid_dev.c"
                   "hid_device_le_prf.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()

if(GCC_NOT_5_2_0)
    component_compile_options(-Wno-unused-const-variable)
endif()
