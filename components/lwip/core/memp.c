/**
 * @file
 * Dynamic pool memory manager
 *
 * lwIP has dedicated pools for many structures (netconn, protocol control blocks,
 * packet buffers, ...). All these pools are managed here.
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "lwip/opt.h"

#include "lwip/memp.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/raw.h"
#include "lwip/igmp.h"
#include "lwip/api.h"
#include "lwip/priv/api_msg.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/timers.h"
#include "lwip/stats.h"
#include "netif/etharp.h"
#include "lwip/ip_frag.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "netif/ppp/ppp.h"
#include "netif/ppp/pppos.h"
#include "netif/ppp/pppoe.h"
#include "netif/ppp/pppol2tp.h"
#include "lwip/nd6.h"
#include "lwip/ip6_frag.h"
#include "lwip/mld6.h"
#include "lwip/tcp.h"
#include "lwip/tcpip.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/priv/tcpip_priv.h"
#include "lwip/netifapi.h"

#include <string.h>

#define LWIP_MEMPOOL(name,num,size,desc) LWIP_MEMPOOL_DECLARE(name,num,size,desc)
#include "lwip/priv/memp_std.h"

const struct memp_desc* const memp_pools[MEMP_MAX] = {
#define LWIP_MEMPOOL(name,num,size,desc) &memp_ ## name,
#include "lwip/priv/memp_std.h"
};

#if !MEMP_MEM_MALLOC /* don't build if not configured for use in lwipopts.h */

#if MEMP_SANITY_CHECK
/**
 * Check that memp-lists don't form a circle, using "Floyd's cycle-finding algorithm".
 */
static int
memp_sanity(const struct memp_desc *desc)
{
  struct memp *t, *h;

  t = *desc->tab;
  if (t != NULL) {
    for (h = t->next; (t != NULL) && (h != NULL); t = t->next,
      h = ((h->next != NULL) ? h->next->next : NULL)) {
      if (t == h) {
        return 0;
      }
    }
  }

  return 1;
}
#endif /* MEMP_SANITY_CHECK*/

#if MEMP_OVERFLOW_CHECK

/**
 * Check if a memp element was victim of an overflow
 * (e.g. the restricted area after it has been altered)
 *
 * @param p the memp element to check
 * @param memp_type the pool p comes from
 */
static void
memp_overflow_check_element_overflow(struct memp *p, const struct memp_desc *desc)
{
  u16_t k;
  u8_t *m;
#if MEMP_SANITY_REGION_AFTER_ALIGNED > 0
  m = (u8_t*)p + MEMP_SIZE + desc->size;
  for (k = 0; k < MEMP_SANITY_REGION_AFTER_ALIGNED; k++) {
    if (m[k] != 0xcd) {
      char errstr[128] = "detected memp overflow in pool ";
      strcat(errstr, desc->desc);
      LWIP_ASSERT(errstr, 0);
    }
  }
#endif
}

/**
 * Check if a memp element was victim of an underflow
 * (e.g. the restricted area before it has been altered)
 *
 * @param p the memp element to check
 * @param memp_type the pool p comes from
 */
static void
memp_overflow_check_element_underflow(struct memp *p, const struct memp_desc *desc)
{
  u16_t k;
  u8_t *m;
#if MEMP_SANITY_REGION_BEFORE_ALIGNED > 0
  m = (u8_t*)p + MEMP_SIZE - MEMP_SANITY_REGION_BEFORE_ALIGNED;
  for (k = 0; k < MEMP_SANITY_REGION_BEFORE_ALIGNED; k++) {
    if (m[k] != 0xcd) {
      char errstr[128] = "detected memp underflow in pool ";
      strcat(errstr, desc->desc);
      LWIP_ASSERT(errstr, 0);
    }
  }
#endif
}

/**
 * Do an overflow check for all elements in every pool.
 *
 * @see memp_overflow_check_element for a description of the check
 */
static void
memp_overflow_check_all(void)
{
  u16_t i, j;
  struct memp *p;

  for (i = 0; i < MEMP_MAX; ++i) {
    p = (struct memp *)(size_t)(memp_pools[i]->base);
    for (j = 0; j < memp_pools[i]->num; ++j) {
      memp_overflow_check_element_overflow(p, memp_pools[i]);
      memp_overflow_check_element_underflow(p, memp_pools[i]);
      p = (struct memp*)(size_t)((u8_t*)p + MEMP_SIZE + memp_pools[i]->size + MEMP_SANITY_REGION_AFTER_ALIGNED);
    }
  }
}

