set(srcs "main.c"
        "board.c"
        "cmd_wifi.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS  ".")
