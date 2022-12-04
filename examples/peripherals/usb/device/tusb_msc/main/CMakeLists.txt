idf_component_register(
    SRCS "tusb_msc_storage.c" "tusb_msc_main.c"
    INCLUDE_DIRS .
    REQUIRES fatfs wear_levelling console
    )
