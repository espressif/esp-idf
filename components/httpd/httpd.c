/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
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
 *         Simon Goldschmidt
 *
 */

/* This httpd supports for a
 * rudimentary server-side-include facility which will replace tags of the form
 * <!--#tag--> in any file whose extension is .shtml, .shtm or .ssi with
 * strings provided by an include handler whose pointer is provided to the
 * module via function http_set_ssi_handler().
 * Additionally, a simple common
 * gateway interface (CGI) handling mechanism has been added to allow clients
 * to hook functions to particular request URIs.
 *
 * To enable SSI support, define label LWIP_HTTPD_SSI in lwipopts.h.
 * To enable CGI support, define label LWIP_HTTPD_CGI in lwipopts.h.
 *
 * By default, the server assumes that HTTP headers are already present in
 * each file stored in the file system.  By defining LWIP_HTTPD_DYNAMIC_HEADERS in
 * lwipopts.h, this behavior can be changed such that the server inserts the
 * headers automatically based on the extension of the file being served.  If
 * this mode is used, be careful to ensure that the file system image used
 * does not already contain the header information.
 *
 * File system images without headers can be created using the makefsfile
 * tool with the -h command line option.
 *
 *
 * Notes about valid SSI tags
 * --------------------------
 *
 * The following assumptions are made about tags used in SSI markers:
 *
 * 1. No tag may contain '-' or whitespace characters within the tag name.
 * 2. Whitespace is allowed between the tag leadin "<!--#" and the start of
 *    the tag name and between the tag name and the leadout string "-->".
 * 3. The maximum tag name length is LWIP_HTTPD_MAX_TAG_NAME_LEN, currently 8 characters.
 *
 * Notes on CGI usage
 * ------------------
 *
 * The simple CGI support offered here works with GET method requests only
 * and can handle up to 16 parameters encoded into the URI. The handler
 * function may not write directly to the HTTP output but must return a
 * filename that the HTTP server will send to the browser as a response to
 * the incoming CGI request.
 *
 *
 *
 * The list of supported file types is quite short, so if makefsdata complains
 * about an unknown extension, make sure to add it (and its doctype) to
 * the 'g_psHTTPHeaders' list.
 */

// Adapted for ESP32 by M.J.A.Hamel 2017
//  - added g_fs_root symbol to make compatible with ESP32 library
//  - added HTTPD_DIRECT_HOOK option to support redirects
//  - added http_set_post_handlers so POST handling can be redirected
//  - added httpd_stop so the server can be halted

#include "httpd.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "httpd_structs.h"
#include "lwip/tcp.h"
#include "fs.h"

#include <string.h>
#include <stdlib.h>

#if LWIP_TCP

#ifndef HTTPD_DEBUG
#define HTTPD_DEBUG         LWIP_DBG_OFF
#endif

/** Set this to 1 and add the next line to lwippools.h to use a memp pool
 * for allocating struct http_state instead of the heap:
 *
 * LWIP_MEMPOOL(HTTPD_STATE, 20, 100, "HTTPD_STATE")
 */
#ifndef HTTPD_USE_MEM_POOL
#define HTTPD_USE_MEM_POOL  0
#endif

/** The server port for HTTPD to use */
#ifndef HTTPD_SERVER_PORT
#define HTTPD_SERVER_PORT                   80
#endif

/** Maximum retries before the connection is aborted/closed.
 * - number of times pcb->poll is called -> default is 4*500ms = 2s;
 * - reset when pcb->sent is called
 */
#ifndef HTTPD_MAX_RETRIES
#define HTTPD_MAX_RETRIES                   4
#endif

/** The poll delay is X*500ms */
#ifndef HTTPD_POLL_INTERVAL
#define HTTPD_POLL_INTERVAL                 4
#endif

/** Priority for tcp pcbs created by HTTPD (very low by default).
 *  Lower priorities get killed first when running out of memroy.
 */
#ifndef HTTPD_TCP_PRIO
#define HTTPD_TCP_PRIO                      TCP_PRIO_MIN
#endif

/** Set this to 1 to enabled timing each file sent */
#ifndef LWIP_HTTPD_TIMING
#define LWIP_HTTPD_TIMING                   0
#endif
#ifndef HTTPD_DEBUG_TIMING
#define HTTPD_DEBUG_TIMING                  LWIP_DBG_OFF
#endif

/** Set this to 1 on platforms where strnstr is not available */
#ifndef LWIP_HTTPD_STRNSTR_PRIVATE
#define LWIP_HTTPD_STRNSTR_PRIVATE          1
#endif

/** Set this to one to show error pages when parsing a request fails instead
    of simply closing the connection. */
#ifndef LWIP_HTTPD_SUPPORT_EXTSTATUS
#define LWIP_HTTPD_SUPPORT_EXTSTATUS        0
#endif

/** Set this to 0 to drop support for HTTP/0.9 clients (to save some bytes) */
#ifndef LWIP_HTTPD_SUPPORT_V09
#define LWIP_HTTPD_SUPPORT_V09              1
#endif

/** Set this to 1 to enable HTTP/1.1 persistent connections.
 * ATTENTION: If the generated file system includes HTTP headers, these must
 * include the "Connection: keep-alive" header (pass argument "-11" to makefsdata).
 */
#ifndef LWIP_HTTPD_SUPPORT_11_KEEPALIVE
#define LWIP_HTTPD_SUPPORT_11_KEEPALIVE     0
#endif

/** Set this to 1 to support HTTP request coming in in multiple packets/pbufs */
#ifndef LWIP_HTTPD_SUPPORT_REQUESTLIST
#define LWIP_HTTPD_SUPPORT_REQUESTLIST      1
#endif

#if LWIP_HTTPD_SUPPORT_REQUESTLIST
/** Number of rx pbufs to enqueue to parse an incoming request (up to the first
    newline) */
#ifndef LWIP_HTTPD_REQ_QUEUELEN
#define LWIP_HTTPD_REQ_QUEUELEN             5
#endif

/** Number of (TCP payload-) bytes (in pbufs) to enqueue to parse and incoming
    request (up to the first double-newline) */
#ifndef LWIP_HTTPD_REQ_BUFSIZE
#define LWIP_HTTPD_REQ_BUFSIZE              LWIP_HTTPD_MAX_REQ_LENGTH
#endif

/** Defines the maximum length of a HTTP request line (up to the first CRLF,
    copied from pbuf into this a global buffer when pbuf- or packet-queues
    are received - otherwise the input pbuf is used directly) */
#ifndef LWIP_HTTPD_MAX_REQ_LENGTH
#define LWIP_HTTPD_MAX_REQ_LENGTH           LWIP_MIN(1023, (LWIP_HTTPD_REQ_QUEUELEN * PBUF_POOL_BUFSIZE))
#endif
#endif /* LWIP_HTTPD_SUPPORT_REQUESTLIST */

/** Maximum length of the filename to send as response to a POST request,
 * filled in by the application when a POST is finished.
 */
#ifndef LWIP_HTTPD_POST_MAX_RESPONSE_URI_LEN
#define LWIP_HTTPD_POST_MAX_RESPONSE_URI_LEN 63
#endif

/** Set this to 0 to not send the SSI tag (default is on, so the tag will
 * be sent in the HTML page */
#ifndef LWIP_HTTPD_SSI_INCLUDE_TAG
#define LWIP_HTTPD_SSI_INCLUDE_TAG           1
#endif

/** Set this to 1 to call tcp_abort when tcp_close fails with memory error.
 * This can be used to prevent consuming all memory in situations where the
 * HTTP server has low priority compared to other communication. */
#ifndef LWIP_HTTPD_ABORT_ON_CLOSE_MEM_ERROR
#define LWIP_HTTPD_ABORT_ON_CLOSE_MEM_ERROR  0
#endif

/** Set this to 1 to kill the oldest connection when running out of
 * memory for 'struct http_state' or 'struct http_ssi_state'.
 * ATTENTION: This puts all connections on a linked list, so may be kind of slow.
 */
#ifndef LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
#define LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED 0
#endif

/** Minimum length for a valid HTTP/0.9 request: "GET /\r\n" -> 7 bytes */
#define MIN_REQ_LEN   7

#define CRLF "\r\n"
#define HTTP11_CONNECTIONKEEPALIVE "Connection: keep-alive"

#if LWIP_HTTPD_SSI
#define LWIP_HTTPD_IS_SSI(hs) ((hs)->ssi)
#else /* LWIP_HTTPD_SSI */
#define LWIP_HTTPD_IS_SSI(hs) 0
#endif /* LWIP_HTTPD_SSI */

/** These defines check whether tcp_write has to copy data or not */

/** This was TI's check whether to let TCP copy data or not
#define HTTP_IS_DATA_VOLATILE(hs) ((hs->file < (char *)0x20000000) ? 0 : TCP_WRITE_FLAG_COPY)*/
#ifndef HTTP_IS_DATA_VOLATILE
#if LWIP_HTTPD_SSI
/* Copy for SSI files, no copy for non-SSI files */
#define HTTP_IS_DATA_VOLATILE(hs)   ((hs)->ssi ? TCP_WRITE_FLAG_COPY : 0)
#else /* LWIP_HTTPD_SSI */
/** Default: don't copy if the data is sent from file-system directly */
#define HTTP_IS_DATA_VOLATILE(hs) (((hs->file != NULL) && (hs->handle != NULL) && (hs->file == \
                                   (char*)hs->handle->data + hs->handle->len - hs->left)) \
                                   ? 0 : TCP_WRITE_FLAG_COPY)
#endif /* LWIP_HTTPD_SSI */
#endif

/** Default: headers are sent from ROM */
#ifndef HTTP_IS_HDR_VOLATILE
#define HTTP_IS_HDR_VOLATILE(hs, ptr) 0
#endif

#if LWIP_HTTPD_SSI
/** Default: Tags are sent from struct http_state and are therefore volatile */
#ifndef HTTP_IS_TAG_VOLATILE
#define HTTP_IS_TAG_VOLATILE(ptr) TCP_WRITE_FLAG_COPY
#endif
#endif /* LWIP_HTTPD_SSI */

/* Return values for http_send_*() */
#define HTTP_DATA_TO_SEND_BREAK    2
#define HTTP_DATA_TO_SEND_CONTINUE 1
#define HTTP_NO_DATA_TO_SEND       0

#if HTTPD_USE_MEM_POOL
#define HTTP_ALLOC_SSI_STATE()  (struct http_ssi_state *)memp_malloc(MEMP_HTTPD_SSI_STATE)
#define HTTP_ALLOC_HTTP_STATE() (struct http_state *)memp_malloc(MEMP_HTTPD_STATE)
#else /* HTTPD_USE_MEM_POOL */
#define HTTP_ALLOC_SSI_STATE()  (struct http_ssi_state *)mem_malloc(sizeof(struct http_ssi_state))
#define HTTP_ALLOC_HTTP_STATE() (struct http_state *)mem_malloc(sizeof(struct http_state))
#endif /* HTTPD_USE_MEM_POOL */


#include <mbedtls/sha1.h>
#include <mbedtls/base64.h>
#include "strcasestr.h"

static const char WS_HEADER[] = "Upgrade: websocket\r\n";
static const char WS_GUID[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
static const char WS_RSP[] = "HTTP/1.1 101 Switching Protocols\r\n" \
                             "Upgrade: websocket\r\n" \
                             "Connection: Upgrade\r\n" \
                             "Sec-WebSocket-Accept: ";

/* Response buffer length (30 = base64 encoded key max length) */
#define WS_BUF_LEN           (sizeof(WS_RSP) + sizeof(CRLF CRLF) + 30 - 2)

/* WebSocket timeout: X*(HTTPD_POLL_INTERVAL), default is 10*4*500ms = 20s */
#ifndef WS_TIMEOUT
#define WS_TIMEOUT           10
#endif

/* Callback functions */
static tWsHandler websocket_cb = NULL;
static tWsOpenHandler websocket_open_cb = NULL;

typedef struct
{
  const char *name;
  u8_t shtml;
} default_filename;

const default_filename g_psDefaultFilenames[] = {
  {"/index.shtml", 1 },
  {"/index.ssi",   1 },
  {"/index.shtm",  1 },
  {"/index.html",  0 },
  {"/index.htm",   0 }
};

#define NUM_DEFAULT_FILENAMES (sizeof(g_psDefaultFilenames) /   \
                               sizeof(default_filename))

#if LWIP_HTTPD_SUPPORT_REQUESTLIST
/** HTTP request is copied here from pbufs for simple parsing */
static char httpd_req_buf[LWIP_HTTPD_MAX_REQ_LENGTH+1];
#endif /* LWIP_HTTPD_SUPPORT_REQUESTLIST */

#if LWIP_HTTPD_SUPPORT_POST
/** Filename for response file to send when POST is finished */
static char http_post_response_filename[LWIP_HTTPD_POST_MAX_RESPONSE_URI_LEN+1];
static tPostBeginHandler    g_PfnPostBeginHandler = NULL;
static tPostReceiveHandler  g_PfnPostReceiveHandler = NULL;
static tPostFinishedHandler g_PfnPostFinishedHandler = NULL;

#endif /* LWIP_HTTPD_SUPPORT_POST */

#if LWIP_HTTPD_DYNAMIC_HEADERS
/* The number of individual strings that comprise the headers sent before each
 * requested file.
 */
#define NUM_FILE_HDR_STRINGS 3
#endif /* LWIP_HTTPD_DYNAMIC_HEADERS */

