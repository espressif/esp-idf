/*
Esp8266 http server - core routines
*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain
 * this notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

#include "httpd.h"
#include "httpd-platform.h"
#include "string.h"
#include "esp_types.h"
#include <stdio.h>
#include <stdlib.h>
#include "esp_system.h"
#include "esp_wifi.h"

#if 0
//Max amount of simultaneous connections
#define MAX_CONN 8
//Max length of request head. This is statically allocated for each connection.
#define MAX_HEAD_LEN 1024
//Max post buffer len. This is dynamically malloc'ed if needed.
#define MAX_POST 1024
//Max send buffer len. This is allocated on the stack.
#define MAX_SENDBUFF_LEN 2048
//If some data can't be sent because the underlaying socket doesn't accept the data (like the nonos
//layer is prone to do), we put it in a backlog that is dynamically malloc'ed. This defines the max
//size of the backlog.
#define MAX_BACKLOG_SIZE (4*1024)

typedef struct HttpSendBacklogItem HttpSendBacklogItem;

struct HttpSendBacklogItem {
    int len;
    HttpSendBacklogItem *next;
    char data[];
};

//Flags
#define HFL_HTTP11 (1<<0)
#define HFL_CHUNKED (1<<1)
#define HFL_SENDINGBODY (1<<2)
#define HFL_DISCONAFTERSENT (1<<3)

//Private data for http connection
struct HttpdPriv {
    char head[MAX_HEAD_LEN];
    int headPos;
    char *sendBuff;
    int sendBuffLen;
    char *chunkHdr;
    HttpSendBacklogItem *sendBacklog;
    int sendBacklogSize;
    int flags;
};
#endif

//Connection pool
static HttpdConnData *connData[HTTPD_MAX_CONNECTIONS];
//This gets set at init time.
static HttpdBuiltInUrl *builtInUrls;

#if 0
//Struct to keep extension->mime data in
typedef struct {
    const char *ext;
    const char *mimetype;
} MimeMap;

//#define RSTR(a) ((const char)(a))

//The mappings from file extensions to mime types. If you need an extra mime type,
//add it here.
static const ICACHE_RODATA_ATTR MimeMap mimeTypes[] = {
    {"htm", "text/htm"},
    {"html", "text/html"},
    {"css", "text/css"},
    {"js", "text/javascript"},
    {"txt", "text/plain"},
    {"jpg", "image/jpeg"},
    {"jpeg", "image/jpeg"},
    {"png", "image/png"},
    {"svg", "image/svg+xml"},
    {NULL, "text/html"}, //default value
};

//Returns a static char* to a mime type for a given url to a file.
const char  *httpdGetMimetype(char *url) {
    int i = 0;
    //Go find the extension
    char *ext = url + (strlen(url) - 1);
    while (ext != url && *ext != '.') ext--;
    if (*ext == '.') ext++;

    //ToDo: strcmp is case sensitive; we may want to do case-intensive matching here...
    while (mimeTypes[i].ext != NULL && strcmp(ext, mimeTypes[i].ext) != 0) i++;
    return mimeTypes[i].mimetype;
}
#endif
#if 0
void  __attribute__((weak))
httpd_encrypt(ConnTypePtr conn, char *plain, unsigned short plen, char **cipher, unsigned short *clen)
{
    *cipher = plain;
    *clen = plen;
}

void  __attribute__((weak))
httpd_decrypt(ConnTypePtr conn, char *cipher, unsigned short clen, char **plain, unsigned short *plen)
{
    *plain = cipher;
    *plen = clen;
}

#endif
//Looks up the connData info for a specific connection
static HttpdConnData  *httpdFindConnData(ConnTypePtr conn, char *remIp, int remPort) {
    int i = 0;
    for (i = 0; i < HTTPD_MAX_CONNECTIONS; i++) {
        if (connData[i] && connData[i]->remote_port == remPort &&
                memcmp(connData[i]->remote_ip, remIp, 4) == 0) {
            connData[i]->conn = conn;
            return connData[i];
        }
    }
    //Shouldn't happen.
    httpd_printf("*** Unknown connection %d.%d.%d.%d:%d\n", remIp[0]&0xff, remIp[1]&0xff, remIp[2]&0xff, remIp[3]&0xff, remPort);
    httpdPlatDisconnect(conn);
    return NULL;
}

//Retires a connection for re-use
static void  httpdRetireConn(HttpdConnData *conn) {
    if (conn->priv->sendBacklog != NULL) {
        HttpSendBacklogItem *i, *j;
        i = conn->priv->sendBacklog;
        do {
            j = i;
            i = i->next;
            free(j);
        } while (i != NULL);
    }
    if (conn->post->buff != NULL) free(conn->post->buff);
    if (conn->post != NULL) free(conn->post);
    if (conn->priv != NULL) free(conn->priv);
    if (conn) free(conn);
    int i = 0;
    for (i = 0; i < HTTPD_MAX_CONNECTIONS; i++) {
        if (connData[i] == conn) connData[i] = NULL;
    }
}

//Stupid li'l helper function that returns the value of a hex char.
static int   httpdHexVal(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

//Decode a percent-encoded value.
//Takes the valLen bytes stored in val, and converts it into at most retLen bytes that
//are stored in the ret buffer. Returns the actual amount of bytes used in ret. Also
//zero-terminates the ret buffer.
int  httpdUrlDecode(char *val, int valLen, char *ret, int retLen) {
    int s = 0, d = 0;
    int esced = 0, escVal = 0;
    while (s < valLen && d < retLen) {
        if (esced == 1)  {
            escVal = httpdHexVal(val[s]) << 4;
            esced = 2;
        } else if (esced == 2) {
            escVal += httpdHexVal(val[s]);
            ret[d++] = escVal;
            esced = 0;
        } else if (val[s] == '%') {
            esced = 1;
        } else if (val[s] == '+') {
            ret[d++] = ' ';
        } else {
            ret[d++] = val[s];
        }
        s++;
    }
    if (d < retLen) ret[d] = 0;
    return d;
}

//Find a specific arg in a string of get- or post-data.
//Line is the string of post/get-data, arg is the name of the value to find. The
//zero-terminated result is written in buff, with at most buffLen bytes used. The
//function returns the length of the result, or -1 if the value wasn't found. The
//returned string will be urldecoded already.
int  httpdFindArg(char *line, char *arg, char *buff, int buffLen) {
    char *p, *e;
    if (line == NULL) return -1;
    p = line;
    while (p != NULL && *p != '\n' && *p != '\r' && *p != 0) {
//      httpd_printf("findArg: %s\n", p);
        if (strncmp(p, arg, strlen(arg)) == 0 && p[strlen(arg)] == '=') {
            p += strlen(arg) + 1; //move p to start of value
            e = (char*)strstr(p, "&");
            if (e == NULL) e = p + strlen(p);
//          httpd_printf("findArg: val %s len %d\n", p, (e-p));
            return httpdUrlDecode(p, (e - p), buff, buffLen);
        }
        p = (char*)strstr(p, "&");
        if (p != NULL) p += 1;
    }
    //httpd_printf("Finding %s in %s: Not found :/\n", arg, line);
    return -1; //not found
}

//Get the value of a certain header in the HTTP client head
//Returns true when found, false when not found.
int  httpdGetHeader(HttpdConnData *conn, char *header, char *ret, int retLen) {
    char *p = conn->priv->head;
    p = p + strlen(p) + 1; //skip GET/POST part
    p = p + strlen(p) + 1; //skip HTTP part
    while (p < (conn->priv->head + conn->priv->headPos)) {
        while (*p <= 32 && *p != 0) p++; //skip crap at start
        //See if this is the header
        if (strncmp(p, header, strlen(header)) == 0 && p[strlen(header)] == ':') {
            //Skip 'key:' bit of header line
            p = p + strlen(header) + 1;
            //Skip past spaces after the colon
            while (*p == ' ') p++;
            //Copy from p to end
            while (*p != 0 && *p != '\r' && *p != '\n' && retLen > 1) {
                *ret++ = *p++;
                retLen--;
            }
            //Zero-terminate string
            *ret = 0;
            //All done :)
            return 1;
        }
        p += strlen(p) + 1; //Skip past end of string and \0 terminator
    }
    return 0;
}

//Call before calling httpdStartResponse to disable automatically-chosen transfer
//encodings (specifically, for now, chunking) and fall back on Connection: Close.
void  httpdDisableTransferEncoding(HttpdConnData *conn) {
    conn->priv->flags &= ~HFL_CHUNKED;
}

//Start the response headers.
void  httpdStartResponse(HttpdConnData *conn, int code) {
    char buff[256];
    int l;
    l = sprintf(buff, "HTTP/1.%d %d OK\r\nServer: esp32-httpd/"HTTPDVER"\r\n%s\r\n",
                (conn->priv->flags & HFL_HTTP11) ? 1 : 0,
                code,
                (conn->priv->flags & HFL_CHUNKED) ? "Transfer-Encoding: chunked" : "Connection: close");
    httpdSend(conn, buff, l);
}

//Send a http header.
void  httpdHeader(HttpdConnData *conn, const char *field, const char *val) {
    httpdSend(conn, field, -1);
    httpdSend(conn, ": ", -1);
    httpdSend(conn, val, -1);
    httpdSend(conn, "\r\n", -1);
}

//Finish the headers.
void  httpdEndHeaders(HttpdConnData *conn) {
    httpdSend(conn, "\r\n", -1);
    conn->priv->flags |= HFL_SENDINGBODY;
}

//Redirect to the given URL.
void  httpdRedirect(HttpdConnData *conn, char *newUrl) {
    httpdStartResponse(conn, 302);
    httpdHeader(conn, "Location", newUrl);
    httpdEndHeaders(conn);
    httpdSend(conn, "Moved to ", -1);
    httpdSend(conn, newUrl, -1);
}

//Use this as a cgi function to redirect one url to another.
int  cgiRedirect(HttpdConnData *connData) {
    if (connData->conn == NULL) {
        //Connection aborted. Clean up.
        return HTTPD_CGI_DONE;
    }
    httpdRedirect(connData, (char*)connData->cgiArg);
    return HTTPD_CGI_DONE;
}

//Used to spit out a 404 error
static int  cgiNotFound(HttpdConnData *connData) {
    if (connData->conn == NULL) return HTTPD_CGI_DONE;
    httpdStartResponse(connData, 404);
    httpdEndHeaders(connData);
    httpdSend(connData, "404 File not found.", -1);
    return HTTPD_CGI_DONE;
}

//This CGI function redirects to a fixed url of http://[hostname]/ if hostname field of request isn't
//already that hostname. Use this in combination with a DNS server that redirects everything to the
//ESP in order to load a HTML page as soon as a phone, tablet etc connects to the ESP. Watch out:
//this will also redirect connections when the ESP is in STA mode, potentially to a hostname that is not
//in the 'official' DNS and so will fail.
int  cgiRedirectToHostname(HttpdConnData *connData) {
    static const char hostFmt[] = "http://%s/";
    char *buff;
    int isIP = 0;
    int x;
    if (connData->conn == NULL) {
        //Connection aborted. Clean up.
        return HTTPD_CGI_DONE;
    }
    if (connData->hostName == NULL) {
        //httpd_printf("Huh? No hostname.\n");
        return HTTPD_CGI_NOTFOUND;
    }

    //Quick and dirty code to see if host is an IP
    if (strlen(connData->hostName) > 8) {
        isIP = 1;
        for (x = 0; x < strlen(connData->hostName); x++) {
            if (connData->hostName[x] != '.' && (connData->hostName[x] < '0' || connData->hostName[x] > '9')) isIP = 0;
        }
    }
    if (isIP) return HTTPD_CGI_NOTFOUND;
    //Check hostname; pass on if the same
    if (strcmp(connData->hostName, (char*)connData->cgiArg) == 0) return HTTPD_CGI_NOTFOUND;
    //Not the same. Redirect to real hostname.
    buff = malloc(strlen((char*)connData->cgiArg) + sizeof(hostFmt));
    sprintf(buff, hostFmt, (char*)connData->cgiArg);
    //httpd_printf("Redirecting to hostname url %s\n", buff);
    httpdRedirect(connData, buff);
    free(buff);
    return HTTPD_CGI_DONE;
}


//Same as above, but will only redirect clients with an IP that is in the range of
//the SoftAP interface. This should preclude clients connected to the STA interface
//to be redirected to nowhere.
int  cgiRedirectApClientToHostname(HttpdConnData *connData) {
#ifndef FREERTOS
    uint32_t *remadr;
    tcpip_adapter_ip_info_t apip;

    wifi_mode_t x;
    esp_wifi_get_mode(&x);
    //Check if we have an softap interface; bail out if not
    if (x != 2 && x != 3) return HTTPD_CGI_NOTFOUND;
    remadr = (uint32_t *)connData->remote_ip;
  /// not used.  wifi_get_ip_info(WIFI_IF_AP, &apip);
    if ((*remadr & apip.netmask.addr) == (apip.ip.addr & apip.netmask.addr)) {
        return cgiRedirectToHostname(connData);
    } else {
        return HTTPD_CGI_NOTFOUND;
    }
#else
    return HTTPD_CGI_NOTFOUND;
#endif
}


//Add data to the send buffer. len is the length of the data. If len is -1
//the data is seen as a C-string.
//Returns 1 for success, 0 for out-of-memory.
int  httpdSend(HttpdConnData *conn, const char *data, int len) {
    if (conn->conn == NULL) return 0;
    if (len < 0) len = strlen(data);
    if (len == 0) return 0;
    if (conn->priv->flags & HFL_CHUNKED && conn->priv->flags & HFL_SENDINGBODY && conn->priv->chunkHdr == NULL) {
        if (conn->priv->sendBuffLen + len + 6 > MAX_SENDBUFF_LEN) return 0;
        //Prefix with chunk header
        conn->priv->chunkHdr = &conn->priv->sendBuff[conn->priv->sendBuffLen];
        strcpy(conn->priv->chunkHdr, "0000\r\n");
        conn->priv->sendBuffLen += 6;
    }
    if (conn->priv->sendBuffLen + len > MAX_SENDBUFF_LEN) return 0;
    memcpy(conn->priv->sendBuff + conn->priv->sendBuffLen, data, len);
    conn->priv->sendBuffLen += len;
    return 1;
}

static char  httpdHexNibble(int val) {
    val &= 0xf;
    if (val < 10) return '0' + val;
    return 'A' + (val - 10);
}

//Function to send any data in conn->priv->sendBuff. Do not use in CGIs unless you know what you
//are doing! Also, if you do set conn->cgi to NULL to indicate the connection is closed, do it BEFORE
//calling this.
void  httpdFlushSendBuffer(HttpdConnData *conn) {
    int r, len;
    
    if (conn->conn == NULL) return;
    if (conn->priv->chunkHdr != NULL) {
        //We're sending chunked data, and the chunk needs fixing up.
        //Finish chunk with cr/lf
        httpdSend(conn, "\r\n", 2);
        //Calculate length of chunk
        len = ((&conn->priv->sendBuff[conn->priv->sendBuffLen]) - conn->priv->chunkHdr) - 8;
        //Fix up chunk header to correct value
        conn->priv->chunkHdr[0] = httpdHexNibble(len >> 12);
        conn->priv->chunkHdr[1] = httpdHexNibble(len >> 8);
        conn->priv->chunkHdr[2] = httpdHexNibble(len >> 4);
        conn->priv->chunkHdr[3] = httpdHexNibble(len >> 0);
        //Reset chunk hdr for next call
        conn->priv->chunkHdr = NULL;
    }
    
    if (conn->priv->flags & HFL_CHUNKED && conn->priv->flags & HFL_SENDINGBODY && conn->cgi == NULL) {
        //End chunk
        strcpy(&conn->priv->sendBuff[conn->priv->sendBuffLen], "0\r\n\r\n");
        conn->priv->sendBuffLen += 5;
    }
    
//    httpd_printf("Server.line %d %x %p\n", __LINE__, conn->priv->flags & HFL_ONLYRESPONSEOK, conn->cgi);
    if (conn->priv->flags & HFL_ONLYRESPONSEOK && conn->cgi == NULL) {
        //End chunk
        strcpy(&conn->priv->sendBuff[0], "HTTP/1.0 200 OK\r\nContent-Length: 0\r\nServer: lwIP/1.4.0\r\n\n");
        conn->priv->sendBuffLen = strlen(conn->priv->sendBuff);        
    }
    
    httpd_printf("httpd.line %d %x %p\n", __LINE__, conn->priv->flags & HFL_ONLYRESPONSEOK, conn->cgi);
    if (conn->priv->flags & HFL_ONLYRESPONSEOK && conn->cgi == NULL) {
        //End chunk
        strcpy(&conn->priv->sendBuff[0], "HTTP/1.0 200 OK\r\nContent-Length: 0\r\nServer: lwIP/1.4.0\r\n\n");
        conn->priv->sendBuffLen = strlen(conn->priv->sendBuff);
    }
    
    if (conn->priv->sendBuffLen != 0) {
        r = httpdPlatSendData(conn->conn, conn->priv->sendBuff, conn->priv->sendBuffLen);        
        if (!r) {
            //Can't send this for some reason. Dump packet in backlog, we can send it later.
            if (conn->priv->sendBacklogSize + conn->priv->sendBuffLen > MAX_BACKLOG_SIZE) {
                //httpd_printf("Httpd: Backlog: Exceeded max backlog size, dropped %d bytes instead of sending them.\n", conn->priv->sendBuffLen);
                conn->priv->sendBuffLen = 0;
                return;
            }
            HttpSendBacklogItem *i = malloc(sizeof(HttpSendBacklogItem) + conn->priv->sendBuffLen);
            if (i == NULL) {
                //httpd_printf("Httpd: Backlog: malloc failed, out of memory!\n");
                return;
            }
            memcpy(i->data, conn->priv->sendBuff, conn->priv->sendBuffLen);
            i->len = conn->priv->sendBuffLen;
            i->next = NULL;
            if (conn->priv->sendBacklog == NULL) {
                conn->priv->sendBacklog = i;
            } else {
                HttpSendBacklogItem *e = conn->priv->sendBacklog;
                while (e->next != NULL) e = e->next;
                e->next = i;
            }
            conn->priv->sendBacklogSize += conn->priv->sendBuffLen;
        }
        conn->priv->sendBuffLen = 0;
        conn->priv->flags = (conn->priv->flags & (~HFL_ONLYRESPONSEOK));
    }
}

void  httpdCgiIsDone(HttpdConnData *conn) {
    conn->cgi = NULL; //no need to call this anymore
    if ((conn->priv->flags & HFL_CHUNKED) 
        /*|| (conn->priv->flags &HFL_ONLYRESPONSEOK)*/) {
        //httpd_printf("Pool slot %d is done. Cleaning up for next req\n", conn->slot);
        httpdFlushSendBuffer(conn);
        //Note: Do not clean up sendBacklog, it may still contain data at this point.
        conn->priv->headPos = 0;
        conn->post->len = -1;
        conn->priv->flags = 0;
        if (conn->post->buff) free(conn->post->buff);
        conn->post->buff = NULL;
        conn->post->buffLen = 0;
        conn->post->received = 0;
        conn->hostName = NULL;
    } else if (HFL_ONLYRESPONSEOK != (conn->priv->flags & HFL_ONLYRESPONSEOK)) {
        //Cannot re-use this connection. Mark to get it killed after all data is sent.
        conn->priv->flags |= HFL_DISCONAFTERSENT;        
    }
}

