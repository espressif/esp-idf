#
# Component Makefile
#

ifeq ($(CONFIG_NIMBLE_ENABLED),y)
COMPONENT_ADD_INCLUDEDIRS += nimble/nimble/include                     \
                             nimble/nimble/host/include                \
                             nimble/porting/nimble/include             \
                             nimble/porting/npl/freertos/include       \
                             nimble/nimble/host/services/ans/include   \
                             nimble/nimble/host/services/bas/include   \
                             nimble/nimble/host/services/gap/include   \
                             nimble/nimble/host/services/gatt/include  \
                             nimble/nimble/host/services/ias/include   \
                             nimble/nimble/host/services/lls/include   \
                             nimble/nimble/host/services/tps/include   \
                             nimble/nimble/host/util/include           \
                             nimble/nimble/host/store/ram/include      \
                             nimble/nimble/host/store/config/include   \
                             esp-hci/include                           \
                             port/include

ifndef CONFIG_NIMBLE_CRYPTO_STACK_MBEDTLS
COMPONENT_ADD_INCLUDEDIRS += nimble/ext/tinycrypt/include
endif

COMPONENT_SRCDIRS += nimble/nimble/host/src                            \
                     nimble/porting/nimble/src                         \
                     nimble/porting/npl/freertos/src                   \
                     nimble/nimble/host/services/ans/src               \
                     nimble/nimble/host/services/bas/src               \
                     nimble/nimble/host/services/gap/src               \
                     nimble/nimble/host/services/gatt/src              \
                     nimble/nimble/host/services/ias/src               \
                     nimble/nimble/host/services/lls/src               \
                     nimble/nimble/host/services/tps/src               \
                     nimble/nimble/host/util/src                       \
                     nimble/nimble/host/store/ram/src                  \
                     nimble/nimble/host/store/config/src               \
                     esp-hci/src                                       \
                     port/src

ifndef CONFIG_NIMBLE_CRYPTO_STACK_MBEDTLS
COMPONENT_SRCDIRS += nimble/ext/tinycrypt/src
endif

COMPONENT_OBJEXCLUDE += nimble/nimble/host/store/config/src/ble_store_config_conf.o

ifeq ($(CONFIG_NIMBLE_MESH),y)
COMPONENT_ADD_INCLUDEDIRS += nimble/nimble/host/mesh/include
COMPONENT_SRCDIRS += nimble/nimble/host/mesh/src

endif
endif
