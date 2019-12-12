/* debug.c -- debug utilities
 *
 * Copyright (C) 2010--2012,2014--2019 Olaf Bergmann <bergmann@tzi.org> and others
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap_config.h"

#if defined(HAVE_STRNLEN) && defined(__GNUC__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE 1
#endif

#if defined(HAVE_ASSERT_H) && !defined(assert)
# include <assert.h>
#endif

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include "libcoap.h"
#include "coap_dtls.h"
#include "block.h"
#include "coap_debug.h"
#include "encode.h"
#include "net.h"
#include "coap_mutex.h"

#ifdef WITH_LWIP
# define fprintf(fd, ...) LWIP_PLATFORM_DIAG((__VA_ARGS__))
# define fflush(...)
#endif

#ifdef WITH_CONTIKI
# ifndef DEBUG
#  define DEBUG DEBUG_PRINT
# endif /* DEBUG */
#include "net/ip/uip-debug.h"
#endif

static coap_log_t maxlog = LOG_WARNING;        /* default maximum log level */

static int use_fprintf_for_show_pdu = 1; /* non zero to output with fprintf */

const char *coap_package_name(void) {
  return PACKAGE_NAME;
}

const char *coap_package_version(void) {
  return PACKAGE_STRING;
}

void
coap_set_show_pdu_output(int use_fprintf) {
  use_fprintf_for_show_pdu = use_fprintf;
}

coap_log_t
coap_get_log_level(void) {
  return maxlog;
}

void
coap_set_log_level(coap_log_t level) {
  maxlog = level;
}

/* this array has the same order as the type log_t */
static const char *loglevels[] = {
  "EMRG", "ALRT", "CRIT", "ERR ", "WARN", "NOTE", "INFO", "DEBG"
};

#ifdef HAVE_TIME_H

COAP_STATIC_INLINE size_t
print_timestamp(char *s, size_t len, coap_tick_t t) {
  struct tm *tmp;
  time_t now = coap_ticks_to_rt(t);
  tmp = localtime(&now);
  return strftime(s, len, "%b %d %H:%M:%S", tmp);
}

#else /* alternative implementation: just print the timestamp */

COAP_STATIC_INLINE size_t
print_timestamp(char *s, size_t len, coap_tick_t t) {
#ifdef HAVE_SNPRINTF
  return snprintf(s, len, "%u.%03u",
                  (unsigned int)coap_ticks_to_rt(t),
                  (unsigned int)(t % COAP_TICKS_PER_SECOND));
#else /* HAVE_SNPRINTF */
  /* @todo do manual conversion of timestamp */
  return 0;
#endif /* HAVE_SNPRINTF */
}

#endif /* HAVE_TIME_H */

#ifndef HAVE_STRNLEN
/**
 * A length-safe strlen() fake.
 *
 * @param s      The string to count characters != 0.
 * @param maxlen The maximum length of @p s.
 *
 * @return The length of @p s.
 */
static inline size_t
strnlen(const char *s, size_t maxlen) {
  size_t n = 0;
  while(*s++ && n < maxlen)
    ++n;
  return n;
}
#endif /* HAVE_STRNLEN */

static size_t
print_readable( const uint8_t *data, size_t len,
                unsigned char *result, size_t buflen, int encode_always ) {
  const uint8_t hex[] = "0123456789ABCDEF";
  size_t cnt = 0;
  assert(data || len == 0);

  if (buflen == 0) { /* there is nothing we can do here but return */
    return 0;
  }

  while (len) {
    if (!encode_always && isprint(*data)) {
      if (cnt+1 < buflen) { /* keep one byte for terminating zero */
      *result++ = *data;
      ++cnt;
      } else {
        break;
      }
    } else {
      if (cnt+4 < buflen) { /* keep one byte for terminating zero */
        *result++ = '\\';
        *result++ = 'x';
        *result++ = hex[(*data & 0xf0) >> 4];
        *result++ = hex[*data & 0x0f];
        cnt += 4;
      } else
        break;
    }

    ++data; --len;
  }

  *result = '\0'; /* add a terminating zero */
  return cnt;
}

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

