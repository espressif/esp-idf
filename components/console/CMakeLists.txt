set(COMPONENT_ADD_INCLUDEDIRS .)
set(COMPONENT_SRCS "argtable3/argtable3.c"
                   "commands.c"
                   "linenoise/linenoise.c"
                   "split_argv.c")

set(COMPONENT_REQUIRES)

register_component()

