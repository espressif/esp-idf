/* With adaptations by Espressif Systems
 *
 * Copyright (c) 2013 Tatsuhiro Tsujikawa
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <netdb.h>

#include "connectlib.h"

/* Basic parser from nghttp2/examples/client.c */
int parse_uri(struct uri *res, const char *uri)
{
    /* We only interested in https */
    size_t len, i, offset;
    int ipv6addr = 0;
    memset(res, 0, sizeof(struct uri));
    len = strlen(uri);
    if (len < 9 || memcmp("https://", uri, 8) != 0) {
        return -1;
    }
    offset = 8;
    res->host = res->hostport = &uri[offset];
    res->hostlen = 0;
    if (uri[offset] == '[') {
        /* IPv6 literal address */
        ++offset;
        ++res->host;
        ipv6addr = 1;
        for (i = offset; i < len; ++i) {
            if (uri[i] == ']') {
                res->hostlen = i - offset;
                offset = i + 1;
                break;
            }
        }
    } else {
        const char delims[] = ":/?#";
        for (i = offset; i < len; ++i) {
            if (strchr(delims, uri[i]) != NULL) {
                break;
            }
        }
        res->hostlen = i - offset;
        offset = i;
    }
    if (res->hostlen == 0) {
        return -1;
    }
    /* Assuming https */
    res->port = 443;
    if (offset < len) {
        if (uri[offset] == ':') {
            /* port */
            const char delims[] = "/?#";
            int port = 0;
            ++offset;
            for (i = offset; i < len; ++i) {
                if (strchr(delims, uri[i]) != NULL) {
                    break;
                }
                if ('0' <= uri[i] && uri[i] <= '9') {
                    port *= 10;
                    port += uri[i] - '0';
                    if (port > 65535) {
                        return -1;
                    }
                } else {
                    return -1;
                }
            }
            if (port == 0) {
                return -1;
            }
            offset = i;
            res->port = (uint16_t)port;
        }
    }
    res->hostportlen = (size_t)(uri + offset + ipv6addr - res->host);
    for (i = offset; i < len; ++i) {
        if (uri[i] == '#') {
            break;
        }
    }
    if (i - offset == 0) {
        res->path = "/";
        res->pathlen = 1;
    } else {
        res->path = &uri[offset];
        res->pathlen = i - offset;
    }
    return 0;
}

int connect_to_host(const char *host, size_t hostlen, uint16_t port)
{
    int ret;
    struct addrinfo hints = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
    };

    char service[6];
    snprintf(service, sizeof(service), "%u", port);

    char *use_host = calloc(1, hostlen + 1);
    if (!use_host) {
         return -1;
    }
    strncpy(use_host, host, hostlen);

    struct addrinfo *res;
    ret = getaddrinfo(use_host, service, &hints, &res);
    if (ret) {
        free(use_host);
        return -1;
    }
    free(use_host);

    ret = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (ret < 0) {
        goto err_freeaddr;
    }

    int fd = ret;
    ret = connect(fd, res->ai_addr, res->ai_addrlen);
    if (ret < 0) {
        goto err_freesocket;
    }

    return fd;

err_freesocket:
    close(fd);
err_freeaddr:
    freeaddrinfo(res);
    return -1;
}