size_t
coap_print_addr(const struct coap_address_t *addr, unsigned char *buf, size_t len) {
#if defined( HAVE_ARPA_INET_H ) || defined( HAVE_WS2TCPIP_H )
  const void *addrptr = NULL;
  in_port_t port;
  unsigned char *p = buf;
  size_t need_buf;

  switch (addr->addr.sa.sa_family) {
  case AF_INET:
    addrptr = &addr->addr.sin.sin_addr;
    port = ntohs(addr->addr.sin.sin_port);
    need_buf = INET_ADDRSTRLEN;
    break;
  case AF_INET6:
    if (len < 7) /* do not proceed if buffer is even too short for [::]:0 */
      return 0;

    *p++ = '[';

    addrptr = &addr->addr.sin6.sin6_addr;
    port = ntohs(addr->addr.sin6.sin6_port);
    need_buf = INET6_ADDRSTRLEN;

    break;
  default:
    memcpy(buf, "(unknown address type)", min(22, len));
    return min(22, len);
  }

  /* Cast needed for Windows, since it doesn't have the correct API signature. */
  if (inet_ntop(addr->addr.sa.sa_family, addrptr, (char *)p,
                min(len, need_buf)) == 0) {
    perror("coap_print_addr");
    return 0;
  }

  p += strnlen((char *)p, len);

  if (addr->addr.sa.sa_family == AF_INET6) {
    if (p < buf + len) {
      *p++ = ']';
    } else
      return 0;
  }

  p += snprintf((char *)p, buf + len - p + 1, ":%d", port);

  return buf + len - p;
#else /* HAVE_ARPA_INET_H */
# if WITH_CONTIKI
  unsigned char *p = buf;
  uint8_t i;
#  if NETSTACK_CONF_WITH_IPV6
  const uint8_t hex[] = "0123456789ABCDEF";

  if (len < 41)
    return 0;

  *p++ = '[';

  for (i=0; i < 16; i += 2) {
    if (i) {
      *p++ = ':';
    }
    *p++ = hex[(addr->addr.u8[i] & 0xf0) >> 4];
    *p++ = hex[(addr->addr.u8[i] & 0x0f)];
    *p++ = hex[(addr->addr.u8[i+1] & 0xf0) >> 4];
    *p++ = hex[(addr->addr.u8[i+1] & 0x0f)];
  }
  *p++ = ']';
#  else /* WITH_UIP6 */
#   warning "IPv4 network addresses will not be included in debug output"

  if (len < 21)
    return 0;
#  endif /* WITH_UIP6 */
  if (buf + len - p < 6)
    return 0;

#ifdef HAVE_SNPRINTF
  p += snprintf((char *)p, buf + len - p + 1, ":%d", uip_htons(addr->port));
#else /* HAVE_SNPRINTF */
  /* @todo manual conversion of port number */
#endif /* HAVE_SNPRINTF */

  return p - buf;
# else /* WITH_CONTIKI */
  /* TODO: output addresses manually */
#   warning "inet_ntop() not available, network addresses will not be included in debug output"
# endif /* WITH_CONTIKI */
  return 0;
#endif
}

#ifdef WITH_CONTIKI
# define fprintf(fd, ...) PRINTF(__VA_ARGS__)
# define fflush(...)

# ifdef HAVE_VPRINTF
#  define vfprintf(fd, ...) vprintf(__VA_ARGS__)
# else /* HAVE_VPRINTF */
#  define vfprintf(fd, ...) PRINTF(__VA_ARGS__)
# endif /* HAVE_VPRINTF */
#endif /* WITH_CONTIKI */

/** Returns a textual description of the message type @p t. */
static const char *
msg_type_string(uint16_t t) {
  static const char *types[] = { "CON", "NON", "ACK", "RST", "???" };

  return types[min(t, sizeof(types)/sizeof(char *) - 1)];
}

/** Returns a textual description of the method or response code. */
static const char *
msg_code_string(uint16_t c) {
  static const char *methods[] = { "0.00", "GET", "POST", "PUT", "DELETE",
                                   "FETCH", "PATCH", "iPATCH" };
  static const char *signals[] = { "7.00", "CSM", "Ping", "Pong", "Release",
                                   "Abort" };
  static char buf[5];

  if (c < sizeof(methods)/sizeof(const char *)) {
    return methods[c];
  } else if (c >= 224 && c - 224 < (int)(sizeof(signals)/sizeof(const char *))) {
    return signals[c-224];
  } else {
    snprintf(buf, sizeof(buf), "%u.%02u", (c >> 5) & 0x7, c & 0x1f);
    return buf;
  }
}

