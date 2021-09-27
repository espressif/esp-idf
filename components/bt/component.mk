#
# Component Makefile
#
ifdef CONFIG_BT_ENABLED

COMPONENT_SRCDIRS := controller

COMPONENT_ADD_INCLUDEDIRS := include

LIBS := btdm_app

COMPONENT_ADD_LDFLAGS     := -lbt -L $(COMPONENT_PATH)/controller/lib_esp32/esp32 \
                          -u ld_include_hli_vectors_bt \
                           $(addprefix -l,$(LIBS))

# re-link program if BT binary libs change
COMPONENT_ADD_LINKER_DEPS := $(patsubst %,$(COMPONENT_PATH)/controller/lib_esp32/esp32/lib%.a,$(LIBS))

COMPONENT_SUBMODULES += controller/lib_esp32
COMPONENT_ADD_LDFRAGMENTS += linker.lf


# TODO: annotate fallthroughs in Bluedroid code with comments
CFLAGS += -Wno-implicit-fallthrough

COMPONENT_ADD_INCLUDEDIRS +=    include/esp32/include
COMPONENT_SRCDIRS +=  controller/esp32

COMPONENT_PRIV_INCLUDEDIRS += common/btc/include              	   \
							  common/include

COMPONENT_ADD_INCLUDEDIRS +=  common/api/include/api                       \
                              common/btc/profile/esp/blufi/include         \
                              common/btc/profile/esp/include               \
		              common/osi/include
COMPONENT_SRCDIRS +=    common/api        \
	                common/btc/core                               \
                        common/osi              \
                        common/btc/profile/esp/blufi

ifdef CONFIG_BT_BLUEDROID_ENABLED

COMPONENT_PRIV_INCLUDEDIRS +=   host/bluedroid/bta/include                   \
                                host/bluedroid/bta/ar/include                \
                                host/bluedroid/bta/av/include                \
                                host/bluedroid/bta/hf_ag/include             \
                                host/bluedroid/bta/hf_client/include         \
                                host/bluedroid/bta/dm/include                \
                                host/bluedroid/bta/gatt/include              \
                                host/bluedroid/bta/hd/include                \
                                host/bluedroid/bta/hh/include                \
                                host/bluedroid/bta/jv/include                \
                                host/bluedroid/bta/sdp/include               \
                                host/bluedroid/bta/sys/include               \
                                host/bluedroid/device/include                \
                                host/bluedroid/gki/include                   \
                                host/bluedroid/hci/include                   \
                                host/bluedroid/utils/include                 \
                                host/bluedroid/external/sbc/decoder/include  \
                                host/bluedroid/external/sbc/encoder/include  \
                                host/bluedroid/external/sbc/plc/include      \
                                host/bluedroid/btc/profile/esp/include       \
                                host/bluedroid/btc/profile/std/gatt/include  \
                                host/bluedroid/btc/profile/std/gap/include   \
                                host/bluedroid/btc/profile/std/a2dp/include  \
                                host/bluedroid/btc/profile/std/hid/include   \
                                host/bluedroid/btc/profile/std/include       \
                                host/bluedroid/btc/include                   \
                                host/bluedroid/btif/include                  \
                                host/bluedroid/stack/btm/include             \
                                host/bluedroid/stack/btu/include             \
                                host/bluedroid/stack/gap/include             \
                                host/bluedroid/stack/gatt/include            \
                                host/bluedroid/stack/hcic/include            \
                                host/bluedroid/stack/hid/include             \
                                host/bluedroid/stack/l2cap/include           \
                                host/bluedroid/stack/sdp/include             \
                                host/bluedroid/stack/smp/include             \
                                host/bluedroid/stack/avct/include            \
                                host/bluedroid/stack/avrc/include            \
                                host/bluedroid/stack/avdt/include            \
                                host/bluedroid/stack/a2dp/include            \
                                host/bluedroid/stack/rfcomm/include          \
                                host/bluedroid/stack/include                 \
                                host/bluedroid/utils/include                 \
                                host/bluedroid/common/include                \