#if LWIP_HTTPD_SSI

#define HTTPD_LAST_TAG_PART 0xFFFF

enum tag_check_state {
  TAG_NONE,       /* Not processing an SSI tag */
  TAG_LEADIN,     /* Tag lead in "<!--#" being processed */
  TAG_FOUND,      /* Tag name being read, looking for lead-out start */
  TAG_LEADOUT,    /* Tag lead out "-->" being processed */
  TAG_SENDING     /* Sending tag replacement string */
};

struct http_ssi_state {
  const char *parsed;     /* Pointer to the first unparsed byte in buf. */
#if !LWIP_HTTPD_SSI_INCLUDE_TAG
  const char *tag_started;/* Pointer to the first opening '<' of the tag. */
#endif /* !LWIP_HTTPD_SSI_INCLUDE_TAG */
  const char *tag_end;    /* Pointer to char after the closing '>' of the tag. */
  u32_t parse_left; /* Number of unparsed bytes in buf. */
  u16_t tag_index;   /* Counter used by tag parsing state machine */
  u16_t tag_insert_len; /* Length of insert in string tag_insert */
#if LWIP_HTTPD_SSI_MULTIPART
  u16_t tag_part; /* Counter passed to and changed by tag insertion function to insert multiple times */
#endif /* LWIP_HTTPD_SSI_MULTIPART */
  u8_t tag_name_len; /* Length of the tag name in string tag_name */
  char tag_name[LWIP_HTTPD_MAX_TAG_NAME_LEN + 1]; /* Last tag name extracted */
  char tag_insert[LWIP_HTTPD_MAX_TAG_INSERT_LEN + 1]; /* Insert string for tag_name */
  enum tag_check_state tag_state; /* State of the tag processor */
};
#endif /* LWIP_HTTPD_SSI */

struct http_state {
#if LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
  struct http_state *next;
#endif /* LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */
  struct fs_file file_handle;
  struct fs_file *handle;
  char *file;       /* Pointer to first unsent byte in buf. */

  u8_t is_websocket;

  struct tcp_pcb *pcb;
#if LWIP_HTTPD_SUPPORT_REQUESTLIST
  struct pbuf *req;
#endif /* LWIP_HTTPD_SUPPORT_REQUESTLIST */

#if LWIP_HTTPD_DYNAMIC_FILE_READ
  char *buf;        /* File read buffer. */
  int buf_len;      /* Size of file read buffer, buf. */
#endif /* LWIP_HTTPD_DYNAMIC_FILE_READ */
  u32_t left;       /* Number of unsent bytes in buf. */
  u8_t retries;
#if LWIP_HTTPD_SUPPORT_11_KEEPALIVE
  u8_t keepalive;
#endif /* LWIP_HTTPD_SUPPORT_11_KEEPALIVE */
#if LWIP_HTTPD_SSI
  struct http_ssi_state *ssi;
#endif /* LWIP_HTTPD_SSI */
#if LWIP_HTTPD_CGI
  char *params[LWIP_HTTPD_MAX_CGI_PARAMETERS]; /* Params extracted from the request URI */
  char *param_vals[LWIP_HTTPD_MAX_CGI_PARAMETERS]; /* Values for each extracted param */
#endif /* LWIP_HTTPD_CGI */
#if LWIP_HTTPD_DYNAMIC_HEADERS
  const char *hdrs[NUM_FILE_HDR_STRINGS]; /* HTTP headers to be sent. */
  u16_t hdr_pos;     /* The position of the first unsent header byte in the
                        current string */
  u16_t hdr_index;   /* The index of the hdr string currently being sent. */
#endif /* LWIP_HTTPD_DYNAMIC_HEADERS */
#if LWIP_HTTPD_TIMING
  u32_t time_started;
#endif /* LWIP_HTTPD_TIMING */
#if LWIP_HTTPD_SUPPORT_POST
  u32_t post_content_len_left;
#if LWIP_HTTPD_POST_MANUAL_WND
  u32_t unrecved_bytes;
  u8_t no_auto_wnd;
  u8_t post_finished;
#endif /* LWIP_HTTPD_POST_MANUAL_WND */
#endif /* LWIP_HTTPD_SUPPORT_POST*/
};

static err_t http_close_conn(struct tcp_pcb *pcb, struct http_state *hs);
static err_t http_close_or_abort_conn(struct tcp_pcb *pcb, struct http_state *hs, u8_t abort_conn);
static err_t http_find_file(struct http_state *hs, const char *uri, int is_09);
static err_t http_init_file(struct http_state *hs, struct fs_file *file, int is_09, const char *uri, u8_t tag_check);
static err_t http_poll(void *arg, struct tcp_pcb *pcb);

static err_t websocket_send_close(struct tcp_pcb *pcb);

#if LWIP_HTTPD_FS_ASYNC_READ
static void http_continue(void *connection);
#endif /* LWIP_HTTPD_FS_ASYNC_READ */

#if LWIP_HTTPD_SSI
/* SSI insert handler function pointer. */
tSSIHandler g_pfnSSIHandler = NULL;
int g_iNumTags = 0;
const char **g_ppcTags = NULL;

#define LEN_TAG_LEAD_IN 5
const char * const g_pcTagLeadIn = "<!--#";

#define LEN_TAG_LEAD_OUT 3
const char * const g_pcTagLeadOut = "-->";
#endif /* LWIP_HTTPD_SSI */

#if LWIP_HTTPD_CGI
/* CGI handler information */
const tCGI *g_pCGIs;
int g_iNumCGIs;
#endif /* LWIP_HTTPD_CGI */

#if LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
/** global list of active HTTP connections, use to kill the oldest when
    running out of memory */
static struct http_state *http_connections;
#endif /* LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */

#if LWIP_HTTPD_STRNSTR_PRIVATE
/** Like strstr but does not need 'buffer' to be NULL-terminated */
static char*
strnstr(const char* buffer, const char* token, size_t n)
{
  const char* p;
  int tokenlen = (int)strlen(token);
  if (tokenlen == 0) {
    return (char *)buffer;
  }
  for (p = buffer; *p && (p + tokenlen <= buffer + n); p++) {
    if ((*p == *token) && (strncmp(p, token, tokenlen) == 0)) {
      return (char *)p;
    }
  }
  return NULL;
}
#endif /* LWIP_HTTPD_STRNSTR_PRIVATE */

#if LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
static void
http_kill_oldest_connection(u8_t ssi_required)
{
  struct http_state *hs = http_connections;
  struct http_state *hs_free_next = NULL;
  while(hs && hs->next) {
    if (ssi_required) {
      if (hs->next->ssi != NULL) {
        hs_free_next = hs;
      }
    } else {
      hs_free_next = hs;
    }
    hs = hs->next;
  }
  if (hs_free_next != NULL) {
    LWIP_ASSERT("hs_free_next->next != NULL", hs_free_next->next != NULL);
    LWIP_ASSERT("hs_free_next->next->pcb != NULL", hs_free_next->next->pcb != NULL);
    /* send RST when killing a connection because of memory shortage */
    http_close_or_abort_conn(hs_free_next->next->pcb, hs_free_next->next, 1); /* this also unlinks the http_state from the list */
  }
}
#endif /* LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */

#if LWIP_HTTPD_SSI
/** Allocate as struct http_ssi_state. */
static struct http_ssi_state*
http_ssi_state_alloc(void)
{
  struct http_ssi_state *ret = HTTP_ALLOC_SSI_STATE();
#if LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
  if (ret == NULL) {
    http_kill_oldest_connection(1);
    ret = HTTP_ALLOC_SSI_STATE();
  }
#endif /* LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */
  if (ret != NULL) {
    memset(ret, 0, sizeof(struct http_ssi_state));
  }
  return ret;
}

/** Free a struct http_ssi_state. */
static void
http_ssi_state_free(struct http_ssi_state *ssi)
{
  if (ssi != NULL) {
#if HTTPD_USE_MEM_POOL
    memp_free(MEMP_HTTPD_SSI_STATE, ssi);
#else /* HTTPD_USE_MEM_POOL */
    mem_free(ssi);
#endif /* HTTPD_USE_MEM_POOL */
  }
}
#endif /* LWIP_HTTPD_SSI */

/** Initialize a struct http_state.
 */
static void
http_state_init(struct http_state* hs)
{
  /* Initialize the structure. */
  memset(hs, 0, sizeof(struct http_state));
#if LWIP_HTTPD_DYNAMIC_HEADERS
  /* Indicate that the headers are not yet valid */
  hs->hdr_index = NUM_FILE_HDR_STRINGS;
#endif /* LWIP_HTTPD_DYNAMIC_HEADERS */
}

/** Allocate a struct http_state. */
static struct http_state*
http_state_alloc(void)
{
  struct http_state *ret = HTTP_ALLOC_HTTP_STATE();
#if LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
  if (ret == NULL) {
    http_kill_oldest_connection(0);
    ret = HTTP_ALLOC_HTTP_STATE();
  }
#endif /* LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */
  if (ret != NULL) {
    http_state_init(ret);
#if LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
    /* add the connection to the list */
    if (http_connections == NULL) {
      http_connections = ret;
    } else {
      struct http_state *last;
      for(last = http_connections; last->next != NULL; last = last->next);
      LWIP_ASSERT("last != NULL", last != NULL);
      last->next = ret;
    }
#endif /* LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */
  }
  return ret;
}

/** Free a struct http_state.
 * Also frees the file data if dynamic.
 */
static void
http_state_eof(struct http_state *hs)
{
  if(hs->handle) {
#if LWIP_HTTPD_TIMING
    u32_t ms_needed = sys_now() - hs->time_started;
    u32_t needed = LWIP_MAX(1, (ms_needed/100));
    LWIP_DEBUGF(HTTPD_DEBUG_TIMING, ("httpd: needed %"U32_F" ms to send file of %d bytes -> %"U32_F" bytes/sec\n",
      ms_needed, hs->handle->len, ((((u32_t)hs->handle->len) * 10) / needed)));
#endif /* LWIP_HTTPD_TIMING */
    fs_close(hs->handle);
    hs->handle = NULL;
  }
#if LWIP_HTTPD_DYNAMIC_FILE_READ
  if (hs->buf != NULL) {
    mem_free(hs->buf);
    hs->buf = NULL;
  }
#endif /* LWIP_HTTPD_DYNAMIC_FILE_READ */
#if LWIP_HTTPD_SSI
  if (hs->ssi) {
    http_ssi_state_free(hs->ssi);
    hs->ssi = NULL;
  }
#endif /* LWIP_HTTPD_SSI */
}

/** Free a struct http_state.
 * Also frees the file data if dynamic.
 */
static void
http_state_free(struct http_state *hs)
{
  if (hs != NULL) {
    http_state_eof(hs);
#if LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
    /* take the connection off the list */
    if (http_connections) {
      if (http_connections == hs) {
        http_connections = hs->next;
      } else {
        struct http_state *last;
        for(last = http_connections; last->next != NULL; last = last->next) {
          if (last->next == hs) {
            last->next = hs->next;
            break;
          }
        }
      }
    }
#endif /* LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */
#if HTTPD_USE_MEM_POOL
    memp_free(MEMP_HTTPD_STATE, hs);
#else /* HTTPD_USE_MEM_POOL */
    mem_free(hs);
#endif /* HTTPD_USE_MEM_POOL */
  }
}

/** Call tcp_write() in a loop trying smaller and smaller length
 *
 * @param pcb tcp_pcb to send
 * @param ptr Data to send
 * @param length Length of data to send (in/out: on return, contains the
 *        amount of data sent)
 * @param apiflags directly passed to tcp_write
 * @return the return value of tcp_write
 */
static err_t
http_write(struct tcp_pcb *pcb, const void* ptr, u16_t *length, u8_t apiflags)
{
   u16_t len;
   err_t err;
   LWIP_ASSERT("length != NULL", length != NULL);
   len = *length;
   if (len == 0) {
     return ERR_OK;
   }
   do {
     LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("Trying to send %d bytes\n", len));
     err = tcp_write(pcb, ptr, len, apiflags);
     if (err == ERR_MEM) {
       if ((tcp_sndbuf(pcb) == 0)
            || (tcp_sndqueuelen(pcb) >= TCP_SND_QUEUELEN(pcb))
           ) {
         /* no need to try smaller sizes */
         len = 1;
       } else {
         len /= 2;
       }
       LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE,
                   ("Send failed, trying less (%d bytes)\n", len));
     }
   } while ((err == ERR_MEM) && (len > 1));

   if (err == ERR_OK) {
     LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("Sent %d bytes\n", len));
   } else {
     LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("Send failed with err %d (\"%s\")\n", err, lwip_strerr(err)));
   }

   *length = len;
   return err;
}

/**
 * The connection shall be actively closed (using RST to close from fault states).
 * Reset the sent- and recv-callbacks.
 *
 * @param pcb the tcp pcb to reset callbacks
 * @param hs connection state to free
 */