/** Returns a textual description of the option name. */
static const char *
msg_option_string(uint8_t code, uint16_t option_type) {
  struct option_desc_t {
    uint16_t type;
    const char *name;
  };

  static struct option_desc_t options[] = {
    { COAP_OPTION_IF_MATCH, "If-Match" },
    { COAP_OPTION_URI_HOST, "Uri-Host" },
    { COAP_OPTION_ETAG, "ETag" },
    { COAP_OPTION_IF_NONE_MATCH, "If-None-Match" },
    { COAP_OPTION_OBSERVE, "Observe" },
    { COAP_OPTION_URI_PORT, "Uri-Port" },
    { COAP_OPTION_LOCATION_PATH, "Location-Path" },
    { COAP_OPTION_URI_PATH, "Uri-Path" },
    { COAP_OPTION_CONTENT_FORMAT, "Content-Format" },
    { COAP_OPTION_MAXAGE, "Max-Age" },
    { COAP_OPTION_URI_QUERY, "Uri-Query" },
    { COAP_OPTION_ACCEPT, "Accept" },
    { COAP_OPTION_LOCATION_QUERY, "Location-Query" },
    { COAP_OPTION_BLOCK2, "Block2" },
    { COAP_OPTION_BLOCK1, "Block1" },
    { COAP_OPTION_PROXY_URI, "Proxy-Uri" },
    { COAP_OPTION_PROXY_SCHEME, "Proxy-Scheme" },
    { COAP_OPTION_SIZE1, "Size1" },
    { COAP_OPTION_SIZE2, "Size2" },
    { COAP_OPTION_NORESPONSE, "No-Response" }
  };

  static struct option_desc_t options_csm[] = {
    { COAP_SIGNALING_OPTION_MAX_MESSAGE_SIZE, "Max-Message-Size" },
    { COAP_SIGNALING_OPTION_BLOCK_WISE_TRANSFER, "Block-wise-Transfer" }
  };

  static struct option_desc_t options_pingpong[] = {
    { COAP_SIGNALING_OPTION_CUSTODY, "Custody" }
  };

  static struct option_desc_t options_release[] = {
    { COAP_SIGNALING_OPTION_ALTERNATIVE_ADDRESS, "Alternative-Address" },
    { COAP_SIGNALING_OPTION_HOLD_OFF, "Hold-Off" }
  };

  static struct option_desc_t options_abort[] = {
    { COAP_SIGNALING_OPTION_BAD_CSM_OPTION, "Bad-CSM-Option" }
  };

  static char buf[6];
  size_t i;

  if (code == COAP_SIGNALING_CSM) {
    for (i = 0; i < sizeof(options_csm)/sizeof(struct option_desc_t); i++) {
      if (option_type == options_csm[i].type) {
        return options_csm[i].name;
      }
    }
  } else if (code == COAP_SIGNALING_PING || code == COAP_SIGNALING_PONG) {
    for (i = 0; i < sizeof(options_pingpong)/sizeof(struct option_desc_t); i++) {
      if (option_type == options_pingpong[i].type) {
        return options_pingpong[i].name;
      }
    }
  } else if (code == COAP_SIGNALING_RELEASE) {
    for (i = 0; i < sizeof(options_release)/sizeof(struct option_desc_t); i++) {
      if (option_type == options_release[i].type) {
        return options_release[i].name;
      }
    }
  } else if (code == COAP_SIGNALING_ABORT) {
    for (i = 0; i < sizeof(options_abort)/sizeof(struct option_desc_t); i++) {
      if (option_type == options_abort[i].type) {
        return options_abort[i].name;
      }
    }
  } else {
    /* search option_type in list of known options */
    for (i = 0; i < sizeof(options)/sizeof(struct option_desc_t); i++) {
      if (option_type == options[i].type) {
        return options[i].name;
      }
    }
  }
  /* unknown option type, just print to buf */
  snprintf(buf, sizeof(buf), "%u", option_type);
  return buf;
}

