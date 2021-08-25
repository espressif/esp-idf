set(public_include_dirs
    "include"
    "openthread/include")

set(private_include_dirs
    "openthread/examples/platforms"
    "openthread/include/openthread"
    "openthread/src"
    "openthread/src/core"
    "openthread/src/lib/hdlc"
    "openthread/src/lib/spinel"
    "openthread/src/ncp"
    "private_include"
    )

set(src_dirs
    "openthread/examples/apps/cli"
    "openthread/examples/platforms/utils"
    "openthread/src/core/api"
    "openthread/src/core/backbone_router"
    "openthread/src/core/coap"
    "openthread/src/core/common"
    "openthread/src/core/crypto"
    "openthread/src/core/diags"
    "openthread/src/core/mac"
    "openthread/src/core/meshcop"
    "openthread/src/core/net"
    "openthread/src/core/radio"
    "openthread/src/core/thread"
    "openthread/src/core/utils"
    "openthread/src/cli"
    "openthread/src/lib/hdlc"
    "openthread/src/lib/spinel"
    "openthread/src/lib/platform")

set(exclude_srcs
    "openthread/examples/apps/cli/main.c"
    "openthread/examples/platforms/utils/logging_rtt.c"
    "openthread/examples/platforms/utils/soft_source_match_table.c"
    "openthread/src/core/common/extension_example.cpp")

if(CONFIG_OPENTHREAD_BORDER_ROUTER)
    list(APPEND src_dirs
   "openthread/src/core/border_router")
endif()

set_source_files_properties("openthread/src/core/net/srp_server.cpp"
    PROPERTIES COMPILE_FLAGS
    -Wno-maybe-uninitialized)

if(CONFIG_OPENTHREAD_FTD)
    set(device_type "OPENTHREAD_FTD=1")
endif()
