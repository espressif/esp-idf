#
# The mbedtls component gets pulled in during the build(due to a dependency of component bootloader_support),
# but we needed to avoid inclusion of mbedtls in this hal layer test app, thus creating a "dummy" mbedtls component.
# This dummy mbedtls component will get the priority during the build stage and thus the "real" mbedtls component
# does not get pulled.
#
idf_build_get_property(idf_target IDF_TARGET)
idf_build_get_property(python PYTHON)

set(mbedtls_srcs ".")
set(mbedtls_include_dirs include)

idf_component_register(SRCS "${mbedtls_srcs}"
                INCLUDE_DIRS "${mbedtls_include_dirs}")