//Callback called when the data on a socket has been successfully
//sent.
void  httpdSentCb(ConnTypePtr rconn, char *remIp, int remPort) {
    int r;
    HttpdConnData *conn = httpdFindConnData(rconn, remIp, remPort);
    char *sendBuff;

    if (conn == NULL) return;

    if (conn->priv->sendBacklog != NULL) {
        //We have some backlog to send first.
        HttpSendBacklogItem *next = conn->priv->sendBacklog->next;
        httpdPlatSendData(conn->conn, conn->priv->sendBacklog->data, conn->priv->sendBacklog->len);
        conn->priv->sendBacklogSize -= conn->priv->sendBacklog->len;
        free(conn->priv->sendBacklog);
        conn->priv->sendBacklog = next;
        return;
    }

    if (conn->priv->flags & HFL_DISCONAFTERSENT) { //Marked for destruction?
        //httpd_printf("Pool slot %d is done. Closing.\n", conn->slot);
        httpdPlatDisconnect(conn->conn);
        return; //No need to call httpdFlushSendBuffer.
    }

    //If we don't have a CGI function, there's nothing to do but wait for something from the client.
    if (conn->cgi == NULL) return;

    sendBuff = malloc(MAX_SENDBUFF_LEN);
    conn->priv->sendBuff = sendBuff;
    conn->priv->sendBuffLen = 0;
    r = conn->cgi(conn); //Execute cgi fn.
    if (r == HTTPD_CGI_DONE) {
        httpdCgiIsDone(conn);
    }
    if (r == HTTPD_CGI_NOTFOUND || r == HTTPD_CGI_AUTHENTICATED) {
        //httpd_printf("ERROR! CGI fn returns code %d after sending data! Bad CGI!\n", r);
        httpdCgiIsDone(conn);
    }
    httpdFlushSendBuffer(conn);
    free(sendBuff);
}

