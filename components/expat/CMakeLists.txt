set(COMPONENT_ADD_INCLUDEDIRS expat/expat/lib port/include)
set(COMPONENT_SRCS "expat/expat/lib/loadlibrary.c"
                   "expat/expat/lib/xmlparse.c"
                   "expat/expat/lib/xmlrole.c"
                   "expat/expat/lib/xmltok.c"
                   "expat/expat/lib/xmltok_impl.c"
                   "expat/expat/lib/xmltok_ns.c")

set(COMPONENT_REQUIRES)

register_component()

component_compile_definitions(HAVE_EXPAT_CONFIG_H)
component_compile_definitions(HAVE_GETRANDOM)

# Temporary suppress "fallthrough" warnings until they are fixed in expat repo
if(GCC_NOT_5_2_0)
    component_compile_options(-Wno-implicit-fallthrough)
endif()