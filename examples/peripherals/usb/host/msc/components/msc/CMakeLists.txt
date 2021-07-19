set(sources src/msc_scsi_bot.c
            src/diskio_usb.c
            src/msc_host.c
            src/msc_host_vfs.c)

idf_component_register( SRCS ${sources}
                        INCLUDE_DIRS include
                        PRIV_INCLUDE_DIRS private_include
                        REQUIRES usb fatfs )