COMPONENT_ADD_INCLUDEDIRS +=    host/bluedroid/api/include/api       \

COMPONENT_SRCDIRS +=    host/bluedroid/bta/dm                      \
                        host/bluedroid/bta/gatt                    \
                        host/bluedroid/bta/hd                      \
                        host/bluedroid/bta/hh                      \
                        host/bluedroid/bta/sdp                     \
                        host/bluedroid/bta/av                      \
                        host/bluedroid/bta/ar                      \
                        host/bluedroid/bta/sys                     \
                        host/bluedroid/bta/jv                      \
                        host/bluedroid/bta/hf_ag                   \
                        host/bluedroid/bta/hf_client               \
                        host/bluedroid/bta                         \
                        host/bluedroid/btif                        \
                        host/bluedroid/device                      \
                        host/bluedroid/gki                         \
                        host/bluedroid/hci                         \
                        host/bluedroid/main                        \
                        host/bluedroid/external/sbc/decoder/srce   \
                        host/bluedroid/external/sbc/encoder/srce   \
                        host/bluedroid/external/sbc/plc            \
                        host/bluedroid/btc/core                    \
                        host/bluedroid/btc/profile/std/gap         \
                        host/bluedroid/btc/profile/std/gatt        \
                        host/bluedroid/btc/profile/std/a2dp        \
                        host/bluedroid/btc/profile/std/avrc        \
                        host/bluedroid/btc/profile/std/spp         \
                        host/bluedroid/btc/profile/std/hf_ag       \
                        host/bluedroid/btc/profile/std/hf_client   \
                        host/bluedroid/btc/profile/std/hid         \
                        host/bluedroid/btc/profile                 \
                        host/bluedroid/stack/btm                   \
                        host/bluedroid/stack/btu                   \
                        host/bluedroid/stack/gap                   \
                        host/bluedroid/stack/gatt                  \
                        host/bluedroid/stack/hcic                  \
                        host/bluedroid/stack/hid                   \
                        host/bluedroid/stack/include               \
                        host/bluedroid/stack/l2cap                 \
                        host/bluedroid/stack/sdp                   \
                        host/bluedroid/stack/smp                   \
                        host/bluedroid/stack/avct                  \
                        host/bluedroid/stack/avrc                  \
                        host/bluedroid/stack/avdt                  \
                        host/bluedroid/stack/a2dp                  \
                        host/bluedroid/stack/rfcomm                \
                        host/bluedroid/stack                       \
                        host/bluedroid/utils                       \
                        host/bluedroid/api                         \
                        host/bluedroid


host/bluedroid/bta/sdp/bta_sdp_act.o: CFLAGS += -Wno-unused-const-variable
host/bluedroid/btc/core/btc_config.o: CFLAGS += -Wno-unused-const-variable
host/bluedroid/stack/btm/btm_sec.o: CFLAGS += -Wno-unused-const-variable
host/bluedroid/stack/smp/smp_keys.o: CFLAGS += -Wno-unused-const-variable

COMPONENT_SRCDIRS += common/btc/profile/esp/blufi/bluedroid_host

ifdef CONFIG_BLE_MESH

COMPONENT_SRCDIRS += esp_ble_mesh/mesh_core/bluedroid_host

endif
endif

ifdef CONFIG_BLE_MESH
COMPONENT_ADD_INCLUDEDIRS += esp_ble_mesh/mesh_common/include           \
                             esp_ble_mesh/mesh_common/tinycrypt/include \
                             esp_ble_mesh/mesh_core                     \
                             esp_ble_mesh/mesh_core/include             \
                             esp_ble_mesh/mesh_core/storage             \
                             esp_ble_mesh/btc/include                   \
                             esp_ble_mesh/mesh_models/common/include    \
                             esp_ble_mesh/mesh_models/client/include    \
                             esp_ble_mesh/mesh_models/server/include    \
                             esp_ble_mesh/api/core/include              \
                             esp_ble_mesh/api/models/include            \
                             esp_ble_mesh/api

