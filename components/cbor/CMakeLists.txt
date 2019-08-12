idf_component_register(SRCS "tinycbor/src/cborencoder_close_container_checked.c"
                            "tinycbor/src/cborencoder.c"
                            "tinycbor/src/cborerrorstrings.c"
                            "tinycbor/src/cborparser_dup_string.c"
                            "tinycbor/src/cborparser.c"
                            "tinycbor/src/cborpretty_stdio.c"
                            "tinycbor/src/cborpretty.c"
                            "tinycbor/src/cbortojson.c"
                            "tinycbor/src/cborvalidation.c"
                            "tinycbor/src/open_memstream.c"
                    INCLUDE_DIRS "port/include"
                    PRIV_INCLUDE_DIRS "tinycbor/src")

# for open_memstream.c
target_compile_definitions(${COMPONENT_LIB} PRIVATE "__GLIBC__")

# cbortojson.c:378:17: assignment discards 'const' qualifier from pointer target type
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-discarded-qualifiers")

# cborvalidation.c:429:22: 'valf' may be used uninitialized
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-maybe-uninitialized")