static err_t
http_close_or_abort_conn(struct tcp_pcb *pcb, struct http_state *hs, u8_t abort_conn)
{
  err_t err;
  LWIP_DEBUGF(HTTPD_DEBUG, ("Closing connection %p\n", (void*)pcb));

#if LWIP_HTTPD_SUPPORT_POST
  if (hs != NULL) {
    if ((hs->post_content_len_left != 0)
#if LWIP_HTTPD_POST_MANUAL_WND
       || ((hs->no_auto_wnd != 0) && (hs->unrecved_bytes != 0))
#endif /* LWIP_HTTPD_POST_MANUAL_WND */
       ) {
      /* make sure the post code knows that the connection is closed */
      http_post_response_filename[0] = 0;
      if (g_PfnPostFinishedHandler)
          g_PfnPostFinishedHandler(hs, http_post_response_filename, LWIP_HTTPD_POST_MAX_RESPONSE_URI_LEN);
    }
  }
#endif /* LWIP_HTTPD_SUPPORT_POST*/

  if (hs != NULL) {
    if (hs->is_websocket)
      websocket_send_close(pcb);

    if (hs->req != NULL) {
      /* this should not happen */
      LWIP_DEBUGF(HTTPD_DEBUG, ("Freeing buffer (malformed request?)\n"));
      pbuf_free(hs->req);
      hs->req = NULL;
    }
  }

  tcp_arg(pcb, NULL);
  tcp_recv(pcb, NULL);
  tcp_err(pcb, NULL);
  tcp_poll(pcb, NULL, 0);
  tcp_sent(pcb, NULL);
  if (hs != NULL) {
    http_state_free(hs);
  }

  if (abort_conn) {
    tcp_abort(pcb);
    return ERR_OK;
  }
  err = tcp_close(pcb);
  if (err != ERR_OK) {
    LWIP_DEBUGF(HTTPD_DEBUG, ("Error %d closing %p\n", err, (void*)pcb));
    /* error closing, try again later in poll */
    tcp_poll(pcb, http_poll, HTTPD_POLL_INTERVAL);
  } 
  return err;
}

/**
 * The connection shall be actively closed.
 * Reset the sent- and recv-callbacks.
 *
 * @param pcb the tcp pcb to reset callbacks
 * @param hs connection state to free
 */
static err_t
http_close_conn(struct tcp_pcb *pcb, struct http_state *hs)
{
  return http_close_or_abort_conn(pcb, hs, 0);
}

/** End of file: either close the connection (Connection: close) or
 * close the file (Connection: keep-alive)
 */
static void
http_eof(struct tcp_pcb *pcb, struct http_state *hs)
{
  /* HTTP/1.1 persistent connection? (Not supported for SSI) */
#if LWIP_HTTPD_SUPPORT_11_KEEPALIVE
  if (hs->keepalive && !LWIP_HTTPD_IS_SSI(hs)) {
    http_state_eof(hs);
    http_state_init(hs);
    hs->keepalive = 1;
  } else
#endif /* LWIP_HTTPD_SUPPORT_11_KEEPALIVE */
  if (hs->is_websocket) {
    http_state_eof(hs);
    http_state_init(hs);
    hs->is_websocket = 1;
  } else {
    http_close_conn(pcb, hs);
  }
}

#if LWIP_HTTPD_CGI
/**
 * Extract URI parameters from the parameter-part of an URI in the form
 * "test.cgi?x=y" @todo: better explanation!
 * Pointers to the parameters are stored in hs->param_vals.
 *
 * @param hs http connection state
 * @param params pointer to the NULL-terminated parameter string from the URI
 * @return number of parameters extracted
 */
static int
extract_uri_parameters(struct http_state *hs, char *params)
{
  char *pair;
  char *equals;
  int loop;

  /* If we have no parameters at all, return immediately. */
  if(!params || (params[0] == '\0')) {
      return(0);
  }

  /* Get a pointer to our first parameter */
  pair = params;

  /* Parse up to LWIP_HTTPD_MAX_CGI_PARAMETERS from the passed string and ignore the
   * remainder (if any) */
  for(loop = 0; (loop < LWIP_HTTPD_MAX_CGI_PARAMETERS) && pair; loop++) {

    /* Save the name of the parameter */
    hs->params[loop] = pair;

    /* Remember the start of this name=value pair */
    equals = pair;

    /* Find the start of the next name=value pair and replace the delimiter
     * with a 0 to terminate the previous pair string. */
    pair = strchr(pair, '&');
    if(pair) {
      *pair = '\0';
      pair++;
    } else {
       /* We didn't find a new parameter so find the end of the URI and
        * replace the space with a '\0' */
        pair = strchr(equals, ' ');
        if(pair) {
            *pair = '\0';
        }

        /* Revert to NULL so that we exit the loop as expected. */
        pair = NULL;
    }

    /* Now find the '=' in the previous pair, replace it with '\0' and save
     * the parameter value string. */
    equals = strchr(equals, '=');
    if(equals) {
      *equals = '\0';
      hs->param_vals[loop] = equals + 1;
    } else {
      hs->param_vals[loop] = NULL;
    }
  }

  return loop;
}
#endif /* LWIP_HTTPD_CGI */

#if LWIP_HTTPD_SSI
/**
 * Insert a tag (found in an shtml in the form of "<!--#tagname-->" into the file.
 * The tag's name is stored in ssi->tag_name (NULL-terminated), the replacement
 * should be written to hs->tag_insert (up to a length of LWIP_HTTPD_MAX_TAG_INSERT_LEN).
 * The amount of data written is stored to ssi->tag_insert_len.
 *
 * @todo: return tag_insert_len - maybe it can be removed from struct http_state?
 *
 * @param hs http connection state
 */
static void
get_tag_insert(struct http_state *hs)
{
  int loop;
  size_t len;
  struct http_ssi_state *ssi;
  LWIP_ASSERT("hs != NULL", hs != NULL);
  ssi = hs->ssi;
  LWIP_ASSERT("ssi != NULL", ssi != NULL);
#if LWIP_HTTPD_SSI_MULTIPART
  u16_t current_tag_part = ssi->tag_part;
  ssi->tag_part = HTTPD_LAST_TAG_PART;
#endif /* LWIP_HTTPD_SSI_MULTIPART */

  if(g_pfnSSIHandler && g_ppcTags && g_iNumTags) {

    /* Find this tag in the list we have been provided. */
    for(loop = 0; loop < g_iNumTags; loop++) {
      if(strcmp(ssi->tag_name, g_ppcTags[loop]) == 0) {
        ssi->tag_insert_len = g_pfnSSIHandler(loop, ssi->tag_insert,
           LWIP_HTTPD_MAX_TAG_INSERT_LEN
#if LWIP_HTTPD_SSI_MULTIPART
           , current_tag_part, &ssi->tag_part
#endif /* LWIP_HTTPD_SSI_MULTIPART */
#if LWIP_HTTPD_FILE_STATE
           , hs->handle->state
#endif /* LWIP_HTTPD_FILE_STATE */
           );
        return;
      }
    }
  }

  /* If we drop out, we were asked to serve a page which contains tags that
   * we don't have a handler for. Merely echo back the tags with an error
   * marker. */
#define UNKNOWN_TAG1_TEXT "<b>***UNKNOWN TAG "
#define UNKNOWN_TAG1_LEN  18
#define UNKNOWN_TAG2_TEXT "***</b>"
#define UNKNOWN_TAG2_LEN  7
  len = LWIP_MIN(strlen(ssi->tag_name),
    LWIP_HTTPD_MAX_TAG_INSERT_LEN - (UNKNOWN_TAG1_LEN + UNKNOWN_TAG2_LEN));
  MEMCPY(ssi->tag_insert, UNKNOWN_TAG1_TEXT, UNKNOWN_TAG1_LEN);
  MEMCPY(&ssi->tag_insert[UNKNOWN_TAG1_LEN], ssi->tag_name, len);
  MEMCPY(&ssi->tag_insert[UNKNOWN_TAG1_LEN + len], UNKNOWN_TAG2_TEXT, UNKNOWN_TAG2_LEN);
  ssi->tag_insert[UNKNOWN_TAG1_LEN + len + UNKNOWN_TAG2_LEN] = 0;

  len = strlen(ssi->tag_insert);
  LWIP_ASSERT("len <= 0xffff", len <= 0xffff);
  ssi->tag_insert_len = (u16_t)len;
}
#endif /* LWIP_HTTPD_SSI */

#if LWIP_HTTPD_DYNAMIC_HEADERS
/**
 * Generate the relevant HTTP headers for the given filename and write
 * them into the supplied buffer.
 */
static void
get_http_headers(struct http_state *pState, char *pszURI)
{
  unsigned int iLoop;
  char *pszWork;
  char *pszExt;
  char *pszVars;

  /* Ensure that we initialize the loop counter. */
  iLoop = 0;

  /* In all cases, the second header we send is the server identification
     so set it here. */
  pState->hdrs[1] = g_psHTTPHeaderStrings[HTTP_HDR_SERVER];

  /* Is this a normal file or the special case we use to send back the
     default "404: Page not found" response? */
  if (pszURI == NULL) {
    pState->hdrs[0] = g_psHTTPHeaderStrings[HTTP_HDR_NOT_FOUND];
    pState->hdrs[2] = g_psHTTPHeaderStrings[DEFAULT_404_HTML];

    /* Set up to send the first header string. */
    pState->hdr_index = 0;
    pState->hdr_pos = 0;
    return;
  } else {
    /* We are dealing with a particular filename. Look for one other
       special case.  We assume that any filename with "404" in it must be
       indicative of a 404 server error whereas all other files require
       the 200 OK header. */
    if (strstr(pszURI, "404")) {
      pState->hdrs[0] = g_psHTTPHeaderStrings[HTTP_HDR_NOT_FOUND];
    } else if (strstr(pszURI, "400")) {
      pState->hdrs[0] = g_psHTTPHeaderStrings[HTTP_HDR_BAD_REQUEST];
    } else if (strstr(pszURI, "501")) {
      pState->hdrs[0] = g_psHTTPHeaderStrings[HTTP_HDR_NOT_IMPL];
    } else {
      pState->hdrs[0] = g_psHTTPHeaderStrings[HTTP_HDR_OK];
    }

    /* Determine if the URI has any variables and, if so, temporarily remove
       them. */
    pszVars = strchr(pszURI, '?');
    if(pszVars) {
      *pszVars = '\0';
    }

    /* Get a pointer to the file extension.  We find this by looking for the
       last occurrence of "." in the filename passed. */
    pszExt = NULL;
    pszWork = strchr(pszURI, '.');
    while(pszWork) {
      pszExt = pszWork + 1;
      pszWork = strchr(pszExt, '.');
    }

    /* Now determine the content type and add the relevant header for that. */
    for(iLoop = 0; (iLoop < NUM_HTTP_HEADERS) && pszExt; iLoop++) {
      /* Have we found a matching extension? */
      if(!strcmp(g_psHTTPHeaders[iLoop].extension, pszExt)) {
        pState->hdrs[2] =
          g_psHTTPHeaderStrings[g_psHTTPHeaders[iLoop].headerIndex];
        break;
      }
    }

    /* Reinstate the parameter marker if there was one in the original URI. */
    if(pszVars) {
      *pszVars = '?';
    }
  }

  /* Does the URL passed have any file extension?  If not, we assume it
     is a special-case URL used for control state notification and we do
     not send any HTTP headers with the response. */
  if(!pszExt) {
    /* Force the header index to a value indicating that all headers
       have already been sent. */
    pState->hdr_index = NUM_FILE_HDR_STRINGS;
  } else {
    /* Did we find a matching extension? */
    if(iLoop == NUM_HTTP_HEADERS) {
      /* No - use the default, plain text file type. */
      pState->hdrs[2] = g_psHTTPHeaderStrings[HTTP_HDR_DEFAULT_TYPE];
    }

    /* Set up to send the first header string. */
    pState->hdr_index = 0;
    pState->hdr_pos = 0;
  }
}

/** Sub-function of http_send(): send dynamic headers
 *
 * @returns: - HTTP_NO_DATA_TO_SEND: no new data has been enqueued
 *           - HTTP_DATA_TO_SEND_CONTINUE: continue with sending HTTP body
 *           - HTTP_DATA_TO_SEND_BREAK: data has been enqueued, headers pending,
 *                                      so don't send HTTP body yet
 */
static u8_t
http_send_headers(struct tcp_pcb *pcb, struct http_state *hs)
{
  err_t err;
  u16_t len;
  u8_t data_to_send = HTTP_NO_DATA_TO_SEND;
  u16_t hdrlen, sendlen;

  /* How much data can we send? */
  len = tcp_sndbuf(pcb);
  sendlen = len;

  while(len && (hs->hdr_index < NUM_FILE_HDR_STRINGS) && sendlen) {
    const void *ptr;
    u16_t old_sendlen;
    /* How much do we have to send from the current header? */
    hdrlen = (u16_t)strlen(hs->hdrs[hs->hdr_index]);

    /* How much of this can we send? */
    sendlen = (len < (hdrlen - hs->hdr_pos)) ? len : (hdrlen - hs->hdr_pos);

    /* Send this amount of data or as much as we can given memory
    * constraints. */
    ptr = (const void *)(hs->hdrs[hs->hdr_index] + hs->hdr_pos);
    old_sendlen = sendlen;
    err = http_write(pcb, ptr, &sendlen, HTTP_IS_HDR_VOLATILE(hs, ptr));
    if ((err == ERR_OK) && (old_sendlen != sendlen)) {
      /* Remember that we added some more data to be transmitted. */
      data_to_send = HTTP_DATA_TO_SEND_CONTINUE;
    } else if (err != ERR_OK) {
       /* special case: http_write does not try to send 1 byte */
      sendlen = 0;
    }

    /* Fix up the header position for the next time round. */
    hs->hdr_pos += sendlen;
    len -= sendlen;

    /* Have we finished sending this string? */
    if(hs->hdr_pos == hdrlen) {
      /* Yes - move on to the next one */
      hs->hdr_index++;
      hs->hdr_pos = 0;
    }
  }
  /* If we get here and there are still header bytes to send, we send
   * the header information we just wrote immediately. If there are no
   * more headers to send, but we do have file data to send, drop through
   * to try to send some file data too. */
  if((hs->hdr_index < NUM_FILE_HDR_STRINGS) || !hs->file) {
    LWIP_DEBUGF(HTTPD_DEBUG, ("tcp_output\n"));
    return HTTP_DATA_TO_SEND_BREAK;
  }
  return data_to_send;
}
#endif /* LWIP_HTTPD_DYNAMIC_HEADERS */

