idf_component_register(SRCS "main.c" "dns_server.c"
                    INCLUDE_DIRS "include"
                    EMBED_FILES root.html)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
