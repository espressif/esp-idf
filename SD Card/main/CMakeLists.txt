# set(srcs "sd_card_example_main.c")

# set(srcs "main.c")
# set(srcs "file_name_change_date.c")
set(srcs "using_thread_update.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       REQUIRES fatfs sd_card
                       WHOLE_ARCHIVE)