/**
 * Initialize the restricted areas of all memp elements in every pool.
 */
static void
memp_overflow_init(const struct memp_desc *desc)
{
  u16_t i;
  struct memp *p;
  u8_t *m;

  p = (struct memp *)(size_t)(desc->base);
  for (i = 0; i < desc->num; ++i) {
#if MEMP_SANITY_REGION_BEFORE_ALIGNED > 0
    m = (u8_t*)p + MEMP_SIZE - MEMP_SANITY_REGION_BEFORE_ALIGNED;
    memset(m, 0xcd, MEMP_SANITY_REGION_BEFORE_ALIGNED);
#endif
#if MEMP_SANITY_REGION_AFTER_ALIGNED > 0
    m = (u8_t*)p + MEMP_SIZE + desc->size;
    memset(m, 0xcd, MEMP_SANITY_REGION_AFTER_ALIGNED);
#endif
    p = (struct memp*)(size_t)((u8_t*)p + MEMP_SIZE + desc->size + MEMP_SANITY_REGION_AFTER_ALIGNED);
  }
}
#endif /* MEMP_OVERFLOW_CHECK */


void
memp_init_pool(const struct memp_desc *desc)
{
  int i;
  struct memp *memp;
  
  *desc->tab = NULL;
  memp = (struct memp*)LWIP_MEM_ALIGN(desc->base);
  /* create a linked list of memp elements */
  for (i = 0; i < desc->num; ++i) {
    memp->next = *desc->tab;
    *desc->tab = memp;
    memp = (struct memp *)(void *)((u8_t *)memp + MEMP_SIZE + desc->size
#if MEMP_OVERFLOW_CHECK
      + MEMP_SANITY_REGION_AFTER_ALIGNED
#endif
    );
  }  

#if MEMP_OVERFLOW_CHECK
  memp_overflow_init(desc);
#endif /* MEMP_OVERFLOW_CHECK */
}

/**
 * Initialize this module.
 *
 * Carves out memp_memory into linked lists for each pool-type.
 */
void
memp_init(void)
{
  u16_t i;

  for (i = 0; i < MEMP_MAX; ++i) {
    MEMP_STATS_AVAIL(used, i, 0);
    MEMP_STATS_AVAIL(max, i, 0);
    MEMP_STATS_AVAIL(err, i, 0);
    MEMP_STATS_AVAIL(avail, i, memp_pools[i]->num);
  }

  /* for every pool: */
  for (i = 0; i < MEMP_MAX; ++i) {
    memp_init_pool(memp_pools[i]);
  }

#if MEMP_OVERFLOW_CHECK
  /* check everything a first time to see if it worked */
  memp_overflow_check_all();
#endif /* MEMP_OVERFLOW_CHECK */
}

void *
#if !MEMP_OVERFLOW_CHECK
memp_malloc_pool(const struct memp_desc *desc)
#else
memp_malloc_pool_fn(const struct memp_desc *desc, const char* file, const int line)
#endif
{
  struct memp *memp;
  SYS_ARCH_DECL_PROTECT(old_level);

  SYS_ARCH_PROTECT(old_level);

  memp = *desc->tab;

#if MEMP_OVERFLOW_CHECK == 1
  memp_overflow_check_element_overflow(memp, desc);
  memp_overflow_check_element_underflow(memp, desc);
#endif /* MEMP_OVERFLOW_CHECK */

  if (memp != NULL) {
    *desc->tab = memp->next;
#if MEMP_OVERFLOW_CHECK
    memp->next = NULL;
    memp->file = file;
    memp->line = line;
#endif /* MEMP_OVERFLOW_CHECK */
    LWIP_ASSERT("memp_malloc: memp properly aligned",
                ((mem_ptr_t)memp % MEM_ALIGNMENT) == 0);
    memp = (struct memp*)(void *)((u8_t*)memp + MEMP_SIZE);
  }

  SYS_ARCH_UNPROTECT(old_level);

  return memp;
}

/**
 * Get an element from a specific pool.
 *
 * @param type the pool to get an element from
 *
 * the debug version has two more parameters:
 * @param file file name calling this function
 * @param line number of line where this function is called
 *
 * @return a pointer to the allocated memory or a NULL pointer on error
 */