static unsigned int
print_content_format(unsigned int format_type,
                     unsigned char *result, unsigned int buflen) {
  struct desc_t {
    unsigned int type;
    const char *name;
  };

  static struct desc_t formats[] = {
    { COAP_MEDIATYPE_TEXT_PLAIN, "text/plain" },
    { COAP_MEDIATYPE_APPLICATION_LINK_FORMAT, "application/link-format" },
    { COAP_MEDIATYPE_APPLICATION_XML, "application/xml" },
    { COAP_MEDIATYPE_APPLICATION_OCTET_STREAM, "application/octet-stream" },
    { COAP_MEDIATYPE_APPLICATION_EXI, "application/exi" },
    { COAP_MEDIATYPE_APPLICATION_JSON, "application/json" },
    { COAP_MEDIATYPE_APPLICATION_CBOR, "application/cbor" },
    { COAP_MEDIATYPE_APPLICATION_COSE_SIGN, "application/cose; cose-type=\"cose-sign\"" },
    { COAP_MEDIATYPE_APPLICATION_COSE_SIGN1, "application/cose; cose-type=\"cose-sign1\"" },
    { COAP_MEDIATYPE_APPLICATION_COSE_ENCRYPT, "application/cose; cose-type=\"cose-encrypt\"" },
    { COAP_MEDIATYPE_APPLICATION_COSE_ENCRYPT0, "application/cose; cose-type=\"cose-encrypt0\"" },
    { COAP_MEDIATYPE_APPLICATION_COSE_MAC, "application/cose; cose-type=\"cose-mac\"" },
    { COAP_MEDIATYPE_APPLICATION_COSE_MAC0, "application/cose; cose-type=\"cose-mac0\"" },
    { COAP_MEDIATYPE_APPLICATION_COSE_KEY, "application/cose-key" },
    { COAP_MEDIATYPE_APPLICATION_COSE_KEY_SET, "application/cose-key-set" },
    { COAP_MEDIATYPE_APPLICATION_SENML_JSON, "application/senml+json" },
    { COAP_MEDIATYPE_APPLICATION_SENSML_JSON, "application/sensml+json" },
    { COAP_MEDIATYPE_APPLICATION_SENML_CBOR, "application/senml+cbor" },
    { COAP_MEDIATYPE_APPLICATION_SENSML_CBOR, "application/sensml+cbor" },
    { COAP_MEDIATYPE_APPLICATION_SENML_EXI, "application/senml-exi" },
    { COAP_MEDIATYPE_APPLICATION_SENSML_EXI, "application/sensml-exi" },
    { COAP_MEDIATYPE_APPLICATION_SENML_XML, "application/senml+xml" },
    { COAP_MEDIATYPE_APPLICATION_SENSML_XML, "application/sensml+xml" },
    { 75, "application/dcaf+cbor" }
  };

  size_t i;

  /* search format_type in list of known content formats */
  for (i = 0; i < sizeof(formats)/sizeof(struct desc_t); i++) {
    if (format_type == formats[i].type) {
      return snprintf((char *)result, buflen, "%s", formats[i].name);
    }
  }

  /* unknown content format, just print numeric value to buf */
  return snprintf((char *)result, buflen, "%d", format_type);
}

/**
 * Returns 1 if the given @p content_format is either unknown or known
 * to carry binary data. The return value @c 0 hence indicates
 * printable data which is also assumed if @p content_format is @c 01.
 */
COAP_STATIC_INLINE int
is_binary(int content_format) {
  return !(content_format == -1 ||
           content_format == COAP_MEDIATYPE_TEXT_PLAIN ||
           content_format == COAP_MEDIATYPE_APPLICATION_LINK_FORMAT ||
           content_format == COAP_MEDIATYPE_APPLICATION_XML ||
           content_format == COAP_MEDIATYPE_APPLICATION_JSON);
}

#define COAP_DO_SHOW_OUTPUT_LINE           \
 do {                                      \
   if (use_fprintf_for_show_pdu) {         \
     fprintf(COAP_DEBUG_FD, "%s", outbuf); \
   }                                       \
   else {                                  \
     coap_log(level, "%s", outbuf);        \
   }                                       \
 } while (0)