/** Sub-function of http_send(): end-of-file (or block) is reached,
 * either close the file or read the next block (if supported).
 *
 * @returns: 0 if the file is finished or no data has been read
 *           1 if the file is not finished and data has been read
 */
static u8_t
http_check_eof(struct tcp_pcb *pcb, struct http_state *hs)
{
#if LWIP_HTTPD_DYNAMIC_FILE_READ
  int count;
#endif /* LWIP_HTTPD_DYNAMIC_FILE_READ */

  /* Do we have a valid file handle? */
  if (hs->handle == NULL) {
    /* No - close the connection. */
    http_eof(pcb, hs);
    return 0;
  }
  if (fs_bytes_left(hs->handle) <= 0) {
    /* We reached the end of the file so this request is done. */
    LWIP_DEBUGF(HTTPD_DEBUG, ("End of file.\n"));
    http_eof(pcb, hs);
    return 0;
  }
#if LWIP_HTTPD_DYNAMIC_FILE_READ
  /* Do we already have a send buffer allocated? */
  if(hs->buf) {
    /* Yes - get the length of the buffer */
    count = hs->buf_len;
  } else {
    /* We don't have a send buffer so allocate one up to 2mss bytes long. */
    count = 2 * tcp_mss(pcb);
    do {
      hs->buf = (char*)mem_malloc((mem_size_t)count);
      if (hs->buf != NULL) {
        hs->buf_len = count;
        break;
      }
      count = count / 2;
    } while (count > 100);

    /* Did we get a send buffer? If not, return immediately. */
    if (hs->buf == NULL) {
      LWIP_DEBUGF(HTTPD_DEBUG, ("No buff\n"));
      return 0;
    }
  }

  /* Read a block of data from the file. */
  LWIP_DEBUGF(HTTPD_DEBUG, ("Trying to read %d bytes.\n", count));

#if LWIP_HTTPD_FS_ASYNC_READ
  count = fs_read_async(hs->handle, hs->buf, count, http_continue, hs);
#else /* LWIP_HTTPD_FS_ASYNC_READ */
  count = fs_read(hs->handle, hs->buf, count);
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
  if (count < 0) {
    if (count == FS_READ_DELAYED) {
      /* Delayed read, wait for FS to unblock us */
      return 0;
    }
    /* We reached the end of the file so this request is done.
     * @todo: don't close here for HTTP/1.1? */
    LWIP_DEBUGF(HTTPD_DEBUG, ("End of file.\n"));
    http_eof(pcb, hs);
    return 0;
  }

  /* Set up to send the block of data we just read */
  LWIP_DEBUGF(HTTPD_DEBUG, ("Read %d bytes.\n", count));
  hs->left = count;
  hs->file = hs->buf;
#if LWIP_HTTPD_SSI
  if (hs->ssi) {
    hs->ssi->parse_left = count;
    hs->ssi->parsed = hs->buf;
  }
#endif /* LWIP_HTTPD_SSI */
#else /* LWIP_HTTPD_DYNAMIC_FILE_READ */
  LWIP_ASSERT("SSI and DYNAMIC_HEADERS turned off but eof not reached", 0);
#endif /* LWIP_HTTPD_SSI || LWIP_HTTPD_DYNAMIC_HEADERS */
  return 1;
}

/** Sub-function of http_send(): This is the normal send-routine for non-ssi files
 *
 * @returns: - 1: data has been written (so call tcp_ouput)
 *           - 0: no data has been written (no need to call tcp_output)
 */
static u8_t
http_send_data_nonssi(struct tcp_pcb *pcb, struct http_state *hs)
{
  err_t err;
  u16_t len;
  u16_t mss;
  u8_t data_to_send = 0;

  /* We are not processing an SHTML file so no tag checking is necessary.
   * Just send the data as we received it from the file. */

  /* We cannot send more data than space available in the send
     buffer. */
  if (tcp_sndbuf(pcb) < hs->left) {
    len = tcp_sndbuf(pcb);
  } else {
    len = (u16_t)hs->left;
    LWIP_ASSERT("hs->left did not fit into u16_t!", (len == hs->left));
  }
  mss = tcp_mss(pcb);
  if (len > (2 * mss)) {
    len = 2 * mss;
  }

  err = http_write(pcb, hs->file, &len, HTTP_IS_DATA_VOLATILE(hs));
  if (err == ERR_OK) {
    data_to_send = 1;
    hs->file += len;
    hs->left -= len;
  }

  return data_to_send;
}

#if LWIP_HTTPD_SSI
/** Sub-function of http_send(): This is the send-routine for ssi files
 *
 * @returns: - 1: data has been written (so call tcp_ouput)
 *           - 0: no data has been written (no need to call tcp_output)
 */
static u8_t
http_send_data_ssi(struct tcp_pcb *pcb, struct http_state *hs)
{
  err_t err = ERR_OK;
  u16_t len;
  u16_t mss;
  u8_t data_to_send = 0;

  struct http_ssi_state *ssi = hs->ssi;
  LWIP_ASSERT("ssi != NULL", ssi != NULL);
  /* We are processing an SHTML file so need to scan for tags and replace
   * them with insert strings. We need to be careful here since a tag may
   * straddle the boundary of two blocks read from the file and we may also
   * have to split the insert string between two tcp_write operations. */

  /* Do we have remaining data to send before parsing more? */
  if(ssi->parsed > hs->file) {
    /* We cannot send more data than space available in the send
       buffer. */
    if (tcp_sndbuf(pcb) < (ssi->parsed - hs->file)) {
      len = tcp_sndbuf(pcb);
    } else {
      LWIP_ASSERT("Data size does not fit into u16_t!",
                  (ssi->parsed - hs->file) <= 0xffff);
      len = (u16_t)(ssi->parsed - hs->file);
    }
    mss = tcp_mss(pcb);
    if(len > (2 * mss)) {
      len = 2 * mss;
    }

    err = http_write(pcb, hs->file, &len, HTTP_IS_DATA_VOLATILE(hs));
    if (err == ERR_OK) {
      data_to_send = 1;
      hs->file += len;
      hs->left -= len;
    }

    /* If the send buffer is full, return now. */
    if(tcp_sndbuf(pcb) == 0) {
      return data_to_send;
    }
  }

  LWIP_DEBUGF(HTTPD_DEBUG, ("State %d, %d left\n", ssi->tag_state, (int)ssi->parse_left));

  /* We have sent all the data that was already parsed so continue parsing
   * the buffer contents looking for SSI tags. */
  while((ssi->parse_left) && (err == ERR_OK)) {
    /* How much data could we send? */
    len = tcp_sndbuf(pcb);
    if (len == 0) {
      return data_to_send;
    }
    switch(ssi->tag_state) {
      case TAG_NONE:
        /* We are not currently processing an SSI tag so scan for the
         * start of the lead-in marker. */
        if(*ssi->parsed == g_pcTagLeadIn[0]) {
          /* We found what could be the lead-in for a new tag so change
           * state appropriately. */
          ssi->tag_state = TAG_LEADIN;
          ssi->tag_index = 1;
#if !LWIP_HTTPD_SSI_INCLUDE_TAG
          ssi->tag_started = ssi->parsed;
#endif /* !LWIP_HTTPD_SSI_INCLUDE_TAG */
        }

        /* Move on to the next character in the buffer */
        ssi->parse_left--;
        ssi->parsed++;
        break;

      case TAG_LEADIN:
        /* We are processing the lead-in marker, looking for the start of
         * the tag name. */

        /* Have we reached the end of the leadin? */
        if(ssi->tag_index == LEN_TAG_LEAD_IN) {
          ssi->tag_index = 0;
          ssi->tag_state = TAG_FOUND;
        } else {
          /* Have we found the next character we expect for the tag leadin? */
          if(*ssi->parsed == g_pcTagLeadIn[ssi->tag_index]) {
            /* Yes - move to the next one unless we have found the complete
             * leadin, in which case we start looking for the tag itself */
            ssi->tag_index++;
          } else {
            /* We found an unexpected character so this is not a tag. Move
             * back to idle state. */
            ssi->tag_state = TAG_NONE;
          }

          /* Move on to the next character in the buffer */
          ssi->parse_left--;
          ssi->parsed++;
        }
        break;

      case TAG_FOUND:
        /* We are reading the tag name, looking for the start of the
         * lead-out marker and removing any whitespace found. */

        /* Remove leading whitespace between the tag leading and the first
         * tag name character. */
        if((ssi->tag_index == 0) && ((*ssi->parsed == ' ') ||
           (*ssi->parsed == '\t') || (*ssi->parsed == '\n') ||
           (*ssi->parsed == '\r'))) {
          /* Move on to the next character in the buffer */
          ssi->parse_left--;
          ssi->parsed++;
          break;
        }

        /* Have we found the end of the tag name? This is signalled by
         * us finding the first leadout character or whitespace */
        if((*ssi->parsed == g_pcTagLeadOut[0]) ||
           (*ssi->parsed == ' ')  || (*ssi->parsed == '\t') ||
           (*ssi->parsed == '\n') || (*ssi->parsed == '\r')) {

          if(ssi->tag_index == 0) {
            /* We read a zero length tag so ignore it. */
            ssi->tag_state = TAG_NONE;
          } else {
            /* We read a non-empty tag so go ahead and look for the
             * leadout string. */
            ssi->tag_state = TAG_LEADOUT;
            LWIP_ASSERT("ssi->tag_index <= 0xff", ssi->tag_index <= 0xff);
            ssi->tag_name_len = (u8_t)ssi->tag_index;
            ssi->tag_name[ssi->tag_index] = '\0';
            if(*ssi->parsed == g_pcTagLeadOut[0]) {
              ssi->tag_index = 1;
            } else {
              ssi->tag_index = 0;
            }
          }
        } else {
          /* This character is part of the tag name so save it */
          if(ssi->tag_index < LWIP_HTTPD_MAX_TAG_NAME_LEN) {
            ssi->tag_name[ssi->tag_index++] = *ssi->parsed;
          } else {
            /* The tag was too long so ignore it. */
            ssi->tag_state = TAG_NONE;
          }
        }

        /* Move on to the next character in the buffer */
        ssi->parse_left--;
        ssi->parsed++;

        break;

      /* We are looking for the end of the lead-out marker. */
      case TAG_LEADOUT:
        /* Remove leading whitespace between the tag leading and the first
         * tag leadout character. */
        if((ssi->tag_index == 0) && ((*ssi->parsed == ' ') ||
           (*ssi->parsed == '\t') || (*ssi->parsed == '\n') ||
           (*ssi->parsed == '\r'))) {
          /* Move on to the next character in the buffer */
          ssi->parse_left--;
          ssi->parsed++;
          break;
        }

        /* Have we found the next character we expect for the tag leadout? */
        if(*ssi->parsed == g_pcTagLeadOut[ssi->tag_index]) {
          /* Yes - move to the next one unless we have found the complete
           * leadout, in which case we need to call the client to process
           * the tag. */

          /* Move on to the next character in the buffer */
          ssi->parse_left--;
          ssi->parsed++;

          if(ssi->tag_index == (LEN_TAG_LEAD_OUT - 1)) {
            /* Call the client to ask for the insert string for the
             * tag we just found. */
#if LWIP_HTTPD_SSI_MULTIPART
            ssi->tag_part = 0; /* start with tag part 0 */
#endif /* LWIP_HTTPD_SSI_MULTIPART */
            get_tag_insert(hs);

            /* Next time through, we are going to be sending data
             * immediately, either the end of the block we start
             * sending here or the insert string. */
            ssi->tag_index = 0;
            ssi->tag_state = TAG_SENDING;
            ssi->tag_end = ssi->parsed;
#if !LWIP_HTTPD_SSI_INCLUDE_TAG
            ssi->parsed = ssi->tag_started;
#endif /* !LWIP_HTTPD_SSI_INCLUDE_TAG*/

            /* If there is any unsent data in the buffer prior to the
             * tag, we need to send it now. */
            if (ssi->tag_end > hs->file) {
              /* How much of the data can we send? */
#if LWIP_HTTPD_SSI_INCLUDE_TAG
              if(len > ssi->tag_end - hs->file) {
                len = (u16_t)(ssi->tag_end - hs->file);
              }
#else /* LWIP_HTTPD_SSI_INCLUDE_TAG*/
              if(len > ssi->tag_started - hs->file) {
                /* we would include the tag in sending */
                len = (u16_t)(ssi->tag_started - hs->file);
              }
#endif /* LWIP_HTTPD_SSI_INCLUDE_TAG*/

              err = http_write(pcb, hs->file, &len, HTTP_IS_DATA_VOLATILE(hs));
              if (err == ERR_OK) {
                data_to_send = 1;
#if !LWIP_HTTPD_SSI_INCLUDE_TAG
                if(ssi->tag_started <= hs->file) {
                  /* pretend to have sent the tag, too */
                  len += ssi->tag_end - ssi->tag_started;
                }
#endif /* !LWIP_HTTPD_SSI_INCLUDE_TAG*/
                hs->file += len;
                hs->left -= len;
              }
            }
          } else {
            ssi->tag_index++;
          }
        } else {
          /* We found an unexpected character so this is not a tag. Move
           * back to idle state. */
          ssi->parse_left--;
          ssi->parsed++;
          ssi->tag_state = TAG_NONE;
        }
        break;

      /*
       * We have found a valid tag and are in the process of sending
       * data as a result of that discovery. We send either remaining data
       * from the file prior to the insert point or the insert string itself.
       */
      case TAG_SENDING:
        /* Do we have any remaining file data to send from the buffer prior
         * to the tag? */
        if(ssi->tag_end > hs->file) {
          /* How much of the data can we send? */
#if LWIP_HTTPD_SSI_INCLUDE_TAG
          if(len > ssi->tag_end - hs->file) {
            len = (u16_t)(ssi->tag_end - hs->file);
          }
#else /* LWIP_HTTPD_SSI_INCLUDE_TAG*/
          LWIP_ASSERT("hs->started >= hs->file", ssi->tag_started >= hs->file);
          if (len > ssi->tag_started - hs->file) {
            /* we would include the tag in sending */
            len = (u16_t)(ssi->tag_started - hs->file);
          }
#endif /* LWIP_HTTPD_SSI_INCLUDE_TAG*/
          if (len != 0) {
            err = http_write(pcb, hs->file, &len, HTTP_IS_DATA_VOLATILE(hs));
          } else {
            err = ERR_OK;
          }
          if (err == ERR_OK) {
            data_to_send = 1;
#if !LWIP_HTTPD_SSI_INCLUDE_TAG
            if(ssi->tag_started <= hs->file) {
              /* pretend to have sent the tag, too */
              len += ssi->tag_end - ssi->tag_started;
            }
#endif /* !LWIP_HTTPD_SSI_INCLUDE_TAG*/
            hs->file += len;
            hs->left -= len;
          }
        } else {
#if LWIP_HTTPD_SSI_MULTIPART
          if(ssi->tag_index >= ssi->tag_insert_len) {
            /* Did the last SSIHandler have more to send? */
            if (ssi->tag_part != HTTPD_LAST_TAG_PART) {
              /* If so, call it again */
              ssi->tag_index = 0;
              get_tag_insert(hs);
            }
          }
#endif /* LWIP_HTTPD_SSI_MULTIPART */

          /* Do we still have insert data left to send? */
          if(ssi->tag_index < ssi->tag_insert_len) {
            /* We are sending the insert string itself. How much of the
             * insert can we send? */
            if(len > (ssi->tag_insert_len - ssi->tag_index)) {
              len = (ssi->tag_insert_len - ssi->tag_index);
            }

            /* Note that we set the copy flag here since we only have a
             * single tag insert buffer per connection. If we don't do
             * this, insert corruption can occur if more than one insert
             * is processed before we call tcp_output. */
            err = http_write(pcb, &(ssi->tag_insert[ssi->tag_index]), &len,
                             HTTP_IS_TAG_VOLATILE(hs));
            if (err == ERR_OK) {
              data_to_send = 1;
              ssi->tag_index += len;
              /* Don't return here: keep on sending data */
            }
          } else {
#if LWIP_HTTPD_SSI_MULTIPART
            if (ssi->tag_part == HTTPD_LAST_TAG_PART)
#endif /* LWIP_HTTPD_SSI_MULTIPART */
            {
              /* We have sent all the insert data so go back to looking for
               * a new tag. */
              LWIP_DEBUGF(HTTPD_DEBUG, ("Everything sent.\n"));
              ssi->tag_index = 0;
              ssi->tag_state = TAG_NONE;
#if !LWIP_HTTPD_SSI_INCLUDE_TAG
              ssi->parsed = ssi->tag_end;
#endif /* !LWIP_HTTPD_SSI_INCLUDE_TAG*/
            }
          }
          break;
      }
    }
  }

  /* If we drop out of the end of the for loop, this implies we must have
   * file data to send so send it now. In TAG_SENDING state, we've already
   * handled this so skip the send if that's the case. */
  if((ssi->tag_state != TAG_SENDING) && (ssi->parsed > hs->file)) {
    /* We cannot send more data than space available in the send
       buffer. */
    if (tcp_sndbuf(pcb) < (ssi->parsed - hs->file)) {
      len = tcp_sndbuf(pcb);
    } else {
      LWIP_ASSERT("Data size does not fit into u16_t!",
                  (ssi->parsed - hs->file) <= 0xffff);
      len = (u16_t)(ssi->parsed - hs->file);
    }
    if(len > (2 * tcp_mss(pcb))) {
      len = 2 * tcp_mss(pcb);
    }

    err = http_write(pcb, hs->file, &len, HTTP_IS_DATA_VOLATILE(hs));
    if (err == ERR_OK) {
      data_to_send = 1;
      hs->file += len;
      hs->left -= len;
    }
  }
  return data_to_send;
}
#endif /* LWIP_HTTPD_SSI */