//This is called when the headers have been received and the connection is ready to send
//the result headers and data.
//We need to find the CGI function to call, call it, and dependent on what it returns either
//find the next cgi function, wait till the cgi data is sent or close up the connection.
static void  httpdProcessRequest(HttpdConnData *conn) {
    int r;
    int i = 0;
    if (conn->url == NULL) {
        //httpd_printf("WtF? url = NULL\n");
        return; //Shouldn't happen
    }
    //See if we can find a CGI that's happy to handle the request.
    while (1) {
        //Look up URL in the built-in URL table.
        while (builtInUrls[i].url != NULL) {
            int match = 0;
            //See if there's a literal match
            if (strcmp(builtInUrls[i].url, conn->url) == 0) match = 1;
            //See if there's a wildcard match
            if (builtInUrls[i].url[strlen(builtInUrls[i].url) - 1] == '*' &&
                    strncmp(builtInUrls[i].url, conn->url, strlen(builtInUrls[i].url) - 1) == 0) match = 1;
            if (match) {
                if (strcmp(conn->getArgs, "command=soundbox") == 0) {
                    conn->cgiData = NULL;
                    conn->cgi = builtInUrls[i].cgiCb;
                    conn->cgiArg = builtInUrls[i].cgiArg;
                    conn->priv->flags |= HFL_ONLYRESPONSEOK;
                    
                    httpd_printf("Soundbox request is right.%d\n", __LINE__);
                    break;
                }
            }
            i++;
        }
        if (builtInUrls[i].url == NULL) {
            //Drat, we're at the end of the URL table. This usually shouldn't happen. Well, just
            //generate a built-in 404 to handle this.
            httpd_printf("%s not found. 404!\n", conn->url);
            conn->cgi = cgiNotFound;
        }

        //Okay, we have a CGI function that matches the URL. See if it wants to handle the
        //particular URL we're supposed to handle.
        
        r = conn->cgi(conn);
        if (r == HTTPD_CGI_MORE) {
            //Yep, it's happy to do so and has more data to send.
            if (conn->recvHdl) {
                //Seems the CGI is planning to do some long-term communications with the socket.
                //Disable the timeout on it, so we won't run into that.
                httpdPlatDisableTimeout(conn->conn);
            }
            httpdFlushSendBuffer(conn);
            return;
        } else if (r == HTTPD_CGI_DONE) {
            //Yep, it's happy to do so and already is done sending data.
            httpdCgiIsDone(conn);
            return;
        } else if (r == HTTPD_CGI_NOTFOUND || r == HTTPD_CGI_AUTHENTICATED) {
            //URL doesn't want to handle the request: either the data isn't found or there's no
            //need to generate a login screen.
            i++; //look at next url the next iteration of the loop.
        }
    }
}