void
coap_show_pdu(coap_log_t level, const coap_pdu_t *pdu) {
#if COAP_CONSTRAINED_STACK
  static coap_mutex_t static_show_pdu_mutex = COAP_MUTEX_INITIALIZER;
  static unsigned char buf[1024]; /* need some space for output creation */
  static char outbuf[COAP_DEBUG_BUF_SIZE];
#else /* ! COAP_CONSTRAINED_STACK */
  unsigned char buf[1024]; /* need some space for output creation */
  char outbuf[COAP_DEBUG_BUF_SIZE];
#endif /* ! COAP_CONSTRAINED_STACK */
  size_t buf_len = 0; /* takes the number of bytes written to buf */
  int encode = 0, have_options = 0, i;
  coap_opt_iterator_t opt_iter;
  coap_opt_t *option;
  int content_format = -1;
  size_t data_len;
  unsigned char *data;
  int outbuflen = 0;

  /* Save time if not needed */
  if (level > coap_get_log_level())
    return;

#if COAP_CONSTRAINED_STACK
  coap_mutex_lock(&static_show_pdu_mutex);
#endif /* COAP_CONSTRAINED_STACK */

  snprintf(outbuf, sizeof(outbuf), "v:%d t:%s c:%s i:%04x {",
          COAP_DEFAULT_VERSION, msg_type_string(pdu->type),
          msg_code_string(pdu->code), pdu->tid);

  for (i = 0; i < pdu->token_length; i++) {
    outbuflen = strlen(outbuf);
    snprintf(&outbuf[outbuflen], sizeof(outbuf)-outbuflen,
              "%02x", pdu->token[i]);
  }
  outbuflen = strlen(outbuf);
  snprintf(&outbuf[outbuflen], sizeof(outbuf)-outbuflen,  "}");

  /* show options, if any */
  coap_option_iterator_init(pdu, &opt_iter, COAP_OPT_ALL);

  outbuflen = strlen(outbuf);
  snprintf(&outbuf[outbuflen], sizeof(outbuf)-outbuflen,  " [");
  while ((option = coap_option_next(&opt_iter))) {
    if (!have_options) {
      have_options = 1;
    } else {
      outbuflen = strlen(outbuf);
      snprintf(&outbuf[outbuflen], sizeof(outbuf)-outbuflen,  ",");
    }

    if (pdu->code == COAP_SIGNALING_CSM) switch(opt_iter.type) {
    case COAP_SIGNALING_OPTION_MAX_MESSAGE_SIZE:
      buf_len = snprintf((char *)buf, sizeof(buf), "%u",
                         coap_decode_var_bytes(coap_opt_value(option),
                                               coap_opt_length(option)));
      break;
    default:
      buf_len = 0;
      break;
    } else if (pdu->code == COAP_SIGNALING_PING
            || pdu->code == COAP_SIGNALING_PONG) {
      buf_len = 0;
    } else if (pdu->code == COAP_SIGNALING_RELEASE) switch(opt_iter.type) {
    case COAP_SIGNALING_OPTION_ALTERNATIVE_ADDRESS:
      buf_len = print_readable(coap_opt_value(option),
                               coap_opt_length(option),
                               buf, sizeof(buf), 0);
      break;
    case COAP_SIGNALING_OPTION_HOLD_OFF:
      buf_len = snprintf((char *)buf, sizeof(buf), "%u",
                         coap_decode_var_bytes(coap_opt_value(option),
                                               coap_opt_length(option)));
      break;
    default:
      buf_len = 0;
      break;
    } else if (pdu->code == COAP_SIGNALING_ABORT) switch(opt_iter.type) {
    case COAP_SIGNALING_OPTION_BAD_CSM_OPTION:
      buf_len = snprintf((char *)buf, sizeof(buf), "%u",
                         coap_decode_var_bytes(coap_opt_value(option),
                                               coap_opt_length(option)));
      break;
    default:
      buf_len = 0;
      break;
    } else switch (opt_iter.type) {
    case COAP_OPTION_CONTENT_FORMAT:
      content_format = (int)coap_decode_var_bytes(coap_opt_value(option),
                                                  coap_opt_length(option));

      buf_len = print_content_format(content_format, buf, sizeof(buf));
      break;

    case COAP_OPTION_BLOCK1:
    case COAP_OPTION_BLOCK2:
      /* split block option into number/more/size where more is the
       * letter M if set, the _ otherwise */
      buf_len = snprintf((char *)buf, sizeof(buf), "%u/%c/%u",
                         coap_opt_block_num(option), /* block number */
                         COAP_OPT_BLOCK_MORE(option) ? 'M' : '_', /* M bit */
                         (1 << (COAP_OPT_BLOCK_SZX(option) + 4))); /* block size */

      break;

    case COAP_OPTION_URI_PORT:
    case COAP_OPTION_MAXAGE:
    case COAP_OPTION_OBSERVE:
    case COAP_OPTION_SIZE1:
    case COAP_OPTION_SIZE2:
      /* show values as unsigned decimal value */
      buf_len = snprintf((char *)buf, sizeof(buf), "%u",
                         coap_decode_var_bytes(coap_opt_value(option),
                                               coap_opt_length(option)));
      break;

    default:
      /* generic output function for all other option types */
      if (opt_iter.type == COAP_OPTION_URI_PATH ||
          opt_iter.type == COAP_OPTION_PROXY_URI ||
          opt_iter.type == COAP_OPTION_URI_HOST ||
          opt_iter.type == COAP_OPTION_LOCATION_PATH ||
          opt_iter.type == COAP_OPTION_LOCATION_QUERY ||
          opt_iter.type == COAP_OPTION_URI_QUERY) {
        encode = 0;
      } else {
        encode = 1;
      }

      buf_len = print_readable(coap_opt_value(option),
                               coap_opt_length(option),
                               buf, sizeof(buf), encode);
    }

    outbuflen = strlen(outbuf);
    snprintf(&outbuf[outbuflen], sizeof(outbuf)-outbuflen,
              " %s:%.*s", msg_option_string(pdu->code, opt_iter.type),
              (int)buf_len, buf);
  }

  outbuflen = strlen(outbuf);
  snprintf(&outbuf[outbuflen], sizeof(outbuf)-outbuflen,  " ]");

  if (coap_get_data(pdu, &data_len, &data)) {

    outbuflen = strlen(outbuf);
    snprintf(&outbuf[outbuflen], sizeof(outbuf)-outbuflen,  " :: ");

    if (is_binary(content_format)) {
      int keep_data_len = data_len;
      uint8_t *keep_data = data;

      outbuflen = strlen(outbuf);
      snprintf(&outbuf[outbuflen], sizeof(outbuf)-outbuflen,
               "binary data length %zu\n", data_len);
      COAP_DO_SHOW_OUTPUT_LINE;
      /*
       * Output hex dump of binary data as a continuous entry
       */
      outbuf[0] = '\000';
      snprintf(outbuf, sizeof(outbuf),  "<<");
      while (data_len--) {
        outbuflen = strlen(outbuf);
        snprintf(&outbuf[outbuflen], sizeof(outbuf)-outbuflen,
                 "%02x", *data++);
      }
      outbuflen = strlen(outbuf);
      snprintf(&outbuf[outbuflen], sizeof(outbuf)-outbuflen,  ">>");
      data_len = keep_data_len;
      data = keep_data;
      outbuflen = strlen(outbuf);
      snprintf(&outbuf[outbuflen], sizeof(outbuf)-outbuflen,  "\n");
      COAP_DO_SHOW_OUTPUT_LINE;
      /*
       * Output ascii readable (if possible), immediately under the
       * hex value of the character output above to help binary debugging
       */
      outbuf[0] = '\000';
      snprintf(outbuf, sizeof(outbuf),  "<<");
      while (data_len--) {
        outbuflen = strlen(outbuf);
        snprintf(&outbuf[outbuflen], sizeof(outbuf)-outbuflen,
                 "%c ", isprint (*data) ? *data : '.');
        data++;
      }
      outbuflen = strlen(outbuf);
      snprintf(&outbuf[outbuflen], sizeof(outbuf)-outbuflen,  ">>");
    } else {
      if (print_readable(data, data_len, buf, sizeof(buf), 0)) {
        outbuflen = strlen(outbuf);
        snprintf(&outbuf[outbuflen], sizeof(outbuf)-outbuflen,  "'%s'", buf);
      }
    }
  }

  outbuflen = strlen(outbuf);
  snprintf(&outbuf[outbuflen], sizeof(outbuf)-outbuflen,  "\n");
  COAP_DO_SHOW_OUTPUT_LINE;

#if COAP_CONSTRAINED_STACK
  coap_mutex_unlock(&static_show_pdu_mutex);
#endif /* COAP_CONSTRAINED_STACK */
}