/**
 * Try to send more data on this pcb.
 *
 * @param pcb the pcb to send data
 * @param hs connection state
 */
static u8_t
http_send(struct tcp_pcb *pcb, struct http_state *hs)
{
  u8_t data_to_send = HTTP_NO_DATA_TO_SEND;

  LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("http_send: pcb=%p hs=%p left=%d\n", (void*)pcb,
    (void*)hs, hs != NULL ? (int)hs->left : 0));

#if LWIP_HTTPD_SUPPORT_POST && LWIP_HTTPD_POST_MANUAL_WND
  if (hs->unrecved_bytes != 0) {
    return 0;
  }
#endif /* LWIP_HTTPD_SUPPORT_POST && LWIP_HTTPD_POST_MANUAL_WND */

  /* If we were passed a NULL state structure pointer, ignore the call. */
  if (hs == NULL) {
    return 0;
  }

#if LWIP_HTTPD_FS_ASYNC_READ
  /* Check if we are allowed to read from this file.
     (e.g. SSI might want to delay sending until data is available) */
  if (!fs_is_file_ready(hs->handle, http_continue, hs)) {
    return 0;
  }
#endif /* LWIP_HTTPD_FS_ASYNC_READ */

#if LWIP_HTTPD_DYNAMIC_HEADERS
  /* Do we have any more header data to send for this file? */
  if(hs->hdr_index < NUM_FILE_HDR_STRINGS) {
    data_to_send = http_send_headers(pcb, hs);
    if (data_to_send != HTTP_DATA_TO_SEND_CONTINUE) {
      return data_to_send;
    }
  }
#endif /* LWIP_HTTPD_DYNAMIC_HEADERS */

  /* Have we run out of file data to send? If so, we need to read the next
   * block from the file. */
  if (hs->left == 0) {
    if (!http_check_eof(pcb, hs)) {
      return 0;
    }
  }

#if LWIP_HTTPD_SSI
  if(hs->ssi) {
    data_to_send = http_send_data_ssi(pcb, hs);
  } else
#endif /* LWIP_HTTPD_SSI */
  {
    data_to_send = http_send_data_nonssi(pcb, hs);
  }

  if((hs->left == 0) && (fs_bytes_left(hs->handle) <= 0)) {
    /* We reached the end of the file so this request is done.
     * This adds the FIN flag right into the last data segment. */
    LWIP_DEBUGF(HTTPD_DEBUG, ("End of file.\n"));
    http_eof(pcb, hs);
    return 0;
  }
  LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("send_data end.\n"));
  return data_to_send;
}

#if LWIP_HTTPD_SUPPORT_EXTSTATUS
/** Initialize a http connection with a file to send for an error message
 *
 * @param hs http connection state
 * @param error_nr HTTP error number
 * @return ERR_OK if file was found and hs has been initialized correctly
 *         another err_t otherwise
 */
static err_t
http_find_error_file(struct http_state *hs, u16_t error_nr)
{
  const char *uri1, *uri2, *uri3;
  err_t err;

  if (error_nr == 501) {
    uri1 = "/501.html";
    uri2 = "/501.htm";
    uri3 = "/501.shtml";
  } else {
    /* 400 (bad request is the default) */
    uri1 = "/400.html";
    uri2 = "/400.htm";
    uri3 = "/400.shtml";
  }
  err = fs_open(&hs->file_handle, uri1);
  if (err != ERR_OK) {
    err = fs_open(&hs->file_handle, uri2);
    if (err != ERR_OK) {
      err = fs_open(&hs->file_handle, uri3);
      if (err != ERR_OK) {
        LWIP_DEBUGF(HTTPD_DEBUG, ("Error page for error %"U16_F" not found\n",
          error_nr));
        return ERR_ARG;
      }
    }
  }
  return http_init_file(hs, &hs->file_handle, 0, NULL, 0);
}
#else /* LWIP_HTTPD_SUPPORT_EXTSTATUS */
#define http_find_error_file(hs, error_nr) ERR_ARG
#endif /* LWIP_HTTPD_SUPPORT_EXTSTATUS */

/**
 * Get the file struct for a 404 error page.
 * Tries some file names and returns NULL if none found.
 *
 * @param uri pointer that receives the actual file name URI
 * @return file struct for the error page or NULL no matching file was found
 */
static struct fs_file *
http_get_404_file(struct http_state *hs, const char **uri)
{
  err_t err;

  LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("No match:sending 404.\n"));
  
  *uri = "/404.html";
  err = fs_open(&hs->file_handle, *uri);
  if (err != ERR_OK) {
    /* 404.html doesn't exist. Try 404.htm instead. */
    *uri = "/404.htm";
    err = fs_open(&hs->file_handle, *uri);
    if (err != ERR_OK) {
      /* 404.htm doesn't exist either. Try 404.shtml instead. */
      *uri = "/404.shtml";
      err = fs_open(&hs->file_handle, *uri);
      if (err != ERR_OK) {
        /* 404.htm doesn't exist either. Indicate to the caller that it should
         * send back a default 404 page.
         */
        *uri = NULL;
        return NULL;
      }
    }
  }

  return &hs->file_handle;
}

#if LWIP_HTTPD_SUPPORT_POST

void http_set_post_handlers(tPostBeginHandler bHandler,     // optional, can be NULL
                            tPostReceiveHandler rHandler,   // required
                            tPostFinishedHandler fHandler)  // optional, can be NULL
{
    g_PfnPostBeginHandler = bHandler;
    g_PfnPostReceiveHandler = rHandler;
    g_PfnPostFinishedHandler = fHandler;
}
                            
static err_t
http_handle_post_finished(struct http_state *hs)
{
#if LWIP_HTTPD_POST_MANUAL_WND
  /* Prevent multiple calls to httpd_post_finished, since it might have already
     been called before from httpd_post_data_recved(). */
  if (hs->post_finished) {
    return ERR_OK;
  }
  hs->post_finished = 1;
#endif /* LWIP_HTTPD_POST_MANUAL_WND */
  /* application error or POST finished */
  /* NULL-terminate the buffer */
  http_post_response_filename[0] = 0;
  if (g_PfnPostFinishedHandler)
      g_PfnPostFinishedHandler(hs, http_post_response_filename, LWIP_HTTPD_POST_MAX_RESPONSE_URI_LEN);
  return http_find_file(hs, http_post_response_filename, 0);
}

/** Pass received POST body data to the application and correctly handle
 * returning a response document or closing the connection.
 * ATTENTION: The application is responsible for the pbuf now, so don't free it!
 *
 * @param hs http connection state
 * @param p pbuf to pass to the application
 * @return ERR_OK if passed successfully, another err_t if the response file
 *         hasn't been found (after POST finished)
 */
static err_t
http_post_rxpbuf(struct http_state *hs, struct pbuf *p)
{
  err_t err;

  /* adjust remaining Content-Length */
  if (hs->post_content_len_left < p->tot_len) {
    hs->post_content_len_left = 0;
  } else {
    hs->post_content_len_left -= p->tot_len;
  }
  if (g_PfnPostReceiveHandler)
      err = g_PfnPostReceiveHandler(hs, p);
  else {
      pbuf_free(p);
      err = ERR_OK;
  }
  if ((err != ERR_OK) || (hs->post_content_len_left == 0)) {
#if LWIP_HTTPD_SUPPORT_POST && LWIP_HTTPD_POST_MANUAL_WND
    if (hs->unrecved_bytes != 0) {
       return ERR_OK;
    }
#endif /* LWIP_HTTPD_SUPPORT_POST && LWIP_HTTPD_POST_MANUAL_WND */
    /* application error or POST finished */
    return http_handle_post_finished(hs);
  }

  return ERR_OK;
}

/** Handle a post request. Called from http_parse_request when method 'POST'
 * is found.
 *
 * @param p The input pbuf (containing the POST header and body).
 * @param hs The http connection state.
 * @param data HTTP request (header and part of body) from input pbuf(s).
 * @param data_len Size of 'data'.
 * @param uri The HTTP URI parsed from input pbuf(s).
 * @param uri_end Pointer to the end of 'uri' (here, the rest of the HTTP
 *                header starts).
 * @return ERR_OK: POST correctly parsed and accepted by the application.
 *         ERR_INPROGRESS: POST not completely parsed (no error yet)
 *         another err_t: Error parsing POST or denied by the application
 */