COMPONENT_SRCDIRS += esp_ble_mesh/mesh_common               \
                     esp_ble_mesh/mesh_common/tinycrypt/src \
                     esp_ble_mesh/mesh_core                 \
                     esp_ble_mesh/mesh_core/storage         \
                     esp_ble_mesh/btc                       \
                     esp_ble_mesh/mesh_models/common        \
                     esp_ble_mesh/mesh_models/client        \
                     esp_ble_mesh/mesh_models/server        \
                     esp_ble_mesh/api/core                  \
                     esp_ble_mesh/api/models
endif


ifdef CONFIG_BT_NIMBLE_ENABLED

COMPONENT_ADD_INCLUDEDIRS += host/nimble/nimble/nimble/include                     \
                             host/nimble/nimble/nimble/host/include                \
                             host/nimble/nimble/porting/nimble/include             \
                             host/nimble/nimble/porting/npl/freertos/include       \
                             host/nimble/nimble/nimble/host/services/ans/include   \
                             host/nimble/nimble/nimble/host/services/bas/include   \
                             host/nimble/nimble/nimble/host/services/dis/include   \
                             host/nimble/nimble/nimble/host/services/gap/include   \
                             host/nimble/nimble/nimble/host/services/gatt/include  \
                             host/nimble/nimble/nimble/host/services/ias/include   \
                             host/nimble/nimble/nimble/host/services/ipss/include  \
                             host/nimble/nimble/nimble/host/services/lls/include   \
                             host/nimble/nimble/nimble/host/services/tps/include   \
                             host/nimble/nimble/nimble/host/util/include           \
                             host/nimble/nimble/nimble/host/store/ram/include      \
                             host/nimble/nimble/nimble/host/store/config/include   \
                             host/nimble/esp-hci/include                           \
                             host/nimble/port/include

ifndef CONFIG_BT_NIMBLE_CRYPTO_STACK_MBEDTLS
COMPONENT_ADD_INCLUDEDIRS += host/nimble/nimble/ext/tinycrypt/include
endif

COMPONENT_SRCDIRS += host/nimble/nimble/nimble/host/src                            \
                     host/nimble/nimble/porting/nimble/src                         \
                     host/nimble/nimble/porting/npl/freertos/src                   \
                     host/nimble/nimble/nimble/host/services/ans/src               \
                     host/nimble/nimble/nimble/host/services/bas/src               \
                     host/nimble/nimble/nimble/host/services/dis/src               \
                     host/nimble/nimble/nimble/host/services/gap/src               \
                     host/nimble/nimble/nimble/host/services/gatt/src              \
                     host/nimble/nimble/nimble/host/services/ias/src               \
                     host/nimble/nimble/nimble/host/services/ipss/src              \
                     host/nimble/nimble/nimble/host/services/lls/src               \
                     host/nimble/nimble/nimble/host/services/tps/src               \
                     host/nimble/nimble/nimble/host/util/src                       \
                     host/nimble/nimble/nimble/host/store/ram/src                  \
                     host/nimble/nimble/nimble/host/store/config/src               \
                     host/nimble/esp-hci/src                                       \
                     host/nimble/port/src

ifndef CONFIG_BT_NIMBLE_CRYPTO_STACK_MBEDTLS
COMPONENT_SRCDIRS += host/nimble/nimble/ext/tinycrypt/src
endif

COMPONENT_OBJEXCLUDE += host/nimble/nimble/nimble/host/store/config/src/ble_store_config_conf.o

ifdef CONFIG_BLE_MESH

COMPONENT_SRCDIRS += esp_ble_mesh/mesh_core/nimble_host

endif

ifdef CONFIG_BT_NIMBLE_MESH

COMPONENT_ADD_INCLUDEDIRS += host/nimble/nimble/nimble/host/mesh/include
COMPONENT_SRCDIRS += host/nimble/nimble/nimble/host/mesh/src

endif
COMPONENT_SRCDIRS += common/btc/profile/esp/blufi/nimble_host
endif

endif
