# tlsio_openssl_compact


## Overview

The tlsio_openssl_compact adapter implements a tlsio adapter for compact OpenSSL platforms such as Espressif's ESP32.  


## References

[tlsio base specification](https://github.com/Azure/azure-c-shared-utility/blob/master/devdoc/tlsio_requirements.md)

[OpenSSL](https://www.openssl.org/)

## Base Specification

The tlsio_openssl_compact adapter conforms to the
[tlsio base specification](https://github.com/Azure/azure-c-shared-utility/blob/master/devdoc/tlsio_requirements.md) and 
additionally implements the additional requirements below.


## API Calls


###   tlsio_setoption

**SRS_TLSIO_OPENSSL_COMPACT_30_520 [** The `tlsio_setoption` shall do nothing and return __FAILURE__. **]**


###   tlsio_retrieveoptions

**SRS_TLSIO_OPENSSL_COMPACT_30_560: [** The `tlsio_retrieveoptions` shall return an empty options handler. **]**