//Parse a line of header data and modify the connection data accordingly.
static void  httpdParseHeader(char *h, HttpdConnData *conn) {
    int i;
    char firstLine = 0;

    if (strncmp(h, "GET ", 4) == 0) {
        conn->requestType = HTTPD_METHOD_GET;
        firstLine = 1;
    } else if (strncmp(h, "PUT ", 4) == 0) {
        conn->requestType = HTTPD_METHOD_PUT;
        firstLine = 1;
    } else if (strncmp(h, "Host:", 5) == 0) {
        i = 5;
        while (h[i] == ' ') i++;
        conn->hostName = &h[i];
    } else if (strncmp(h, "POST ", 5) == 0) {
        conn->requestType = HTTPD_METHOD_POST;
        firstLine = 1;
    }

    if (firstLine) {
        char *e;

        //Skip past the space after POST/GET
        i = 0;
        while (h[i] != ' ') i++;
        conn->url = h + i + 1;

        //Figure out end of url.
        e = (char*)strstr(conn->url, " ");
        if (e == NULL) return; //wtf?
        *e = 0; //terminate url part
        e++; //Skip to protocol indicator
        while (*e == ' ') e++; //Skip spaces.
        //If HTTP/1.1, note that and set chunked encoding
        if (strcasecmp(e, "HTTP/1.1") == 0) conn->priv->flags |= HFL_HTTP11 | HFL_CHUNKED;

        // httpd_printf("URL = %s\n", conn->url);
        //Parse out the URL part before the GET parameters.
        conn->getArgs = (char*)strstr(conn->url, "?");
        if (conn->getArgs != 0) {
            *conn->getArgs = 0;
            conn->getArgs++;
            //httpd_printf("GET args = %s\n", conn->getArgs);
        } else {
            conn->getArgs = NULL;
        }
    } else if (strncmp(h, "Connection:", 11) == 0) {
        i = 11;
        //Skip trailing spaces
        while (h[i] == ' ') i++;
        if (strncmp(&h[i], "close", 5) == 0) conn->priv->flags &= ~HFL_CHUNKED; //Don't use chunked conn
    } else if (strncmp(h, "Content-Length:", 15) == 0) {
        i = 15;
        //Skip trailing spaces
        while (h[i] == ' ') i++;
        //Get POST data length
        conn->post->len = atoi(h + i);

        // Allocate the buffer
        if (conn->post->len > MAX_POST) {
            // we'll stream this in in chunks
            conn->post->buffSize = MAX_POST;
        } else {
            conn->post->buffSize = conn->post->len;
        }
        //httpd_printf("Mallocced buffer for %d + 1 bytes of post data.\n", conn->post->buffSize);
        conn->post->buff = (char*)malloc(conn->post->buffSize + 1);
        conn->post->buffLen = 0;
    } else if (strncmp(h, "Content-Type: ", 14) == 0) {
        if (strstr(h, "multipart/form-data")) {
            // It's multipart form data so let's pull out the boundary for future use
            char *b;
            if ((b = strstr(h, "boundary=")) != NULL) {
                conn->post->multipartBoundary = b + 7; // move the pointer 2 chars before boundary then fill them with dashes
                conn->post->multipartBoundary[0] = '-';
                conn->post->multipartBoundary[1] = '-';
                //httpd_printf("boundary = %s\n", conn->post->multipartBoundary);
            }
        }
    }
}


