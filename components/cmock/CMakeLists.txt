# Although unity is a submodule of cmock, we still depend on the unity component in IDF.
# This is because CI currently doesn't checkout submodules recursively.
idf_component_register(SRCS "CMock/src/cmock.c"
                    REQUIRES unity
                    INCLUDE_DIRS "CMock/src")