void coap_show_tls_version(coap_log_t level)
{
  char buffer[64];
  coap_string_tls_version(buffer, sizeof(buffer));
  coap_log(level, "%s\n", buffer);
}

char *coap_string_tls_version(char *buffer, size_t bufsize)
{
  coap_tls_version_t *tls_version = coap_get_tls_library_version();
  char beta[8];
  char sub[2];
  char b_beta[8];
  char b_sub[2];

  switch (tls_version->type) {
  case COAP_TLS_LIBRARY_NOTLS:
    snprintf(buffer, bufsize, "TLS Library: None");
    break;
  case COAP_TLS_LIBRARY_TINYDTLS:
    snprintf(buffer, bufsize, "TLS Library: TinyDTLS - runtime %lu.%lu.%lu, "
             "libcoap built for %lu.%lu.%lu",
             (unsigned long)(tls_version->version >> 16),
             (unsigned long)((tls_version->version >> 8) & 0xff),
             (unsigned long)(tls_version->version & 0xff),
             (unsigned long)(tls_version->built_version >> 16),
             (unsigned long)((tls_version->built_version >> 8) & 0xff),
             (unsigned long)(tls_version->built_version & 0xff));
    break;
  case COAP_TLS_LIBRARY_OPENSSL:
    switch (tls_version->version &0xf) {
    case 0:
      strcpy(beta, "-dev");
      break;
    case 0xf:
      strcpy(beta, "");
      break;
    default:
      strcpy(beta, "-beta");
      beta[5] = (tls_version->version &0xf) + '0';
      beta[6] = '\000';
      break;
    }
    sub[0] = ((tls_version->version >> 4) & 0xff) ?
                    ((tls_version->version >> 4) & 0xff) + 'a' -1 : '\000';
    sub[1] = '\000';
    switch (tls_version->built_version &0xf) {
    case 0:
      strcpy(b_beta, "-dev");
      break;
    case 0xf:
      strcpy(b_beta, "");
      break;
    default:
      strcpy(b_beta, "-beta");
      b_beta[5] = (tls_version->built_version &0xf) + '0';
      b_beta[6] = '\000';
      break;
    }
    b_sub[0] = ((tls_version->built_version >> 4) & 0xff) ?
               ((tls_version->built_version >> 4) & 0xff) + 'a' -1 : '\000';
    b_sub[1] = '\000';
    snprintf(buffer, bufsize, "TLS Library: OpenSSL - runtime "
             "%lu.%lu.%lu%s%s, libcoap built for %lu.%lu.%lu%s%s",
             (unsigned long)(tls_version->version >> 28),
             (unsigned long)((tls_version->version >> 20) & 0xff),
             (unsigned long)((tls_version->version >> 12) & 0xff), sub, beta,
             (unsigned long)(tls_version->built_version >> 28),
             (unsigned long)((tls_version->built_version >> 20) & 0xff),
             (unsigned long)((tls_version->built_version >> 12) & 0xff),
             b_sub, b_beta);
    break;
  case COAP_TLS_LIBRARY_GNUTLS:
    snprintf(buffer, bufsize, "TLS Library: GnuTLS - runtime %lu.%lu.%lu, "
             "libcoap built for %lu.%lu.%lu",
             (unsigned long)(tls_version->version >> 16),
             (unsigned long)((tls_version->version >> 8) & 0xff),
             (unsigned long)(tls_version->version & 0xff),
             (unsigned long)(tls_version->built_version >> 16),
             (unsigned long)((tls_version->built_version >> 8) & 0xff),
             (unsigned long)(tls_version->built_version & 0xff));
    break;
  case COAP_TLS_LIBRARY_MBEDTLS:
    snprintf(buffer, bufsize, "TLS Library: MbedTLS - runtime %lu.%lu.%lu, "
             "libcoap built for %lu.%lu.%lu",
             (unsigned long)(tls_version->version >> 24),
             (unsigned long)((tls_version->version >> 16) & 0xff),
             (unsigned long)((tls_version->version >> 8) & 0xff),
             (unsigned long)(tls_version->built_version >> 24),
             (unsigned long)((tls_version->built_version >> 16) & 0xff),
             (unsigned long)((tls_version->built_version >> 8) & 0xff));
    break;
  default:
    snprintf(buffer, bufsize, "Library type %d unknown", tls_version->type);
    break;
  }
  return buffer;
}

