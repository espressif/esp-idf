ifdef CONFIG_OPENTHREAD_ENABLED

COMPONENT_ADD_INCLUDEDIRS :=       \
    openthread/include             \
    include

COMPONENT_PRIV_INCLUDEDIRS :=      \
    openthread/examples/platforms  \
    openthread/src                 \
    openthread/src/core            \
    openthread/src/lib/hdlc        \
    openthread/src/lib/spinel      \
    openthread/src/ncp             \
    private_include

COMPONENT_SRCDIRS :=                      \
    openthread/examples/apps/cli          \
    openthread/src/cli                    \
    openthread/src/core                   \
    openthread/src/core/api               \
    openthread/src/core/backbone_router   \
    openthread/src/core/coap              \
    openthread/src/core/common            \
    openthread/src/core/crypto            \
    openthread/src/core/diags             \
    openthread/src/core/mac               \
    openthread/src/core/meshcop           \
    openthread/src/core/net               \
    openthread/src/core/radio             \
    openthread/src/core/thread            \
    openthread/src/core/utils             \
    openthread/src/lib/hdlc               \
    openthread/src/lib/platform           \
    openthread/src/lib/spinel             \
    port

ifdef CONFIG_OPENTHREAD_BORDER_ROUTER
COMPONENT_SRCDIRS += openthread/src/core/border_router
endif

COMPONENT_OBJEXCLUDE :=                              \
    openthread/examples/apps/cli/main.o              \
    openthread/src/core/common/extension_example.o   \


COMPONENT_SUBMODULES += lib

ALL_LIB_FILES := $(COMPONENT_PATH)/lib/$(IDF_TARGET)/libopenthread_port.a

ifdef CONFIG_OPENTHREAD_BORDER_ROUTER
    ALL_LIB_FILES += $(COMPONENT_PATH)/lib/$(IDF_TARGET)/libopenthread_br.a
endif
COMPONENT_ADD_LDFLAGS += $(ALL_LIB_FILES)

IDF_VERSION_FOR_OPENTHREAD_PACKAGE := $(shell git -C $(COMPONENT_PATH) rev-parse --short HEAD)
OPENTHREAD_VERSION := $(shell git -C $(COMPONENT_PATH)/openthread rev-parse --short HEAD)
OPENTHREAD_PACKAGE_VERSION := $(IDF_VERSION_FOR_OPENTHREAD_PACKAGE)-$(OPENTHREAD_VERSION)

COMMON_FLAGS :=                                                              \
    -DOPENTHREAD_CONFIG_FILE=\<openthread-core-esp32x-ftd-config.h\>             \
    -DPACKAGE_VERSION=\"OPENTHREAD_PACKAGE_VERSION\"                         \
    -Wno-maybe-uninitialized

ifdef CONFIG_OPENTHREAD_FTD
    COMMON_FLAGS += -DOPENTHREAD_FTD=1
else ifdef CONFIG_OPENTHREAD_MTD
    COMMON_FLAGS += -DOPENTHREAD_MTD=1
else ifdef CONFIG_OPENTHREAD_RADIO
    COMMON_FLAGS += -DOPENTHREAD_RADIO=1
endif

CFLAGS += $(COMMON_FLAGS)

CXXFLAGS += $(COMMON_FLAGS)

CPPFLAGS += $(COMMON_FLAGS)

endif
