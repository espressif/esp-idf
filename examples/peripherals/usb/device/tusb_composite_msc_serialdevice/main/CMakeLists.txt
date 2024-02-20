idf_component_register(
    SRCS tusb_composite_main.c
    INCLUDE_DIRS .
    PRIV_REQUIRES fatfs wear_levelling esp_partition
)