static err_t
http_post_request(struct pbuf **inp, struct http_state *hs,
                  char *data, u16_t data_len, char *uri, char *uri_end)
{
    err_t err;
    /* search for end-of-header (first double-CRLF) */
    char* crlfcrlf = strnstr(uri_end + 1, CRLF CRLF, data_len - (uri_end + 1 - data));

    if (crlfcrlf == NULL) 
        LWIP_DEBUGF(HTTPD_DEBUG, ("POST not terminated by CRLFCRLF\n"));
    else {
        /* search for "Content-Length: " */
        #define HTTP_HDR_CONTENT_LEN                "Content-Length: "
        #define HTTP_HDR_CONTENT_LEN_LEN            16
        #define HTTP_HDR_CONTENT_LEN_DIGIT_MAX_LEN  10
        char *scontent_len = strnstr(uri_end + 1, HTTP_HDR_CONTENT_LEN, crlfcrlf - (uri_end + 1));
        if (scontent_len == NULL) 
            LWIP_DEBUGF(HTTPD_DEBUG, ("POST does not have Content-Length\n"));
        else {
            char *scontent_len_end = strnstr(scontent_len + HTTP_HDR_CONTENT_LEN_LEN, CRLF, HTTP_HDR_CONTENT_LEN_DIGIT_MAX_LEN);
            if (scontent_len_end != NULL) {
                int content_len;
                char *conten_len_num = scontent_len + HTTP_HDR_CONTENT_LEN_LEN;
                *scontent_len_end = 0;
                content_len = atoi(conten_len_num);
                if (content_len <= 0) {
                    LWIP_DEBUGF(HTTPD_DEBUG, ("POST Content-Length is %d!\n",content_len));
                    return ERR_ARG;
                } else {
                    /* adjust length of HTTP header passed to application */
                    const char *hdr_start_after_uri = uri_end + 1;
                    u16_t hdr_len = LWIP_MIN(data_len, crlfcrlf + 4 - data);
                    u16_t hdr_data_len = LWIP_MIN(data_len, crlfcrlf + 4 - hdr_start_after_uri);
                    u8_t post_auto_wnd = 1;
                    http_post_response_filename[0] = 0;
                    if (g_PfnPostBeginHandler)
                        err = g_PfnPostBeginHandler(hs, uri, hdr_start_after_uri, hdr_data_len, content_len,
                                            http_post_response_filename, 
                                            LWIP_HTTPD_POST_MAX_RESPONSE_URI_LEN, 
                                            &post_auto_wnd);
                    else 
                        err = ERR_OK;
                    
                    if (err == ERR_OK) {
                        /* try to pass in data of the first pbuf(s) */
                        struct pbuf *q = *inp;
                        u16_t start_offset = hdr_len;
                        #if LWIP_HTTPD_POST_MANUAL_WND
                            hs->no_auto_wnd = !post_auto_wnd;
                        #endif /* LWIP_HTTPD_POST_MANUAL_WND */
                        /* set the Content-Length to be received for this POST */
                        hs->post_content_len_left = (u32_t)content_len;

                        /* get to the pbuf where the body starts */
                        while((q != NULL) && (q->len <= start_offset)) {
                            struct pbuf *head = q;
                            start_offset -= q->len;
                            q = q->next;
                            /* free the head pbuf */
                            head->next = NULL;
                            pbuf_free(head);
                        }
                        *inp = NULL;
                        if (q != NULL) {
                            /* hide the remaining HTTP header */
                            pbuf_header(q, -(s16_t)start_offset);
                            #if LWIP_HTTPD_POST_MANUAL_WND
                                  if (!post_auto_wnd) {
                                    /* already tcp_recved() this data... */
                                    hs->unrecved_bytes = q->tot_len;
                                  }
                            #endif /* LWIP_HTTPD_POST_MANUAL_WND */
                            return http_post_rxpbuf(hs, q);
                        } else {
                            return ERR_OK;
                        }
                    } else {
                        /* return file passed from application */
                        return http_find_file(hs, http_post_response_filename, 0);
                    }
                }
            }
        }
    }
  
    LWIP_DEBUGF(HTTPD_DEBUG, ("POST incomplete\n"));
    
  /* if we come here, the POST is incomplete */
#if LWIP_HTTPD_SUPPORT_REQUESTLIST
  return ERR_INPROGRESS;
#else /* LWIP_HTTPD_SUPPORT_REQUESTLIST */
  return ERR_ARG;
#endif /* LWIP_HTTPD_SUPPORT_REQUESTLIST */
}

#if LWIP_HTTPD_POST_MANUAL_WND
/** A POST implementation can call this function to update the TCP window.
 * This can be used to throttle data reception (e.g. when received data is
 * programmed to flash and data is received faster than programmed).
 *
 * @param connection A connection handle passed to httpd_post_begin for which
 *        httpd_post_finished has *NOT* been called yet!
 * @param recved_len Length of data received (for window update)
 */
void httpd_post_data_recved(void *connection, u16_t recved_len)
{
  struct http_state *hs = (struct http_state*)connection;
  if (hs != NULL) {
    if (hs->no_auto_wnd) {
      u16_t len = recved_len;
      if (hs->unrecved_bytes >= recved_len) {
        hs->unrecved_bytes -= recved_len;
      } else {
        LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_LEVEL_WARNING, ("httpd_post_data_recved: recved_len too big\n"));
        len = (u16_t)hs->unrecved_bytes;
        hs->unrecved_bytes = 0;
      }
      if (hs->pcb != NULL) {
        if (len != 0) {
          tcp_recved(hs->pcb, len);
        }
        if ((hs->post_content_len_left == 0) && (hs->unrecved_bytes == 0)) {
          /* finished handling POST */
          http_handle_post_finished(hs);
          http_send(hs->pcb, hs);
        }
      }
    }
  }
}
#endif /* LWIP_HTTPD_POST_MANUAL_WND */

#endif /* LWIP_HTTPD_SUPPORT_POST */

#if LWIP_HTTPD_FS_ASYNC_READ
/** Try to send more data if file has been blocked before
 * This is a callback function passed to fs_read_async().
 */
static void
http_continue(void *connection)
{
  struct http_state *hs = (struct http_state*)connection;
  if (hs && (hs->pcb) && (hs->handle)) {
    LWIP_ASSERT("hs->pcb != NULL", hs->pcb != NULL);
    LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("httpd_continue: try to send more data\n"));
    if (http_send(hs->pcb, hs)) {
      /* If we wrote anything to be sent, go ahead and send it now. */
      LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("tcp_output\n"));
      tcp_output(hs->pcb);
    }
  }
}
#endif /* LWIP_HTTPD_FS_ASYNC_READ */

/**
 * When data has been received in the correct state, try to parse it
 * as a HTTP request.
 *
 * @param p the received pbuf
 * @param hs the connection state
 * @param pcb the tcp_pcb which received this packet
 * @return ERR_OK if request was OK and hs has been initialized correctly
 *         ERR_INPROGRESS if request was OK so far but not fully received
 *         another err_t otherwise
 */
static err_t
http_parse_request(struct pbuf **inp, struct http_state *hs, struct tcp_pcb *pcb)
{
  char *data;
  char *crlf;
  u16_t data_len;
  struct pbuf *p = *inp;
#if LWIP_HTTPD_SUPPORT_REQUESTLIST
  u16_t clen;
#endif /* LWIP_HTTPD_SUPPORT_REQUESTLIST */
#if LWIP_HTTPD_SUPPORT_POST
  err_t err;
#endif /* LWIP_HTTPD_SUPPORT_POST */

  LWIP_UNUSED_ARG(pcb); /* only used for post */
  LWIP_ASSERT("p != NULL", p != NULL);
  LWIP_ASSERT("hs != NULL", hs != NULL);

  if ((hs->handle != NULL) || (hs->file != NULL)) {
    LWIP_DEBUGF(HTTPD_DEBUG, ("Received data while sending a file\n"));
    /* already sending a file */
    /* @todo: abort? */
    return ERR_USE;
  }

#if LWIP_HTTPD_SUPPORT_REQUESTLIST

  LWIP_DEBUGF(HTTPD_DEBUG, ("Received %"U16_F" bytes\n", p->tot_len));

  /* first check allowed characters in this pbuf? */

  /* enqueue the pbuf */
  if (hs->req == NULL) {
    LWIP_DEBUGF(HTTPD_DEBUG, ("First pbuf\n"));
    hs->req = p;
  } else {
    LWIP_DEBUGF(HTTPD_DEBUG, ("pbuf enqueued\n"));
    pbuf_cat(hs->req, p);
  }

  if (hs->req->next != NULL) {
    data_len = LWIP_MIN(hs->req->tot_len, LWIP_HTTPD_MAX_REQ_LENGTH);
    pbuf_copy_partial(hs->req, httpd_req_buf, data_len, 0);
    data = httpd_req_buf;
  } else
#endif /* LWIP_HTTPD_SUPPORT_REQUESTLIST */
  {
    data = (char *)p->payload;
    data_len = p->len;
    if (p->len != p->tot_len) {
      LWIP_DEBUGF(HTTPD_DEBUG, ("Warning: incomplete header due to chained pbufs\n"));
    }
  }

  /* Parse WebSocket request */
  hs->is_websocket = 0;
  unsigned char *retval = NULL;
  if (strncasestr(data, WS_HEADER, data_len)) {
    LWIP_DEBUGF(HTTPD_DEBUG, ("WebSocket opening handshake\n"));
    char *key_start = strncasestr(data, "Sec-WebSocket-Key: ", data_len);
    if (key_start) {
      key_start += 19;
      char *key_end = strncasestr(key_start, "\r\n", data_len);
      if (key_end) {
        char key[64];
        int len = sizeof (char) * (key_end - key_start);
        if ((len + sizeof (WS_GUID) < sizeof (key)) && (len > 0)) {
          /* Allocate response buffer */
          retval = mem_malloc(WS_BUF_LEN);
          if (retval == NULL) {
            LWIP_DEBUGF(HTTPD_DEBUG, ("Out of memory\n"));
            return ERR_MEM;
          }
          unsigned char *retval_ptr;
          retval_ptr = memcpy(retval, WS_RSP, sizeof(WS_RSP));
          retval_ptr += sizeof(WS_RSP) - 1;

          /* Concatenate key */
          memcpy(key, key_start, len);
          strlcpy(&key[len], WS_GUID, sizeof(key));
          LWIP_DEBUGF(HTTPD_DEBUG, ("Resulting key: %s\n", key));

          /* Get SHA1 */
          int key_len = sizeof(WS_GUID) - 1 + len;
          unsigned char sha1sum[20];
          mbedtls_sha1((unsigned char *) key, key_len, sha1sum);

          /* Base64 encode */
          unsigned int olen;
          mbedtls_base64_encode(NULL, 0, &olen, sha1sum, 20); //get length
          int ok = mbedtls_base64_encode(retval_ptr, WS_BUF_LEN, &olen, sha1sum, 20);

          if (ok == 0) {
            memcpy(&retval_ptr[olen], CRLF CRLF, sizeof(CRLF CRLF));
            hs->is_websocket = 1;
            LWIP_DEBUGF(HTTPD_DEBUG, ("Base64 encoded: %s\n", retval_ptr));
          }
        } else {
          LWIP_DEBUGF(HTTPD_DEBUG, ("Key overflow"));
          return ERR_MEM;
        }
      }
    } else {
      LWIP_DEBUGF(HTTPD_DEBUG, ("error: malformed packet\n"));
      return ERR_ARG;
    }
  }

  /* received enough data for minimal request? */
  if (data_len >= MIN_REQ_LEN) {
    /* wait for CRLF before parsing anything */
    crlf = strnstr(data, CRLF, data_len);
    if (crlf != NULL) {
#if LWIP_HTTPD_SUPPORT_POST
      int is_post = 0;
#endif /* LWIP_HTTPD_SUPPORT_POST */
      int is_09 = 0;
      char *sp1, *sp2;
      u16_t left_len, uri_len;
      LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("CRLF received, parsing request\n"));
      /* parse method */
      if (!strncmp(data, "GET ", 4)) {
        sp1 = data + 3;
        /* received GET request */
        data[data_len] = 0;
        LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("Received GET request:\n%s\n",data));
#if LWIP_HTTPD_SUPPORT_POST
      } else if (!strncmp(data, "POST ", 5)) {
        /* store request type */
        is_post = 1;
        sp1 = data + 4;
        /* received POST request */
        data[data_len] = 0;
        LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("Received POST request\n%s\n",data));
#endif /* LWIP_HTTPD_SUPPORT_POST */
      } else {
        /* null-terminate the METHOD (pbuf is freed anyway wen returning) */
        data[4] = 0;
        /* unsupported method! */
        LWIP_DEBUGF(HTTPD_DEBUG, ("Unsupported request method (not implemented): \"%s\"\n",
          data));
        return http_find_error_file(hs, 501);
      }
      /* if we come here, method is OK, parse URI */
      left_len = data_len - ((sp1 +1) - data);
      sp2 = strnstr(sp1 + 1, " ", left_len);
#if LWIP_HTTPD_SUPPORT_V09
      if (sp2 == NULL) {
        /* HTTP 0.9: respond with correct protocol version */
        sp2 = strnstr(sp1 + 1, CRLF, left_len);
        is_09 = 1;
#if LWIP_HTTPD_SUPPORT_POST
        if (is_post) {
          /* HTTP/0.9 does not support POST */
          goto badrequest;
        }
#endif /* LWIP_HTTPD_SUPPORT_POST */
      }