static coap_log_handler_t log_handler = NULL;

void coap_set_log_handler(coap_log_handler_t handler) {
  log_handler = handler;
}

void
coap_log_impl(coap_log_t level, const char *format, ...) {

  if (maxlog < level)
    return;

  if (log_handler) {
#if COAP_CONSTRAINED_STACK
    static coap_mutex_t static_log_mutex = COAP_MUTEX_INITIALIZER;
    static char message[COAP_DEBUG_BUF_SIZE];
#else /* ! COAP_CONSTRAINED_STACK */
    char message[COAP_DEBUG_BUF_SIZE];
#endif /* ! COAP_CONSTRAINED_STACK */
    va_list ap;
    va_start(ap, format);
#if COAP_CONSTRAINED_STACK
  coap_mutex_lock(&static_log_mutex);
#endif /* COAP_CONSTRAINED_STACK */

    vsnprintf( message, sizeof(message), format, ap);
    va_end(ap);
    log_handler(level, message);
#if COAP_CONSTRAINED_STACK
    coap_mutex_unlock(&static_log_mutex);
#endif /* COAP_CONSTRAINED_STACK */
  } else {
    char timebuf[32];
    coap_tick_t now;
    va_list ap;
    FILE *log_fd;

    log_fd = level <= LOG_CRIT ? COAP_ERR_FD : COAP_DEBUG_FD;

    coap_ticks(&now);
    if (print_timestamp(timebuf,sizeof(timebuf), now))
      fprintf(log_fd, "%s ", timebuf);

    if (level <= LOG_DEBUG)
      fprintf(log_fd, "%s ", loglevels[level]);

    va_start(ap, format);
    vfprintf(log_fd, format, ap);
    va_end(ap);
    fflush(log_fd);
  }
}

