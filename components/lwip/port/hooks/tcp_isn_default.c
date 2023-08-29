/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * @file
 *
 * Reference implementation of the TCP ISN algorithm standardized in RFC 6528.
 * Produce TCP Initial Sequence Numbers by combining an MD5-generated hash
 * based on the new TCP connection's identity and a stable secret, with the
 * current time at 4-microsecond granularity.
 *
 * Specifically, the implementation uses MD5 to compute a hash of the input
 * buffer, which contains both the four-tuple of the new TCP connection (local
 * and remote IP address and port), as well as a 16-byte secret to make the
 * results unpredictable to external parties.  The secret must be given at
 * initialization time and should ideally remain the same across system
 * reboots.  To be sure: the spoofing-resistance of the resulting ISN depends
 * mainly on the strength of the supplied secret!
 *
 * The implementation takes 32 bits from the computed hash, and adds to it the
 * current time, in 4-microsecond units.  The current time is computed from a
 * boot time given at initialization, and the current uptime as provided by
 * sys_now().  Thus, it assumes that sys_now() returns a time value that is
 * relative to the boot time, i.e., that it starts at 0 at system boot, and
 * only ever increases monotonically.
 *
 * For efficiency reasons, a single MD5 input buffer is used, and partially
 * filled in at initialization time.  Specifically, of this 64-byte buffer, the
 * first 36 bytes are used for the four-way TCP tuple data, followed by the
 * 16-byte secret, followed by 12-byte zero padding.  The 64-byte size of the
 * buffer should achieve the best performance for the actual MD5 computation.
 *
 * Basic usage:
 *
 * 1. in your lwipopts.h, add the following lines:
 *
 *    #include <lwip/arch.h>
 *    struct ip_addr;
 *    u32_t lwip_hook_tcp_isn(const struct ip_addr *local_ip, u16_t local_port,
 *      const struct ip_addr *remote_ip, u16_t remote_port);
 *   "#define LWIP_HOOK_TCP_ISN lwip_hook_tcp_isn";
 *
 * 2. from your own code, call lwip_init_tcp_isn() at initialization time, with
 *    appropriate parameters.
 */

/*
 * Copyright (c) 2016 The MINIX 3 Project.
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
 * Author: David van Moolenbroek <david@minix3.org>
 */

#include "lwip_default_hooks.h"
#include "lwip/ip_addr.h"
#include "lwip/sys.h"
#include <string.h>
#include "esp_rom_md5.h"
#ifdef CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
#include "esp_memory_utils.h"
#endif

#ifdef CONFIG_LWIP_HOOK_TCP_ISN_DEFAULT

static u8_t input[64];
static u32_t base_time;

/**
 * Initialize the TCP ISN module, with the boot time and a secret.
 *
 * @param boot_time Wall clock boot time of the system, in seconds.
 * @param secret_16_bytes A 16-byte secret used to randomize the TCP ISNs.
 */
void
lwip_init_tcp_isn(u32_t boot_time, const u8_t *secret_16_bytes)
{
  /* Initialize the input buffer with the secret and trailing zeroes. */
  memset(input, 0, sizeof(input));

  MEMCPY(&input[36], secret_16_bytes, 16);

  /* Save the boot time in 4-us units. Overflow is no problem here. */
  base_time = boot_time * 250000;
}

/**
 * Hook to generate an Initial Sequence Number (ISN) for a new TCP connection.
 *
 * @param local_ip The local IP address.
 * @param local_port The local port number, in host-byte order.
 * @param remote_ip The remote IP address.
 * @param remote_port The remote port number, in host-byte order.
 * @return The ISN to use for the new TCP connection.
 */
u32_t
lwip_hook_tcp_isn(const ip_addr_t *local_ip, u16_t local_port,
    const ip_addr_t *remote_ip, u16_t remote_port)
{
  u8_t output[16];
  u32_t isn;

#if LWIP_IPV4 && LWIP_IPV6
  if (IP_IS_V6(local_ip))
#endif /* LWIP_IPV4 && LWIP_IPV6 */
#if LWIP_IPV6
  {
    const ip6_addr_t *local_ip6, *remote_ip6;

    local_ip6  = ip_2_ip6(local_ip);
    remote_ip6 = ip_2_ip6(remote_ip);

    SMEMCPY(&input[0],  &local_ip6->addr,  16);
    SMEMCPY(&input[16], &remote_ip6->addr, 16);
  }
#endif /* LWIP_IPV6 */
#if LWIP_IPV4 && LWIP_IPV6
  else
#endif /* LWIP_IPV4 && LWIP_IPV6 */
#if LWIP_IPV4
  {
    const ip4_addr_t *local_ip4, *remote_ip4;

    local_ip4  = ip_2_ip4(local_ip);
    remote_ip4 = ip_2_ip4(remote_ip);

    /* Represent IPv4 addresses as IPv4-mapped IPv6 addresses, to ensure that
     * the IPv4 and IPv6 address spaces are completely disjoint. */
    memset(&input[0], 0, 10);
    input[10] = 0xff;
    input[11] = 0xff;
    SMEMCPY(&input[12], &local_ip4->addr, 4);
    memset(&input[16], 0, 10);
    input[26] = 0xff;
    input[27] = 0xff;
    SMEMCPY(&input[28], &remote_ip4->addr, 4);
  }
#endif /* LWIP_IPV4 */

  input[32] = (u8_t)(local_port >> 8);
  input[33] = (u8_t)(local_port & 0xff);
  input[34] = (u8_t)(remote_port >> 8);
  input[35] = (u8_t)(remote_port & 0xff);

  /* The secret and padding are already filled in. */

  /*
   * Generate the hash using ROM MD5 APIs
   * This hook is invoked in the context of TCP/IP (tiT) task and
   * it is unlikely that its stack would be placed in SPIRAM. Hence
   * even with SPIRAM enabled case and ESP32 revision < 3, using ROM
   * APIs should not create any issues.
   */
#if CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
  assert(!esp_ptr_external_ram(esp_cpu_get_sp()));
#endif

  md5_context_t ctx;
  esp_rom_md5_init(&ctx);
  esp_rom_md5_update(&ctx, input, sizeof(input));
  esp_rom_md5_final(output, &ctx);


  /* Arbitrarily take the first 32 bits from the generated hash. */
  MEMCPY(&isn, output, sizeof(isn));

  /* Add the current time in 4-microsecond units. */
  return isn + base_time + sys_now() * 250;
}

#endif /* LWIP_HOOK_TCP_ISN */
