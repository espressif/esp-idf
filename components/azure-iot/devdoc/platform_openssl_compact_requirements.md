platform_openssl_compact
=============

## Overview

**platform_openssl_compact** implements the **platform** adapter for the Azure IoT C SDK. This implementation
uses a version of the **tlsio** adapter for OpenSSL which is aimed at embedded applications.

Unlike non-micro systems such as Windows and Linux, embedded systems typically
require explicit NTP initialization, so platform_openssl_compact performs NTP
initialization and deinitialization using the interfaces in `azure-c-shared-utility/inc/azure_c_shared_utility/sntp.h`.

## References
[sntp.h](https://github.com/Azure/azure-c-shared-utility/tree/master/inc/azure_c_shared_utility/sntp.h)

###   Standard

**SRS_PLATFORM_OPENSSL_COMPACT_30_001: [** The platform_openssl_compact shall implement the interface provided in the `platfom.h`.


```c
    int platform_init();
    void platform_deinit();
    const IO_INTERFACE_DESCRIPTION* platform_get_default_tlsio();
```
 **]**

**SRS_PLATFORM_OPENSSL_COMPACT_30_002: [** The platform_openssl_compact shall use the tlsio functions defined in 'xio.h'.
```c
typedef OPTIONHANDLER_HANDLE (*IO_RETRIEVEOPTIONS)(CONCRETE_IO_HANDLE concrete_io);
typedef CONCRETE_IO_HANDLE(*IO_CREATE)(void* io_create_parameters);
typedef void(*IO_DESTROY)(CONCRETE_IO_HANDLE concrete_io);
typedef int(*IO_OPEN)(CONCRETE_IO_HANDLE concrete_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context);
typedef int(*IO_CLOSE)(CONCRETE_IO_HANDLE concrete_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context);
typedef int(*IO_SEND)(CONCRETE_IO_HANDLE concrete_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context);
typedef void(*IO_DOWORK)(CONCRETE_IO_HANDLE concrete_io);
typedef int(*IO_SETOPTION)(CONCRETE_IO_HANDLE concrete_io, const char* optionName, const void* value);

typedef struct IO_INTERFACE_DESCRIPTION_TAG
{
    IO_RETRIEVEOPTIONS concrete_io_retrieveoptions;
    IO_CREATE concrete_io_create;
    IO_DESTROY concrete_io_destroy;
    IO_OPEN concrete_io_open;
    IO_CLOSE concrete_io_close;
    IO_SEND concrete_io_send;
    IO_DOWORK concrete_io_dowork;
    IO_SETOPTION concrete_io_setoption;
} IO_INTERFACE_DESCRIPTION;
```
 **]**

**SRS_PLATFORM_OPENSSL_COMPACT_30_003: [** The platform_openssl_compact shall use the sntp functions defined in 'sntp.h'.
```c
int SNTP_SetServerName(const char*, serverName);
int SNTP_Init();
void SNTP_Deinit();
```
 **]**

###   platform_init

The `platform_init` call performs initialization for the compact (microcontroller) version of the OpenSSL tlsio adapter plus sntp client initialization.

```c
int platform_init();
```

**SRS_PLATFORM_OPENSSL_COMPACT_30_004: [** The platform_init shall initialize the tlsio adapter. **]**

**SRS_PLATFORM_OPENSSL_COMPACT_30_005: [** The platform_init shall initialize the sntp client. **]**


###   platform_deinit

```c
int platform_deinit();
```

**SRS_PLATFORM_OPENSSL_COMPACT_30_006: [** The platform_deinit shall deinitialize the sntp client. (The OpenSSL micro tlsio adapter requires no deinitialization.) **]**


###   platform_get_default_tlsio

```c
const IO_INTERFACE_DESCRIPTION* platform_get_default_tlsio(void);
```

**SRS_PLATFORM_OPENSSL_COMPACT_30_007: [** The platform_get_default_tlsio shall return a set of tlsio functions provided by the OpenSSL micro tlsio implementation. **]**
