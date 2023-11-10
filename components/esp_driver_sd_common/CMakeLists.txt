set(srcs)
set(public_include "include")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       )
