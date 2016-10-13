#ifndef __HAVE_CONFIG_H_
#define __HAVE_CONFIG_H_

#define _U_

#define SIZEOF_INT_P 2

//#define DEBUGBUILD
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#if (!defined(nghttp_unlikely))
#define nghttp_unlikely(Expression)	!!(Expression)
#endif

#define nghttp_ASSERT(Expression) do{if (!(Expression)) printf("%d\n", __LINE__);}while(0)

#define CU_ASSERT(a) nghttp_ASSERT(a)
#define CU_ASSERT_FATAL(a) nghttp_ASSERT(a)

#if 1
#define NGHTTP2_DEBUG_INFO() printf("%s %d\n", __FILE__, __LINE__)
#else
#define NGHTTP2_DEBUG_INFO()
#endif

#define NGHTTP_PLATFORM_HTONS(_n)  ((uint16_t)((((_n) & 0xff) << 8) | (((_n) >> 8) & 0xff)))
#define NGHTTP_PLATFORM_HTONL(_n)  ((uint32_t)( (((_n) & 0xff) << 24) | (((_n) & 0xff00) << 8) | (((_n) >> 8)  & 0xff00) | (((_n) >> 24) & 0xff) ))

#define htons(x) NGHTTP_PLATFORM_HTONS(x)
#define ntohs(x) NGHTTP_PLATFORM_HTONS(x)
#define htonl(x) NGHTTP_PLATFORM_HTONL(x)
#define ntohl(x) NGHTTP_PLATFORM_HTONL(x)

#endif
