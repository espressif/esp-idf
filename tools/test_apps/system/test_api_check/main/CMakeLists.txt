# Not setting REQUIRES so that `main.c` depends on all components in the build,
# allowing all the public headers to be extracted.
idf_component_register(SRCS "main.c")