//Callback called when there's data available on a socket.
void httpdRecvCb(ConnTypePtr rconn, char *remIp, int remPort, char *data, unsigned short len) {
    int x, r;
    char *p, *e;
    char *sendBuff = NULL;
    HttpdConnData *conn = httpdFindConnData(rconn, remIp, remPort);
    if (conn == NULL) return;
    if (conn->priv->sendBuff) return;  // the connection is busy

    sendBuff = (char *)malloc(MAX_SENDBUFF_LEN);
    if (!sendBuff)
        return;
//    ENTER_CRITICAL();
    memset(sendBuff, 0, MAX_SENDBUFF_LEN);
    conn->priv->sendBuff = sendBuff;
    conn->priv->sendBuffLen = 0;
//    ESP_MFI_EXIT_CRITICAL();

    //This is slightly evil/dirty: we abuse conn->post->len as a state variable for where in the http communications we are:
    //<0 (-1): Post len unknown because we're still receiving headers
    //==0: No post data
    //>0: Need to receive post data
    //ToDo: See if we can use something more elegant for this.
    for (x = 0; x < len; x++) {
        if (conn->post->len < 0) {
            //This byte is a header byte.
            if (data[x] == '\n') {
                //Compatibility with clients that send \n only: fake a \r in front of this.
                if (conn->priv->headPos != 0 && conn->priv->head[conn->priv->headPos - 1] != '\r') {
                    conn->priv->head[conn->priv->headPos++] = '\r';
                }
            }
            if (conn->priv->headPos != MAX_HEAD_LEN) conn->priv->head[conn->priv->headPos++] = data[x];
            conn->priv->head[conn->priv->headPos] = 0;
            //Scan for /r/n/r/n. Receiving this indicate the headers end.
            if (data[x] == '\n' && (char *)strstr(conn->priv->head, "\r\n\r\n") != NULL) {
                //Indicate we're done with the headers.
                conn->post->len = 0;
                //Reset url data
                conn->url = NULL;
                //Iterate over all received headers and parse them.
                p = conn->priv->head;                
                while (p < (&conn->priv->head[conn->priv->headPos - 4])) {
                    e = (char *)strstr(p, "\r\n"); //Find end of header line
                    if (e == NULL) break;           //Shouldn't happen.
                    e[0] = 0;                       //Zero-terminate header
                    httpdParseHeader(p, conn);  //and parse it.
                    p = e + 2;                      //Skip /r/n (now /0/n)
                }
                
//                httpd_printf("find head ok.line %d\n", __LINE__);
                //If we don't need to receive post data, we can send the response now.
                if (conn->post->len == 0) {
                    httpdProcessRequest(conn);
                }
            }
        } else if (conn->post->len != 0) {
            //This byte is a POST byte.
            
            conn->post->buff[conn->post->buffLen++] = data[x];
            conn->post->received++;
            conn->hostName = NULL;
            
            if (conn->post->buffLen >= conn->post->buffSize || conn->post->received == conn->post->len) {
                //Received a chunk of post data
                conn->post->buff[conn->post->buffLen] = 0; //zero-terminate, in case the cgi handler knows it can use strings
                //Send the response.
                
                if (conn->cgi) {
                    r = conn->cgi(conn);
                    if (r == HTTPD_CGI_DONE) {
                        httpdCgiIsDone(conn);
                    }
                } else {
                    //No CGI fn set yet: probably first call. Allow httpdProcessRequest to choose CGI and
                    //call it the first time.
                    httpdProcessRequest(conn);
                }
                conn->post->buffLen = 0;
            }
        } else {
            //Let cgi handle data if it registered a recvHdl callback. If not, ignore.
            if (conn->recvHdl) {
                r = conn->recvHdl(conn, data + x, len - x);
                if (r == HTTPD_CGI_DONE) {
//                    httpd_printf("Recvhdl returned DONE\n");
                    httpdCgiIsDone(conn);
                    //We assume the recvhdlr has sent something; we'll kill the sock in the sent callback.
                }
                break; //ignore rest of data, recvhdl has parsed it.
            } else {
                httpd_printf("Eh? Got unexpected data from client. %s\n", data);
            }
        }
    }
    if (conn->conn) httpdFlushSendBuffer(conn);
//    ESP_MFI_ENTER_CRITICAL();
    conn->priv->sendBuff = NULL;
    conn->priv->sendBuffLen = 0;
//    ESP_MFI_EXIT_CRITICAL();
    free(sendBuff);
}