#endif /* LWIP_HTTPD_SUPPORT_V09 */
      uri_len = sp2 - (sp1 + 1);
      if ((sp2 != 0) && (sp2 > sp1)) {
        /* wait for CRLFCRLF (indicating end of HTTP headers) before parsing anything */
        if (strnstr(data, CRLF CRLF, data_len) != NULL) {
          char *uri = sp1 + 1;
#if LWIP_HTTPD_SUPPORT_11_KEEPALIVE
          if (!is_09 && strnstr(data, HTTP11_CONNECTIONKEEPALIVE, data_len)) {
            hs->keepalive = 1;
          }
#endif /* LWIP_HTTPD_SUPPORT_11_KEEPALIVE */
          
        #ifdef HTTPD_DIRECT_HOOK
        char* hostName = strnstr(data, CRLF "Host:", data_len);
        if (!is_post && hostName != NULL) {
            char* hEnd;
            hostName += 7;
            while (*hostName==' ') hostName++;
            hEnd = hostName;
            while (*hEnd != '\r') hEnd++;
            // Note: swaps CR -> terminating zero
            *hEnd = 0;
            //printf("httpd GET, for %s\n",hostName);
        }
        #endif

          /* null-terminate the METHOD (pbuf is freed anyway wen returning) */
          *sp1 = 0;
          uri[uri_len] = 0;
          LWIP_DEBUGF(HTTPD_DEBUG, ("Received \"%s\" request for URI: \"%s\"\n",
                      data, uri));
#if LWIP_HTTPD_SUPPORT_POST
          if (is_post) {
#if LWIP_HTTPD_SUPPORT_REQUESTLIST
            struct pbuf **q = &hs->req;
#else /* LWIP_HTTPD_SUPPORT_REQUESTLIST */
            struct pbuf **q = inp;
#endif /* LWIP_HTTPD_SUPPORT_REQUESTLIST */
            err = http_post_request(q, hs, data, data_len, uri, sp2);
            if (err != ERR_OK) {
              /* restore header for next try */
              *sp1 = ' ';
              *sp2 = ' ';
              uri[uri_len] = ' ';
            }
            if (err == ERR_ARG) {
              goto badrequest;
            }
            return err;
          } else
#endif /* LWIP_HTTPD_SUPPORT_POST */
          {
            if (hs->is_websocket && retval != NULL) {
              LWIP_DEBUGF(HTTPD_DEBUG, ("Sending:\n%s\n", retval));
              u16_t len = strlen((char *) retval);
              http_write(pcb, retval, &len, 0);
              mem_free(retval);
              if(websocket_open_cb)
                websocket_open_cb(pcb, uri);
              return ERR_OK; // We handled this
            } else {
                #ifdef HTTPD_DIRECT_HOOK
                u16_t dataLen = 0;
                const void* dataP = NULL;
                if (!is_post && httpd_respond_direct(uri,hostName,&dataP,&dataLen)==ERR_OK) {
                    http_write(pcb, dataP, &dataLen, 0);
                    return ERR_OK;
                }
                #endif
              return http_find_file(hs, uri, is_09);
            }
          }
        }
      } else {
        LWIP_DEBUGF(HTTPD_DEBUG, ("invalid URI\n"));
      }
    }
  }

#if LWIP_HTTPD_SUPPORT_REQUESTLIST
  clen = pbuf_clen(hs->req);
  if ((hs->req->tot_len <= LWIP_HTTPD_REQ_BUFSIZE) &&
    (clen <= LWIP_HTTPD_REQ_QUEUELEN)) {
    /* request not fully received (too short or CRLF is missing) */
    return ERR_INPROGRESS;
  } else
#endif /* LWIP_HTTPD_SUPPORT_REQUESTLIST */
  {
#if LWIP_HTTPD_SUPPORT_POST
badrequest:
#endif /* LWIP_HTTPD_SUPPORT_POST */
    LWIP_DEBUGF(HTTPD_DEBUG, ("bad request\n"));
    /* could not parse request */
    return http_find_error_file(hs, 400);
  }
}

/** Try to find the file specified by uri and, if found, initialize hs
 * accordingly.
 *
 * @param hs the connection state
 * @param uri the HTTP header URI
 * @param is_09 1 if the request is HTTP/0.9 (no HTTP headers in response)
 * @return ERR_OK if file was found and hs has been initialized correctly
 *         another err_t otherwise
 */
static err_t
http_find_file(struct http_state *hs, const char *uri, int is_09)
{
  size_t loop;
  struct fs_file *file = NULL;
  char *params;
  err_t err;
#if LWIP_HTTPD_CGI
  int i;
  int count;
#endif /* LWIP_HTTPD_CGI */
#if !LWIP_HTTPD_SSI
  const
#endif /* !LWIP_HTTPD_SSI */
  /* By default, assume we will not be processing server-side-includes tags */
  u8_t tag_check = 0;

  /* Have we been asked for the default root file? */
  if((uri[0] == '/') &&  (uri[1] == 0)) {
    /* Try each of the configured default filenames until we find one
       that exists. */
    for (loop = 0; loop < NUM_DEFAULT_FILENAMES; loop++) {
      LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("Looking for %s...\n", g_psDefaultFilenames[loop].name));
      err = fs_open(&hs->file_handle, (char *)g_psDefaultFilenames[loop].name);
      uri = (char *)g_psDefaultFilenames[loop].name;
      if(err == ERR_OK) {
        file = &hs->file_handle;
        LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("Opened.\n"));
#if LWIP_HTTPD_SSI
        tag_check = g_psDefaultFilenames[loop].shtml;
#endif /* LWIP_HTTPD_SSI */
        break;
      }
    }
    if (file == NULL) {
      /* None of the default filenames exist so send back a 404 page */
      file = http_get_404_file(hs, &uri);
#if LWIP_HTTPD_SSI
      tag_check = 0;
#endif /* LWIP_HTTPD_SSI */
    }
  } else {
    /* No - we've been asked for a specific file. */
    /* First, isolate the base URI (without any parameters) */
    params = (char *)strchr(uri, '?');
    if (params != NULL) {
      /* URI contains parameters. NULL-terminate the base URI */
      *params = '\0';
      params++;
    }

#if LWIP_HTTPD_CGI
    /* Does the base URI we have isolated correspond to a CGI handler? */
    if (g_iNumCGIs && g_pCGIs) {
      for (i = 0; i < g_iNumCGIs; i++) {
        if (strcmp(uri, g_pCGIs[i].pcCGIName) == 0) {
          /*
           * We found a CGI that handles this URI so extract the
           * parameters and call the handler.
           */
           count = extract_uri_parameters(hs, params);
           uri = g_pCGIs[i].pfnCGIHandler(i, count, hs->params,
                                          hs->param_vals);
           break;
        }
      }
    }
#endif /* LWIP_HTTPD_CGI */

    LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("Opening %s\n", uri));

    err = fs_open(&hs->file_handle, uri);
    if (err == ERR_OK) {
       file = &hs->file_handle;
    } else {
      file = http_get_404_file(hs, &uri);
    }
#if LWIP_HTTPD_SSI
    if (file != NULL) {
      /* See if we have been asked for an shtml file and, if so,
         enable tag checking. */
      tag_check = 0;
      for (loop = 0; loop < NUM_SHTML_EXTENSIONS; loop++) {
        if (strstr(uri, g_pcSSIExtensions[loop])) {
          tag_check = 1;
          break;
        }
      }
    }
#endif /* LWIP_HTTPD_SSI */
  }
  return http_init_file(hs, file, is_09, uri, tag_check);
}

/** Initialize a http connection with a file to send (if found).
 * Called by http_find_file and http_find_error_file.
 *
 * @param hs http connection state
 * @param file file structure to send (or NULL if not found)
 * @param is_09 1 if the request is HTTP/0.9 (no HTTP headers in response)
 * @param uri the HTTP header URI
 * @param tag_check enable SSI tag checking
 * @return ERR_OK if file was found and hs has been initialized correctly
 *         another err_t otherwise
 */
static err_t
http_init_file(struct http_state *hs, struct fs_file *file, int is_09, const char *uri, u8_t tag_check)
{
  if (file != NULL) {
    /* file opened, initialise struct http_state */
#if LWIP_HTTPD_SSI
    if (tag_check) {
      struct http_ssi_state *ssi = http_ssi_state_alloc();
      if (ssi != NULL) {
        ssi->tag_index = 0;
        ssi->tag_state = TAG_NONE;
        ssi->parsed = file->data;
        ssi->parse_left = file->len;
        ssi->tag_end = file->data;
        hs->ssi = ssi;
      }
    }
#else /* LWIP_HTTPD_SSI */
    LWIP_UNUSED_ARG(tag_check);
#endif /* LWIP_HTTPD_SSI */
    hs->handle = file;
    hs->file = (char*)file->data;
    LWIP_ASSERT("File length must be positive!", (file->len >= 0));
    hs->left = file->len;
    hs->retries = 0;
#if LWIP_HTTPD_TIMING
    hs->time_started = sys_now();
#endif /* LWIP_HTTPD_TIMING */
#if !LWIP_HTTPD_DYNAMIC_HEADERS
    LWIP_ASSERT("HTTP headers not included in file system", hs->handle->http_header_included);
#endif /* !LWIP_HTTPD_DYNAMIC_HEADERS */
#if LWIP_HTTPD_SUPPORT_V09
    if (hs->handle->http_header_included && is_09) {
      /* HTTP/0.9 responses are sent without HTTP header,
         search for the end of the header. */
      char *file_start = strnstr(hs->file, CRLF CRLF, hs->left);
      if (file_start != NULL) {
        size_t diff = file_start + 4 - hs->file;
        hs->file += diff;
        hs->left -= (u32_t)diff;
      }
    }
#endif /* LWIP_HTTPD_SUPPORT_V09*/
  } else {
    hs->handle = NULL;
    hs->file = NULL;
    hs->left = 0;
    hs->retries = 0;
  }
#if LWIP_HTTPD_DYNAMIC_HEADERS
    /* Determine the HTTP headers to send based on the file extension of
   * the requested URI. */
  if ((hs->handle == NULL) || !hs->handle->http_header_included) {
    get_http_headers(hs, (char*)uri);
  }
#else /* LWIP_HTTPD_DYNAMIC_HEADERS */
  LWIP_UNUSED_ARG(uri);
#endif /* LWIP_HTTPD_DYNAMIC_HEADERS */
  return ERR_OK;
}

/**
 * The pcb had an error and is already deallocated.
 * The argument might still be valid (if != NULL).
 */
static void
http_err(void *arg, err_t err)
{
  struct http_state *hs = (struct http_state *)arg;
  LWIP_UNUSED_ARG(err);

  LWIP_DEBUGF(HTTPD_DEBUG, ("http_err: %s", lwip_strerr(err)));

  if (hs != NULL) {
    http_state_free(hs);
  }
}

/**
 * Data has been sent and acknowledged by the remote host.
 * This means that more data can be sent.
 */
static err_t
http_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
  struct http_state *hs = (struct http_state *)arg;

  LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("http_sent %p\n", (void*)pcb));

  LWIP_UNUSED_ARG(len);

  if (hs == NULL) {
    return ERR_OK;
  }

  hs->retries = 0;

  http_send(pcb, hs);

  return ERR_OK;
}

/**
 * The poll function is called every 2nd second.
 * If there has been no data sent (which resets the retries) in 8 seconds, close.
 * If the last portion of a file has not been sent in 2 seconds, close.
 *
 * This could be increased, but we don't want to waste resources for bad connections.
 */
static err_t
http_poll(void *arg, struct tcp_pcb *pcb)
{
  struct http_state *hs = (struct http_state *)arg;
  LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("http_poll: pcb=%p hs=%p pcb_state=%s\n",
    (void*)pcb, (void*)hs, tcp_debug_state_str(pcb->state)));

  if (hs == NULL) {
    err_t closed;
    /* arg is null, close. */
    LWIP_DEBUGF(HTTPD_DEBUG, ("http_poll: arg is NULL, close\n"));
    closed = http_close_conn(pcb, NULL);
    LWIP_UNUSED_ARG(closed);
#if LWIP_HTTPD_ABORT_ON_CLOSE_MEM_ERROR
    if (closed == ERR_MEM) {
       tcp_abort(pcb);
       return ERR_ABRT;
    }
#endif /* LWIP_HTTPD_ABORT_ON_CLOSE_MEM_ERROR */
    return ERR_OK;
  } else {
    hs->retries++;
    if (hs->retries == ((hs->is_websocket) ? WS_TIMEOUT : HTTPD_MAX_RETRIES)) {
      LWIP_DEBUGF(HTTPD_DEBUG, ("http_poll: too many retries, close\n"));
      http_close_conn(pcb, hs);
      return ERR_OK;
    }

    /* If this connection has a file open, try to send some more data. If
     * it has not yet received a GET request, don't do this since it will
     * cause the connection to close immediately. */
    if(hs && (hs->handle)) {
      LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("http_poll: try to send more data\n"));
      if(http_send(pcb, hs)) {
        /* If we wrote anything to be sent, go ahead and send it now. */
        LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("tcp_output\n"));
        tcp_output(pcb);
      }
    }
  }

  return ERR_OK;
}

void
websocket_register_callbacks(tWsOpenHandler ws_open_cb, tWsHandler ws_cb)
{
  websocket_open_cb = ws_open_cb;
  websocket_cb = ws_cb;
}

