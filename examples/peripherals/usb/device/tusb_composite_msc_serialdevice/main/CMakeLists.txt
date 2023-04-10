set(srcs "tusb_composite_main.c")
set(requires fatfs wear_levelling)

idf_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS .
    REQUIRES "${requires}"
)
