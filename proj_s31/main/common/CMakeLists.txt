# Included by ../CMakeLists.txt. Only sources under common/.
# Paths are relative to the main component directory.
# Do not call idf_component_register here.

set(COMMON_SRCS
    "common/filemgr/app_filemgr.c"
    "common/filemgr/cmd_filemgr.c"
    "common/cmd_system/cmdmgr.c"
    "common/cmd_system/cmd_system.c"
    "common/cmd_system/cmd_system_common.c"
    "common/cmd_nvs/cmd_nvs.c"
    "common/osal/osal.c"
    "common/sntp/common_sntp.c"
)

if(CONFIG_SOC_DEEP_SLEEP_SUPPORTED OR CONFIG_SOC_LIGHT_SLEEP_SUPPORTED)
    list(APPEND COMMON_SRCS "common/cmd_system/cmd_system_sleep.c")
endif()

if(CONFIG_FTP_SERVER_SUPPORT)
    list(APPEND COMMON_SRCS "common/ftp_server/app_ftpsrv.c")
    list(APPEND COMMON_SRCS "common/ftp_server/cmd_ftpsrv.c")
endif()

if(CONFIG_OTA_SUPPORT)
    list(APPEND COMMON_SRCS "common/ota/ota.c")
    list(APPEND COMMON_SRCS "common/ota/cmd_ota.c")
    if(CONFIG_XMODEM_OTA_SUPPORT)
        list(APPEND COMMON_SRCS "common/ota/ota_xmodem.c")
    endif()
    if(CONFIG_HTTPS_OTA_SUPPORT)
        list(APPEND COMMON_SRCS "common/ota/ota_https.c")
    endif()
endif()

set(COMMON_INCLUDE_DIRS
    "common"
    "common/filemgr"
    "common/cmd_system"
    "common/cmd_nvs"
    "common/ftp_server"
    "common/osal"
    "common/sntp"
)

if(CONFIG_OTA_SUPPORT)
    list(APPEND COMMON_INCLUDE_DIRS "common/ota")
endif()