err_t
websocket_write(struct tcp_pcb *pcb, const uint8_t *data, uint16_t len, uint8_t mode)
{
  uint8_t *buf = mem_malloc(len + 4);
  if (buf == NULL) {
    LWIP_DEBUGF(HTTPD_DEBUG, ("[websocket_write] out of memory\n"));
    return ERR_MEM;
  }

  int offset = 2;
  buf[0] = 0x80 | mode;
  if (len > 125) {
    offset = 4;
    buf[1] = 126;
    buf[2] = len >> 8;
    buf[3] = len;
  } else {
    buf[1] = len;
  }

  memcpy(&buf[offset], data, len);
  len += offset;

  LWIP_DEBUGF(HTTPD_DEBUG, ("[websocket_write] sending packet\n"));
  err_t retval = http_write(pcb, buf, &len, TCP_WRITE_FLAG_COPY);
  mem_free(buf);

  return retval;
}

/**
 * Send status code 1000 (normal closure).
 */
static err_t
websocket_send_close(struct tcp_pcb *pcb)
{
  const u8_t buf[] = {0x88, 0x02, 0x03, 0xe8};
  u16_t len = sizeof (buf);
  LWIP_DEBUGF(HTTPD_DEBUG, ("[wsoc] closing connection\n"));
  return tcp_write(pcb, buf, len, TCP_WRITE_FLAG_COPY);
}

/**
 * Parse websocket frame.
 *
 * @return ERR_OK: frame parsed
 *         ERR_CLSD: close request from client
 *         ERR_VAL: invalid frame.
 */
static err_t
websocket_parse(struct tcp_pcb *pcb, struct pbuf *p)
{
  u8_t *data = (u8_t *) p->payload;
  u16_t data_len = p->len;

  if (data != NULL && data_len > 1) {
    LWIP_DEBUGF(HTTPD_DEBUG, ("[wsoc] frame received\n"));
    if ((data[0] & 0x80) == 0) {
      LWIP_DEBUGF(HTTPD_DEBUG, ("Warning: continuation frames not supported\n"));
      return ERR_OK;
    }
    u8_t opcode = data[0] & 0x0F;
    switch (opcode) {
      case WS_TEXT_MODE:
      case WS_BIN_MODE:
        LWIP_DEBUGF(HTTPD_DEBUG, ("Opcode: 0x%hX, frame length: %d\n", opcode, data_len));
        if (data_len > 6 && websocket_cb != NULL) {
          int data_offset = 6;
          u8_t *dptr = &data[6];
          u8_t *kptr = &data[2];
          u16_t len = data[1] & 0x7F;

          if (len == 127) {
            /* most likely won't happen inside non-fragmented frame */
            LWIP_DEBUGF(HTTPD_DEBUG, ("Warning: frame is too long\n"));
            return ERR_OK;
          } else if (len == 126) {
            /* extended length */
            dptr += 2;
            kptr += 2;
            data_offset += 2;
            len = (data[2] << 8) | data[3];
          }

          data_len -= data_offset;

          if (len > data_len) {
            LWIP_DEBUGF(HTTPD_DEBUG, ("Error: incorrect frame size\n"));
            return ERR_VAL;
          }

          if (data_len != len)
            LWIP_DEBUGF(HTTPD_DEBUG, ("Warning: segmented frame received\n"));

          /* unmask */
          for (int i = 0; i < len; i++)
            *(dptr++) ^= kptr[i % 4];

          /* user callback */
          websocket_cb(pcb, &data[data_offset], len, opcode);
        }
        break;
      case 0x08: // close
        LWIP_DEBUGF(HTTPD_DEBUG, ("Close request\n"));
        return ERR_CLSD;
      default:
        LWIP_DEBUGF(HTTPD_DEBUG, ("Unsupported opcode 0x%hX\n", opcode));
        break;
    }
    return ERR_OK;
  }
  return ERR_VAL;
}

/**
 * Data has been received on this pcb.
 * For HTTP 1.0, this should normally only happen once (if the request fits in one packet).
 */
static err_t
http_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  err_t parsed = ERR_ABRT;
  struct http_state *hs = (struct http_state *)arg;
  LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("http_recv: pcb=%p pbuf=%p err=%s\n", (void*)pcb,
    (void*)p, lwip_strerr(err)));

  if (hs != NULL && hs->is_websocket) {
    if (p == NULL) {
      LWIP_DEBUGF(HTTPD_DEBUG, ("http_recv: buffer error\n"));
      http_close_or_abort_conn(pcb, hs, 0);
      return ERR_BUF;
    }
    tcp_recved(pcb, p->tot_len);
    err_t err = websocket_parse(pcb, p);
    if (p != NULL) {
      /* otherwise tcp buffer hogs */
      LWIP_DEBUGF(HTTPD_DEBUG, ("[wsoc] freeing buffer\n"));
      pbuf_free(p);
    }
    if (err == ERR_CLSD) {
      http_close_conn(pcb, hs);
    }
    /* reset timeout */
    hs->retries = 0;
    return ERR_OK;
  }

  if ((err != ERR_OK) || (p == NULL) || (hs == NULL)) {
    /* error or closed by other side? */
    if (p != NULL) {
      /* Inform TCP that we have taken the data. */
      tcp_recved(pcb, p->tot_len);
      pbuf_free(p);
    }
    if (hs == NULL) {
      /* this should not happen, only to be robust */
      LWIP_DEBUGF(HTTPD_DEBUG, ("Error, http_recv: hs is NULL, close\n"));
    }
    http_close_conn(pcb, hs);
    return ERR_OK;
  }

#if LWIP_HTTPD_SUPPORT_POST && LWIP_HTTPD_POST_MANUAL_WND
  if (hs->no_auto_wnd) {
     hs->unrecved_bytes += p->tot_len;
  } else
#endif /* LWIP_HTTPD_SUPPORT_POST && LWIP_HTTPD_POST_MANUAL_WND */
  {
    /* Inform TCP that we have taken the data. */
    tcp_recved(pcb, p->tot_len);
  }

#if LWIP_HTTPD_SUPPORT_POST
  if (hs->post_content_len_left > 0) {
    /* reset idle counter when POST data is received */
    hs->retries = 0;
    /* this is data for a POST, pass the complete pbuf to the application */
    http_post_rxpbuf(hs, p);
    /* pbuf is passed to the application, don't free it! */
    if (hs->post_content_len_left == 0) {
      /* all data received, send response or close connection */
      http_send(pcb, hs);
    }
    return ERR_OK;
  } else
#endif /* LWIP_HTTPD_SUPPORT_POST */
  {
    if (hs->handle == NULL) {
      parsed = http_parse_request(&p, hs, pcb);
      LWIP_ASSERT("http_parse_request: unexpected return value", parsed == ERR_OK
        || parsed == ERR_INPROGRESS ||parsed == ERR_ARG
        || parsed == ERR_USE || parsed == ERR_MEM);
    } else {
      LWIP_DEBUGF(HTTPD_DEBUG, ("http_recv: already sending data\n"));
    }
#if LWIP_HTTPD_SUPPORT_REQUESTLIST
    if (parsed != ERR_INPROGRESS) {
      /* request fully parsed or error */
      if (hs->req != NULL) {
        pbuf_free(hs->req);
        hs->req = NULL;
      }
    }
#else /* LWIP_HTTPD_SUPPORT_REQUESTLIST */
    if (p != NULL) {
      /* pbuf not passed to application, free it now */
      pbuf_free(p);
    }
#endif /* LWIP_HTTPD_SUPPORT_REQUESTLIST */
    if (parsed == ERR_OK) {
#if LWIP_HTTPD_SUPPORT_POST
      if (hs->post_content_len_left == 0)
#endif /* LWIP_HTTPD_SUPPORT_POST */
      {
        LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("http_recv: data %p len %"S32_F"\n", hs->file, hs->left));
        http_send(pcb, hs);
      }
    } else if (parsed == ERR_ARG || parsed == ERR_MEM) {
      /* @todo: close on ERR_USE? */
      http_close_conn(pcb, hs);
    }
  }
  return ERR_OK;
}

struct tcp_pcb* gListenPCB = NULL;

/**
 * A new incoming connection has been accepted.
 */
static err_t
http_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
  struct http_state *hs;
  struct tcp_pcb_listen *lpcb = (struct tcp_pcb_listen*)arg;
  LWIP_UNUSED_ARG(err);
  LWIP_DEBUGF(HTTPD_DEBUG, ("http_accept %p / %p\n", (void*)pcb, arg));
  
  /* Decrease the listen backlog counter */
  tcp_accepted(lpcb);
  /* Set priority */
  tcp_setprio(pcb, HTTPD_TCP_PRIO);

  /* Allocate memory for the structure that holds the state of the
     connection - initialized by that function. */
  hs = http_state_alloc();
  if (hs == NULL) {
    LWIP_DEBUGF(HTTPD_DEBUG, ("http_accept: Out of memory, RST\n"));
    return ERR_MEM;
  }
  hs->pcb = pcb;

  /* Tell TCP that this is the structure we wish to be passed for our
     callbacks. */
  tcp_arg(pcb, hs);

  /* Set up the various callback functions */
  tcp_recv(pcb, http_recv);
  tcp_err(pcb, http_err);
  tcp_poll(pcb, http_poll, HTTPD_POLL_INTERVAL);
  tcp_sent(pcb, http_sent);
  
  return ERR_OK;
}

/**
 * Initialize the httpd with the specified local address.
 */


static void
httpd_init_addr(const ip_addr_t *local_addr)
{
  struct tcp_pcb *pcb;
  err_t err;

  pcb = tcp_new();
  LWIP_ASSERT("httpd_init: tcp_new failed", pcb != NULL);
  tcp_setprio(pcb, HTTPD_TCP_PRIO);
  
  /* set SOF_REUSEADDR here to explicitly bind httpd to multiple interfaces */
  // ip_set_option(pcb, SOF_REUSEADDR);
  
  err = tcp_bind(pcb, local_addr, HTTPD_SERVER_PORT);
  LWIP_ASSERT("httpd_init: tcp_bind failed", err == ERR_OK);
  pcb = tcp_listen(pcb);
  LWIP_ASSERT("httpd_init: tcp_listen failed", pcb != NULL);
  /* initialize callback arg and accept callback */
  tcp_arg(pcb, pcb);
  tcp_accept(pcb, http_accept);
  gListenPCB = pcb;
}

/**
 * Initialize the httpd: set up a listening PCB and bind it to the defined port
 */
void
httpd_init(void)
{
#if HTTPD_USE_MEM_POOL
  LWIP_ASSERT("memp_sizes[MEMP_HTTPD_STATE] >= sizeof(http_state)",
     memp_sizes[MEMP_HTTPD_STATE] >= sizeof(http_state));
  LWIP_ASSERT("memp_sizes[MEMP_HTTPD_SSI_STATE] >= sizeof(http_ssi_state)",
     memp_sizes[MEMP_HTTPD_SSI_STATE] >= sizeof(http_ssi_state));
#endif
  LWIP_DEBUGF(HTTPD_DEBUG, ("httpd_init\n"));

  httpd_init_addr(IP_ADDR_ANY);
}

#if LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
    
/* External stop */
void
httpd_stop(void)
{
    err_t err;
    
    LWIP_DEBUGF(HTTPD_DEBUG, ("httpd_stop\n"));
    // Close any active connections
    while (http_connections) {
        LWIP_DEBUGF(HTTPD_DEBUG, ("Closing conn\n"));
        err = http_close_conn(http_connections->pcb,http_connections);
        LWIP_ASSERT("close conn failed",err==ERR_OK);
    }
    
    // Close listening PCB if present
    if (gListenPCB) {
        LWIP_DEBUGF(HTTPD_DEBUG, ("Closing listener\n"));
        err = tcp_close(gListenPCB);
        LWIP_ASSERT("close listen failed",err==ERR_OK);
        gListenPCB = NULL;
    }
}

#endif

#if LWIP_HTTPD_SSI
/**
 * Set the SSI handler function.
 *
 * @param ssi_handler the SSI handler function
 * @param tags an array of SSI tag strings to search for in SSI-enabled files
 * @param num_tags number of tags in the 'tags' array
 */
void
http_set_ssi_handler(tSSIHandler ssi_handler, const char **tags, int num_tags)
{
  LWIP_DEBUGF(HTTPD_DEBUG, ("http_set_ssi_handler\n"));

  LWIP_ASSERT("no ssi_handler given", ssi_handler != NULL);
  LWIP_ASSERT("no tags given", tags != NULL);
  LWIP_ASSERT("invalid number of tags", num_tags > 0);

  g_pfnSSIHandler = ssi_handler;
  g_ppcTags = tags;
  g_iNumTags = num_tags;
}
#endif /* LWIP_HTTPD_SSI */

#if LWIP_HTTPD_CGI
/**
 * Set an array of CGI filenames/handler functions
 *
 * @param cgis an array of CGI filenames/handler functions
 * @param num_handlers number of elements in the 'cgis' array
 */
void
http_set_cgi_handlers(const tCGI *cgis, int num_handlers)
{
  LWIP_ASSERT("no cgis given", cgis != NULL);
  LWIP_ASSERT("invalid number of handlers", num_handlers > 0);

  g_pCGIs = cgis;
  g_iNumCGIs = num_handlers;
}
#endif /* LWIP_HTTPD_CGI */

#endif /* LWIP_TCP */
