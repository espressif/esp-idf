if(CONFIG_EXAMPLE_MODE_NO_CONNECTION)
    set(MAIN_SRC "enc_adv_data_cent_no_connect.c")
else()
    set(MAIN_SRC "enc_adv_data_cent.c")
endif()

idf_component_register(SRCS ${MAIN_SRC} "ble_ead.c"
                       INCLUDE_DIRS ".")
