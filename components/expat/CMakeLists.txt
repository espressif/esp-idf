idf_component_register(SRCS "expat/expat/lib/xmlparse.c"
                            "expat/expat/lib/xmlrole.c"
                            "expat/expat/lib/xmltok.c"
                            "expat/expat/lib/xmltok_impl.c"
                            "expat/expat/lib/xmltok_ns.c"
                    INCLUDE_DIRS expat/expat/lib port/include)

target_compile_definitions(${COMPONENT_LIB} PRIVATE HAVE_EXPAT_CONFIG_H)
target_compile_definitions(${COMPONENT_LIB} PRIVATE HAVE_GETRANDOM)

# Temporary suppress "fallthrough" warnings until they are fixed in expat repo
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-implicit-fallthrough)