void *
#if !MEMP_OVERFLOW_CHECK
memp_malloc(memp_t type)
#else
memp_malloc_fn(memp_t type, const char* file, const int line)
#endif
{
  void *memp;
  SYS_ARCH_DECL_PROTECT(old_level);

  LWIP_ERROR("memp_malloc: type < MEMP_MAX", (type < MEMP_MAX), return NULL;);

  SYS_ARCH_PROTECT(old_level);

#if MEMP_OVERFLOW_CHECK >= 2
  memp_overflow_check_all();
#endif /* MEMP_OVERFLOW_CHECK >= 2 */

#if !MEMP_OVERFLOW_CHECK
  memp = memp_malloc_pool(memp_pools[type]);
#else
  memp = memp_malloc_pool_fn(memp_pools[type], file, line);
#endif

  if (memp != NULL) {
    MEMP_STATS_INC(used, type);
    if(MEMP_STATS_GET(used, type) > MEMP_STATS_GET(max, type)) {
      MEMP_STATS_AVAIL(max, type, MEMP_STATS_GET(used, type));
    }
  } else {
    LWIP_DEBUGF(MEMP_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("memp_malloc: out of memory in pool %s\n", memp_pools[type]->desc));
    MEMP_STATS_INC(err, type);
  }

  SYS_ARCH_UNPROTECT(old_level);

  return memp;
}

static void
#ifdef LWIP_HOOK_MEMP_AVAILABLE
do_memp_free_pool(const struct memp_desc* desc, void *mem, struct memp **old_first)
#else
do_memp_free_pool(const struct memp_desc* desc, void *mem)
#endif
{
  struct memp *memp;
  SYS_ARCH_DECL_PROTECT(old_level);

  LWIP_ASSERT("memp_free: mem properly aligned",
                ((mem_ptr_t)mem % MEM_ALIGNMENT) == 0);

  memp = (struct memp *)(void *)((u8_t*)mem - MEMP_SIZE);

  SYS_ARCH_PROTECT(old_level);

#if MEMP_OVERFLOW_CHECK == 1
  memp_overflow_check_element_overflow(memp, desc);
  memp_overflow_check_element_underflow(memp, desc);
#endif /* MEMP_OVERFLOW_CHECK */

  memp->next = *desc->tab;

#ifdef LWIP_HOOK_MEMP_AVAILABLE
  if (old_first)
    *old_first = *desc->tab;
#endif

  *desc->tab = memp;

#if MEMP_SANITY_CHECK
  LWIP_ASSERT("memp sanity", memp_sanity(desc));
#endif /* MEMP_SANITY_CHECK */

  SYS_ARCH_UNPROTECT(old_level);
}

void
memp_free_pool(const struct memp_desc* desc, void *mem)
{
  if ((desc == NULL) || (mem == NULL)) {
    return;
  }

#ifdef LWIP_HOOK_MEMP_AVAILABLE
  do_memp_free_pool(desc, mem, NULL);
#else
  do_memp_free_pool(desc, mem);
#endif
}

/**
 * Put an element back into its pool.
 *
 * @param type the pool where to put mem
 * @param mem the memp element to free
 */
void
memp_free(memp_t type, void *mem)
{
#ifdef LWIP_HOOK_MEMP_AVAILABLE
  struct memp *old_first;
#endif
  SYS_ARCH_DECL_PROTECT(old_level);

  LWIP_ERROR("memp_free: type < MEMP_MAX", (type < MEMP_MAX), return;);

  SYS_ARCH_PROTECT(old_level);

#if MEMP_OVERFLOW_CHECK >= 2
  memp_overflow_check_all();
#endif /* MEMP_OVERFLOW_CHECK >= 2 */

  MEMP_STATS_DEC(used, type);

#ifdef LWIP_HOOK_MEMP_AVAILABLE
  do_memp_free_pool(memp_pools[type], mem, &old_first);
#else
  do_memp_free_pool(memp_pools[type], mem);
#endif

  SYS_ARCH_UNPROTECT(old_level);

#ifdef LWIP_HOOK_MEMP_AVAILABLE
  if (old_first == NULL) {
    LWIP_HOOK_MEMP_AVAILABLE(type);
  }
#endif
}

#endif /* MEMP_MEM_MALLOC */
