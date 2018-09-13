set(COMPONENT_ADD_INCLUDEDIRS .)
set(COMPONENT_SRCS "commands.c"
                   "split_argv.c"
                   "argtable3/argtable3.c"
                   "linenoise/linenoise.c")

set(COMPONENT_REQUIRES)

register_component()