//The platform layer should ALWAYS call this function, regardless if the connection is closed by the server
//or by the client.
void  httpdDisconCb(ConnTypePtr rconn, char *remIp, int remPort) {
    HttpdConnData *hconn = httpdFindConnData(rconn, remIp, remPort);
    if (hconn == NULL) return;
    httpd_printf("Pool slot %d: socket closed.\n", hconn->slot);
    hconn->conn = NULL; //indicate cgi the connection is gone
    if (hconn->cgi) hconn->cgi(hconn); //Execute cgi fn if needed
    httpdRetireConn(hconn);
}


int  httpdConnectCb(ConnTypePtr conn, char *remIp, int remPort) {
    int i;
    //Find empty conndata in pool
    for (i = 0; i < HTTPD_MAX_CONNECTIONS; i++) if (connData[i] == NULL) break;
    //httpd_printf("Conn req from  %d.%d.%d.%d:%d, using pool slot %d\n", remIp[0]&0xff, remIp[1]&0xff, remIp[2]&0xff, remIp[3]&0xff, remPort, i);
    if (i == HTTPD_MAX_CONNECTIONS) {
        //httpd_printf("Aiee, conn pool overflow!\n");
        return 0;
    }
    connData[i] = malloc(sizeof(HttpdConnData));
    memset(connData[i], 0, sizeof(HttpdConnData));
    connData[i]->priv = malloc(sizeof(HttpdPriv));
    memset(connData[i]->priv, 0, sizeof(HttpdPriv));
    connData[i]->conn = conn;
    connData[i]->slot = i;
    connData[i]->priv->headPos = 0;
    connData[i]->post = malloc(sizeof(HttpdPostData));
    memset(connData[i]->post, 0, sizeof(HttpdPostData));
    connData[i]->post->buff = NULL;
    connData[i]->post->buffLen = 0;
    connData[i]->post->received = 0;
    connData[i]->post->len = -1;
    connData[i]->hostName = NULL;
    connData[i]->remote_port = remPort;
    connData[i]->priv->sendBacklog = NULL;
    connData[i]->priv->sendBacklogSize = 0;
    memcpy(connData[i]->remote_ip, remIp, 4);

    return 1;
}

//Httpd initialization routine. Call this to kick off webserver functionality.
void  httpdInit(HttpdBuiltInUrl *fixedUrls, int port) {
    int i;

    for (i = 0; i < HTTPD_MAX_CONNECTIONS; i++) {
        connData[i] = NULL;
    }
    builtInUrls = fixedUrls;

    httpdPlatInit(port, HTTPD_MAX_CONNECTIONS);
}
