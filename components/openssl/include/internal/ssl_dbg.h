#ifndef _SSL_DEBUG_H_
#define _SSL_DEBUG_H_

#define SSL_DEBUG_ENBALE 0
#define SSL_DEBUG_LEVEL 0
#define SSL_ASSERT_ENABLE 1
#define SSL_DEBUG_LOCATION_ENABLE 1

#if SSL_DEBUG_ENBALE
    #define SSL_PRINT os_printf
#else
    #define SSL_PRINT(...)
#endif

#if SSL_DEBUG_LOCATION_ENABLE
    #define SSL_DEBUG_LOCATION() SSL_PRINT("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__)
#else
    #define SSL_DEBUG_LOCATION()
#endif

#if SSL_ASSERT_ENABLE
    #define SSL_ASSERT(s) { if (!(s)) { SSL_DEBUG_LOCATION(); } }
#else
    #define SSL_ASSERT(s)
#endif

#define SSL_ERR(err, go, ...) { SSL_DEBUG_LOCATION(); SSL_PRINT(__VA_ARGS__); ret = err; goto go; }

#define SSL_RET(go, ...) {  SSL_DEBUG_LOCATION(); SSL_PRINT(__VA_ARGS__); goto go; }

#define SSL_DEBUG(level, ...) { if (level > SSL_DEBUG_LEVEL) {SSL_PRINT(__VA_ARGS__);} }

#endif