static struct packet_num_interval {
  int start;
  int end;
} packet_loss_intervals[10];
static int num_packet_loss_intervals = 0;
static int packet_loss_level = 0;
static int send_packet_count = 0;

int coap_debug_set_packet_loss(const char *loss_level) {
  const char *p = loss_level;
  char *end = NULL;
  int n = (int)strtol(p, &end, 10), i = 0;
  if (end == p || n < 0)
    return 0;
  if (*end == '%') {
    if (n > 100)
      n = 100;
    packet_loss_level = n * 65536 / 100;
    coap_log(LOG_DEBUG, "packet loss level set to %d%%\n", n);
  } else {
    if (n <= 0)
      return 0;
    while (i < 10) {
      packet_loss_intervals[i].start = n;
      if (*end == '-') {
        p = end + 1;
        n = (int)strtol(p, &end, 10);
        if (end == p || n <= 0)
          return 0;
      }
      packet_loss_intervals[i++].end = n;
      if (*end == 0)
        break;
      if (*end != ',')
        return 0;
      p = end + 1;
      n = (int)strtol(p, &end, 10);
      if (end == p || n <= 0)
        return 0;
    }
    if (i == 10)
      return 0;
    num_packet_loss_intervals = i;
  }
  send_packet_count = 0;
  return 1;
}

int coap_debug_send_packet(void) {
  ++send_packet_count;
  if (num_packet_loss_intervals > 0) {
    int i;
    for (i = 0; i < num_packet_loss_intervals; i++) {
      if (send_packet_count >= packet_loss_intervals[i].start
        && send_packet_count <= packet_loss_intervals[i].end)
        return 0;
    }
  }
  if ( packet_loss_level > 0 ) {
    uint16_t r = 0;
    prng( (uint8_t*)&r, 2 );
    if ( r < packet_loss_level )
      return 0;
  }
  return 1;
}
