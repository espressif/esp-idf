#include <string.h>
#include "esp_types.h"
#include "stdlib.h"
#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/sys.h"
#include "errno.h"
#include "audio_log.h"
#include "inc/DownloadTask.h"
#include "inc/M3u8.h"
#include "EspAudioCom.h"
#include "EspAudio.h"

#include "inc/MediaDecoderTask.h"
#include "Storage.h"
#include "MediaHal.h"
#include "Utility.h"

#define PRIO_DOWNLOAD               19
#define DEFAULT_AAC_SEQ             0xFFFFFFFF
#define REQUEST_BUF_LEN             1024
#define RECV_BUF_LEN                1500


#if 1
#define DEBUGHTTP   printf
#else
#define DEBUGHTTP
#endif


uint32_t fileTaskState = 0;
uint32_t m3u8TaskState = 0;
void m3u8DownloadTask(void *pvParameters);
void fileDownloadTask(void *pvParameters);

typedef struct {
    int    status;           // 1:downloading; 2:finished
    char   *reqBuf;
    char   *recvBuf;
    int    blockQuit;
    int    receivedSize;
    uint32_t remaindSize;
    uint32_t downloadedSize;
    uint32_t contentTotalSize;
    UriSrcAttr uri;
} HttpObj;


typedef struct {
    TaskState state;
    pthread_t thread;
    int threadQuit;
} DownloadInstance;


static HttpObj localHttp;
//static mutex_t downloadMutex;
static UrlInfo actLiveUrl;

DownloadInstance dlInstance = { 0 };

typedef void (*Callback_Recv)(HttpObj *obj);

int checkIsReqOk(char *buf)
{
    char *l = strstr(buf, "HTTP/1.1 2");
    if (NULL == l) {
        l = strstr(buf, "HTTP/1.0 2");
        if (NULL == l) {
            l = strstr(buf, "HTTP/1.0 3");
            if (NULL == l) {
                return -1;
            }
            return 1;
        }
    }
    return 0;
}
int getLocation (const char *buf, char* dest)
{
    // Location: http://119.90.25.49/live.xmcdn.com/192.168.3.134/live/38/24/161118_174054_588.aac
    char *l = strstr(buf, "Location:");
    if (NULL == l) {
        LOGE("Can't find Location:%s", buf);
        return -1;
    }
    l += 10;
    int i1 = 0;
    if (l != NULL) {
        while (*l != '\r') {
            dest[i1++] = *(l++);
            if (i1 > ESP_URI_LEN) {
                break;
            }
        }
    } else {
        LOGE("Can't find Location:%s", buf);
        return -1;
    }
    return 0;
}

int getFileLength(char *buf)
{
    char *l = strstr(buf, "Content-Length:");
    if (NULL == l) {
        LOGE("Can't find length:%s", buf);
        return 0;
    }
    l += 16;
    char len[32] = {0};
    int i1 = 0;
    if (l != NULL) {
        while (*l != '\r') {
            len[i1++] = *(l++);
        }
    } else {
        LOGE("Can't find length:%s", buf);
        return 0;
    }
    return atoi(len);

}


void ReqHead(char* outReqBuf, const char *url, char* host)
{
    LOGD("http HEAD requst\r\n");
    sprintf(outReqBuf, "HEAD %s HTTP/1.1\r\n"
            "Accept:*/*\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.80 Safari/537.36\r\n"
            "Host: %s\r\n"
            "Connection:Keep-Alive\r\n"
            "Cache-Control: no-cache\r\n"
            "\r\n\r\n", url, host);
    DEBUGHTTP(outReqBuf);
}


void ReqGetM3u8(char* outReqBuf, const char *url, char* host)
{
    LOGD("http GET requst:");
    if ((NULL == url) || (NULL == host)) {
        LOGE("NULL = url or NULL = host");
        return;
    }
    sprintf(outReqBuf, "GET %s HTTP/1.1\r\n"
            "Accept:*/*\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.80 Safari/537.36\r\n"
            "Host: %s\r\n"
            "Connection: Close\r\n"     // Keep-Alive
            "Cache-Control: no-cache\r\n"
            "\r\n\r\n", url, host);
    DEBUGHTTP(outReqBuf);
}


void ReqDataPre(char* outReqBuf, const char *url, char* host, uint32_t startLen, uint32_t endLen)
{
    if (0 != endLen) {
        sprintf(outReqBuf, "GET %s HTTP/1.1\r\n"
                "Accept:*/*\r\n"
                "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.80 Safari/537.36\r\n"
                "Host: %s\r\n"
                "Proxy-Connection: Keep-Alive\r\n"
                "Cache-Control: no-cache\r\n"
                "Range: bytes=%d-%d\r\n\r\n", url, host, startLen, endLen - 1);
    } else {
        sprintf(outReqBuf, "GET %s HTTP/1.1\r\n"
                "Accept:*/*\r\n"
                "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.80 Safari/537.36\r\n"
                "Host: %s\r\n"
                "Proxy-Connection: Keep-Alive\r\n"
                "Cache-Control: no-cache\r\n\r\n", url, host);
    }

    LOGD("http GET requst-%d:", strlen(outReqBuf));
    //DEBUGHTTP(outReqBuf);
}

int getIpForHost(const char *host, struct sockaddr_in *ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    char str[200] ;

    he = gethostbyname(host);
    if (he == NULL) return 0;
    addr_list = (struct in_addr **)he->h_addr_list;
    if (addr_list[0] == NULL) return 0;
    ip->sin_family = AF_INET;
    memcpy(&ip->sin_addr, addr_list[0], sizeof(ip->sin_addr));
    return 1;
}

int splitHostUrl(const char* url, char* host, uint16_t *port, char **path)
{
    if ((NULL == url) || (NULL == host)) {
        LOGE("ERR:url=NULL or Host=NULL\n");
        return -1;
    }
    char* temp1;
    char* source = (char*)url;
    char portBuf[10] = {0};
    while (url) {
        source = strstr(source, "//");
        if (url == NULL) {
            LOGE("ERR:strchr=NULL\n");
            return -1;
        }
        source += 2;
        temp1 = strchr(source, ':');
        if (temp1 != NULL) {
            // try to find '/'
            strncpy(host, source, temp1 - source);
            temp1 += 1;
            source = strchr(source, '/');
            if (source == NULL) {
                LOGE("ERR:strchr=NULL\n");
                return 0;
            }
            strncpy(portBuf, temp1, source - temp1);
            *port = strtoul(portBuf, 0, 10);
            *path = source;
            LOGD("\nUsed Host[%s] Port[%d],Path:%s\n", host, *port, *path);
        } else {
            // try to find port
            temp1 = strchr(source, '/');
            if (temp1 == NULL) {
                LOGE("ERR:strchr=NULL\n");
                return -1;
            }
            strncpy(host, source, temp1 - source);
            *port = 80;
            *path = temp1;
            LOGD("\nUsed Host[%s] Port[%d],Path:%s\n", host, *port, *path);
        }
        return 0;
    }
    return -1;
}

int connectToServer(const char* actUrl, const char* hostName, uint16_t port, int timeOut)
{
    struct sockaddr_in remote_ip;
    int sock = -1;
    int nNetTimeout = timeOut; // socket timeout 1 Sec
    int retryCnt = 3;
    while (retryCnt--) {
        bzero(&remote_ip, sizeof(struct sockaddr_in));
        if (1 != inet_aton(hostName, &remote_ip.sin_addr)) {
            if (!getIpForHost(hostName, &remote_ip)) {
                vTaskDelay(1000 / portTICK_RATE_MS);
                LOGE("getIpForHost failed");
                continue;
            }
        }
        remote_ip.sin_family = AF_INET;
        int sock = socket(PF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            LOGE("socket failed errno=%d", errno);
            continue;
        }
//        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
        struct timeval tv_out;
         tv_out.tv_sec = 20; //Wait 20 seconds.
         tv_out.tv_usec = 0;
         setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,&tv_out, sizeof(tv_out));

        remote_ip.sin_port = htons(port);
        LOGD("[sock=%d],connecting to server IP:%s,Port:%d...",
             sock, ipaddr_ntoa((const ip_addr_t*)&remote_ip.sin_addr.s_addr), port);
        if (connect(sock, (struct sockaddr *)(&remote_ip), sizeof(struct sockaddr)) != 0) {
            close(sock);
            LOGE("Conn err.\n");
            vTaskDelay(1000 / portTICK_RATE_MS);
            continue;
        }
        return sock;
    }
    LOGE("Lost host,connect and getIpForHost timeout");
    return -2;
}

void resetDownloadAttr(char *src, uint32_t *type)
{
    localHttp.status = 0;
    if (localHttp.blockQuit < 0) {        
        localHttp.blockQuit = 0;
    }
    localHttp.receivedSize = 0;
    localHttp.remaindSize = 0;
    localHttp.downloadedSize = 0;
    localHttp.uri.srcType = *type;
    localHttp.contentTotalSize = 0;
    memset(localHttp.uri.str, 0, ESP_URI_LEN);
    strncpy(localHttp.uri.str, src, strlen(src));
}

void OnFileDataCB(HttpObj *obj)
{
    int ret;

    do {
        ret = audioBufWrite(obj->reqBuf, obj->receivedSize, 100);
        if (ret) {
            obj->remaindSize -= obj->receivedSize;
            obj->downloadedSize += obj->receivedSize;
//            uint32_t rd, wr;
//            audioBufPosGet(&rd, &wr);
//            LOGD("rd:%x,wr:%x,downloaded=%d, remaind=%d, total=%d, recvSize=%d",
//                rd,wr,obj->downloadedSize, obj->remaindSize, obj->contentTotalSize, obj->receivedSize);
            break;
        } else {
            LOGE("audioBufWrite failed ret=%d. rcev=%d,remaind=%d, d=%d", ret,
                 obj->receivedSize, obj->remaindSize, obj->downloadedSize);
            break;
        }
    } while (0 == obj->blockQuit);
}

void OnM3u8DataCB(HttpObj *obj)
{
    obj->remaindSize -= obj->receivedSize;
    obj->downloadedSize += obj->receivedSize;
//    LOGD(":\n%s", obj->reqBuf);
}

char *GetValidUrl(M3u8Info *pActM3u8, M3u8Seq *actSeq)
{
    char *retUrl = NULL;
    if ((NULL == actSeq)
        || (NULL == pActM3u8)) {
        LOGE("pActM3u8 or actSeq is NULL");
        return retUrl;
    }
    int validM3u8 = -1;
    for (uint8_t i = 0; i < 3; i++) {
//        LOGD("M3u8 AAC URL,%dth,%s\n", i, pActM3u8->segList[i].url);
        if ((actSeq->sub == DEFAULT_AAC_SEQ)
            || (1 == m3u8CheckValid(&pActM3u8->segList[i].seq, (const M3u8Seq*)actSeq))) {
            validM3u8 = 1;
            retUrl = pActM3u8->segList[i].url;
            memcpy(actSeq, &pActM3u8->segList[i].seq, sizeof(pActM3u8->segList[i].seq));
            pActM3u8->segList[i].seq.sub = 0;
            LOGD("Get one valid AAC URL,%dth,seq=%x,%s\n", i, actSeq->sub, retUrl);
            if ((0 == actSeq->sub)
                || (retUrl[0] == '\0')) {
                retUrl = NULL;
            }
            return retUrl;
        } else {
//            LOGD("Get one valid AAC URL,%dth,seq=%x,%s\n", i, pActM3u8->segList[i].seq.sub, pActM3u8->segList[i].url);
            pActM3u8->segList[i].seq.sub = 0;
        }
    }
    if (validM3u8 != 1) {
        LOGD("Can not found valid aac url, curtAacSeq=%x", actSeq->sub);
    }
    return retUrl;

}

int httpDownloading(HttpObj *obj, Callback_Recv recvCB)
{
    if ((NULL == obj)
        || (NULL == obj->uri.str)
        || (NULL == obj->reqBuf)
        || (NULL == obj->recvBuf)
        || (0 == strlen(obj->uri.str))) {
        LOGE("Pointer is NULL,obj:%x,URL:%x reqbuf:%x recvBuf:%x,len:%d,blockQuit:%d",
             obj, obj->uri.str, obj->reqBuf, obj->recvBuf, strlen(obj->uri.str), obj->blockQuit);
        return -1;
    }
    char hostName[32] = { 0 };
    int actSocket = -1;
    uint32_t contrlDat = 0;
    uint8_t modifyProFlag = 0;
    int cnt = 0;
    int n = 0;
    LOGD("\n>>>>>Downloading start:%s,0x%x,remaind=%d,downloaded=%d,total=%d,fill=%d",
         obj->uri.str, obj->uri.str, obj->remaindSize, obj->downloadedSize, obj->contentTotalSize, audioBufFillGet());
    obj->status = 1;
    uint16_t ipPort = 80;
    char *uriPath = NULL;
    do {
        // Here will check exit event;

Retry:  LOGD("retry new url size=%d", obj->remaindSize);
        memset(hostName, 0, sizeof(hostName));
        if (0 != splitHostUrl(obj->uri.str, hostName, &ipPort, &uriPath)) {
            LOGE("splitHostUrl failed:%s", obj->uri.str);
            continue;
        }
        LOGD("Hostname:%s", hostName);
        if ((actSocket = connectToServer((const char*)obj->uri.str, hostName, ipPort, 10000)) >= 0) {
            char * datPos = NULL;
            bzero(obj->reqBuf, REQUEST_BUF_LEN);
            ReqDataPre(obj->reqBuf, uriPath, hostName, obj->downloadedSize, obj->contentTotalSize);
            LOGD("Hostname:%s++++++++", hostName);
            write(actSocket, obj->reqBuf, strlen(obj->reqBuf));
            LOGD("Hostname:%s--------", hostName);
            bzero(obj->recvBuf, RECV_BUF_LEN);
            datPos = NULL;
            cnt = 0;
            do {
                bzero(obj->reqBuf, REQUEST_BUF_LEN);
                n = read(actSocket, obj->reqBuf, REQUEST_BUF_LEN);
                if ((n > 0) && (cnt <= RECV_BUF_LEN)) {
//                    DEBUGHTTP("Head is: %s", obj->reqBuf);
                    memcpy(obj->recvBuf + cnt, obj->reqBuf, n);
                    datPos = strstr(obj->recvBuf, "\r\n""""\r\n");
                    if (datPos) {
                        //DEBUGHTTP("Find CRLF CRLF ok");                        
//                        LOGD("Head is: %s", obj->recvBuf);
                    } else {
                        LOGE("Find CRLF CRLF failed,wait more data");
                    }
                }
                cnt += n;
            } while ((datPos == NULL) && (n > 0) && (cnt < RECV_BUF_LEN));
            if (n < 0) {
                close(actSocket);
                continue;
            }
            if (NULL == datPos)  {
//                datPos == strstr(obj->recvBuf, "\r\n""""\r\n");
                LOGD("Head is: %s", obj->recvBuf);

                if (NULL == datPos) {
                    close(actSocket);
                    LOGE("Can't Find CRLF CRLF,it's bad response,try again");
                    continue;
                }
            }
            int ret = checkIsReqOk(obj->recvBuf);
            if (ret < 0) {
                close(actSocket);
                LOGE("Recv a bad resp:%s", obj->recvBuf);
                continue;
            } else if (ret == 1) {
                close(actSocket);
                LOGW("Recv a 3xx resp:%s", obj->recvBuf);
                memset(obj->uri.str, 0, ESP_URI_LEN);
                if (getLocation((const char*)obj->recvBuf, obj->uri.str) < 0) {
                    LOGE("Find 3xx URL error");
                } else {
                    LOGW("Recv a 3xx resp:%s", obj->uri.str);
                }
                goto  Retry;
            }
            if (0 == obj->contentTotalSize) obj->contentTotalSize = getFileLength(obj->recvBuf);
            datPos += 4;
            cnt = cnt - (datPos - obj->recvBuf);
            if (0 == obj->remaindSize) {
                obj->remaindSize = obj->contentTotalSize;
            }
            // Save the tail data
            bzero(obj->reqBuf, REQUEST_BUF_LEN);
            memcpy(obj->reqBuf, datPos, cnt);

            LOGD("downloaded=%d, remaind=%d, total=%d, cnt=%d",
                 obj->downloadedSize, obj->remaindSize, obj->contentTotalSize, cnt);
            int writeComplete = 0;
            do {
                if (writeComplete) {
                    bzero(obj->reqBuf, REQUEST_BUF_LEN);
                    n = read(actSocket, obj->reqBuf, REQUEST_BUF_LEN);
                    cnt = n;
                    // DEBUGHTTP("recv,cnt:%d\n", cnt);
                }
                if (n > 0) {
                    if (cnt > 0) {
                        obj->receivedSize = cnt;
                        recvCB(obj);
                    }
                    cnt = 0;
                    writeComplete = 1;
                }
            } while ((n > 0) && (obj->remaindSize > 0) && (0 == obj->blockQuit));
            close(actSocket);
            LOGD("Sock[%d] has closed,n=%d,blockQuit=%d", actSocket, n, obj->blockQuit);
        } else {
            LOGE("Connect is error.");
        }
        LOGD("Connect closed,file size remaind=%d", obj->remaindSize);

    } while ((obj->remaindSize != 0) && (0 == obj->blockQuit));
    if (0 == obj->remaindSize) {
        obj->status = 2;
    } else {
        obj->status = 3;
    }
    LOGD("\n<<<<<Downloading finish:%s,remaind=%d,downloaded=%d,total=%d fill=%d status=%d\n",
         obj->uri.str, obj->remaindSize, obj->downloadedSize, obj->contentTotalSize,
         audioBufFillGet(), obj->status);
    return 0;
}

void fileDownloadTask(void *pvParameters)
{
    int n, cnt;
    DownloadInstance *downloadObj = (DownloadInstance *)pvParameters;
    LOGI("%s is running.%x %x", __func__, pvParameters, downloadObj->threadQuit);

/////////////////////////////
    M3u8Seq runningAacSeq = {0, 0, DEFAULT_AAC_SEQ};
    M3u8Info *pUsingM3u8 = NULL;
    uint32_t mediaSeq = DEFAULT_AAC_SEQ;

    downloadObj->threadQuit = 0;
    while (downloadObj->threadQuit == 0) {
        switch (downloadObj->state) {
        case TaskState_Ready:
            if ((NULL == actLiveUrl.str)
                || (actLiveUrl.str[0] == '\0')
                || (NULL == localHttp.uri.str )) {
                downloadObj->state  = TaskState_Idle;
                vTaskDelay(200 / portTICK_RATE_MS);
                LOGI("Downloading URL is NULL,downloading task paused");
                break;
            } else {
                LOGI("Download task is ready.%s type:%d", actLiveUrl.str, actLiveUrl.type);
            }
            if (InputSrcType_NetSpec == actLiveUrl.type) {
                resetDownloadAttr(actLiveUrl.str, &actLiveUrl.type);
                downloadObj->state = TaskState_Running;
                audioBufWrRest();
            } else if (InputSrcType_NetLiving ==  actLiveUrl.type) {
                resetDownloadAttr(actLiveUrl.str, &actLiveUrl.type);

                audioBufWrRest();
                mediaSeq = DEFAULT_AAC_SEQ;
                downloadObj->state = TaskState_Running;
                runningAacSeq.sub = DEFAULT_AAC_SEQ;
                if (pUsingM3u8) {
                    free(pUsingM3u8);
                    pUsingM3u8 = NULL;
                }
                pUsingM3u8 = calloc(1, sizeof(M3u8Info));
                if (pUsingM3u8 == NULL) {
                    LOGE("recvBuf malloc error");
                    break;
                }

            } else {
                LOGE("Http uri.srcType is not supported[%d]", localHttp.uri.srcType);
                downloadObj->state = TaskState_Idle;
                vTaskDelay(200 / portTICK_RATE_MS);
            }
            break;
        case TaskState_Pause:
            LOGI("TaskState_Pause,Total=%d, downloaded=%d,remaind=%d", localHttp.contentTotalSize,
                 localHttp.downloadedSize,
                 localHttp.remaindSize);
            downloadObj->state = TaskState_Idle;
            break;
        case TaskState_Resume:
            downloadObj->state = TaskState_Running;
            LOGI("TaskState_Resume,Total=%d, downloaded=%d,remaind=%d", localHttp.contentTotalSize,
                 localHttp.downloadedSize,
                 localHttp.remaindSize);
            break;
        case TaskState_Running:
            if (InputSrcType_NetSpec == localHttp.uri.srcType) {
                LOGI("Excute URL:%s", localHttp.uri.str);
                httpDownloading(&localHttp, OnFileDataCB);
                if (2 == localHttp.status) {
                    downloadObj->state = TaskState_Idle;
                    LOGI("Excute URL over, task will be idle.remind[%d]", localHttp.remaindSize);
                }
            } else if (InputSrcType_NetLiving == localHttp.uri.srcType) {
                resetDownloadAttr(actLiveUrl.str, &actLiveUrl.type);
                memset(localHttp.recvBuf, 0, RECV_BUF_LEN);
                memset(localHttp.reqBuf, 0, REQUEST_BUF_LEN);

                LOGD("Excute URL:%s, actLiveUrl.str=%x", localHttp.uri.str, actLiveUrl.str);
                httpDownloading(&localHttp, OnM3u8DataCB);

                M3u8Info *pActM3u8 = m3u8Parser(localHttp.reqBuf, pUsingM3u8);
                if (NULL == pActM3u8) {
                    LOGE("Parser m3u8 is error");
                    vTaskDelay(1000 / portTICK_RATE_MS);
                    break;
                }
                if ((pActM3u8->mediaSeq <= mediaSeq) && (mediaSeq != DEFAULT_AAC_SEQ)) {
                    LOGD("mediaSeq has not update,used=%d,new=%d", mediaSeq, pActM3u8->mediaSeq);
                    vTaskDelay(1000 / portTICK_RATE_MS);
                    break;
                }
                char *validUrl = GetValidUrl(pActM3u8, &runningAacSeq);

                LOGD("AAC URL,runningAacSeq:%x,%x,%x\n",
                     runningAacSeq.sub, runningAacSeq.main, runningAacSeq.date);
                if (NULL == validUrl) {
                    vTaskDelay(100 / portTICK_RATE_MS);
                    LOGE("uri.str is NULL,state=%d", downloadObj->state);
                    break;
                }
                DecoderObj decoder;
                mediaDecoderTaskInfoGet(&decoder);
                decoder.musicType = checkMediaType(validUrl);
                if (decoder.musicType >= EspAudioMeidaType_M3u) {
                    LOGE("%s,Music type[%d] is error", validUrl, decoder.musicType);
                    vTaskDelay(100 / portTICK_RATE_MS);
                    break;
                } else {
                    mediaDecoderTaskInfoSet(&decoder);
                }
                do {
                    resetDownloadAttr(validUrl, &actLiveUrl.type);
                    memset(localHttp.recvBuf, 0, RECV_BUF_LEN);
                    memset(localHttp.reqBuf, 0, REQUEST_BUF_LEN);

                    httpDownloading(&localHttp, OnFileDataCB);
                    validUrl = GetValidUrl(pActM3u8, &runningAacSeq);

                    LOGD("AAC URL,runningAacSeq:%x,%x,%x\n", runningAacSeq.sub, runningAacSeq.main, runningAacSeq.date);
                    if (NULL == validUrl) {
                        LOGD("uri.str is NULL,state=%d", downloadObj->state);
                        break;
                    }
                } while (validUrl != NULL);
                mediaSeq = pActM3u8->mediaSeq;
            } else {
                LOGE("Http uri.srcType is not supported[%d]", localHttp.uri.srcType);
                downloadObj->state = TaskState_Idle;
                vTaskDelay(200 / portTICK_RATE_MS);
            }

            break;
        case TaskState_Stoped:
            downloadObj->state = TaskState_Idle;
            break;
        case TaskState_Idle:
        default:
            vTaskDelay(250 / portTICK_RATE_MS);
            //  LOGD("blockQuit[%d] state[%d]", localHttp.blockQuit, downloadObj->state);
            break;
        }
    }
    downloadObj->threadQuit = -1;
    downloadObj->thread = NULL;
    downloadObj->state = TaskState_Stoped;
    if (pUsingM3u8) {
        free(pUsingM3u8);
        pUsingM3u8 = NULL;
    }
    LOGD("%s is ended...", __func__);
    vTaskDelete(NULL);
}

TaskState downloadTaskStateGet()
{
    return dlInstance.state;
}

int downloadTaskHTTPStateGet()
{
    return localHttp.status;
}

void downloadTaskDownloadSizeSet(int *size)
{
    if (NULL == size) {
        LOGE("Invalid para");
        return ;
    }
//    localHttp.blockQuit = 1;
    localHttp.downloadedSize = *size;
    downloadTaskResume();
}

void downloadTaskContentSizeGet(uint32_t *size)
{
    if (NULL == size) {
        LOGE("Invalid para");
        return ;
    }
    *size = localHttp.contentTotalSize;
}

void downloadTaskInfoSet(UriSrcAttr *uri)
{
    if (NULL == uri) {
        LOGE("Invalid para");
        return ;
    }
    actLiveUrl.type = uri->srcType;
    memset(actLiveUrl.str, 0 , ESP_URI_LEN);
    strncpy(actLiveUrl.str, uri->str, strlen(uri->str));
    localHttp.blockQuit = 0;
    LOGD("Set url ok:%s", actLiveUrl.str);
}

void downloadTaskPause()
{
    if (0 == dlInstance.thread) {
        dlInstance.state = TaskState_Unknown;
        LOGE("dlInstance.thread is NULL");
        return;
    }
    if (TaskState_Running == dlInstance.state) {
        localHttp.blockQuit = -1;
    }
    dlInstance.state = TaskState_Pause;

    LOGD("download task state=%d", dlInstance.state);
}

void downloadTaskResume()
{
    if (0 == dlInstance.thread) {
        dlInstance.state = TaskState_Unknown;
        LOGE("dlInstance.thread is NULL");
        return;
    }
    if (TaskState_Pause == dlInstance.state) {
        localHttp.blockQuit = 0;
    }
    dlInstance.state = TaskState_Resume;
    LOGD("download task state=%d", dlInstance.state);
}

void downloadTaskCreate(int taskType)
{
    if (localHttp.reqBuf)  {
        free(localHttp.reqBuf);
    }
    if (localHttp.recvBuf) {
        free(localHttp.recvBuf);
    }
    if (localHttp.uri.str) {
        free(localHttp.uri.str);
    }
    localHttp.reqBuf = calloc(1, REQUEST_BUF_LEN);
    if (NULL == localHttp.reqBuf) {
        LOGE("reqBuf malloc error");
        return;
    }
    localHttp.recvBuf = calloc(1, RECV_BUF_LEN);
    if (NULL == localHttp.recvBuf) {
        LOGE("recvBuf malloc error");
        free(localHttp.reqBuf);
        return;
    }
    localHttp.uri.str = calloc(1, ESP_URI_LEN);
    if (NULL == localHttp.uri.str) {
        LOGE("uri.str malloc error");
        free(localHttp.reqBuf);
        free(localHttp.recvBuf);
        return;
    }
//    mutex_init(&downloadMutex);
//    configASSERT(downloadMutex);
    xTaskCreate(fileDownloadTask, "fileDownloadTask", (6 * 1024), &dlInstance, PRIO_DOWNLOAD, &dlInstance.thread);
    configASSERT(dlInstance.thread);
    LOGD("download task state=%d", dlInstance.state);
}

int downloadTaskStart()
{
    if (0 == dlInstance.thread) {
        dlInstance.state = TaskState_Unknown;
        LOGE("dlInstance.thread is NULL");
        return -1;
    }
    if ((actLiveUrl.str == NULL)
        || (actLiveUrl.str[0] == '\0')) {
        dlInstance.state = TaskState_Idle;
        LOGE("uri.str task is NULL");
        return -1;
    }
    dlInstance.state = TaskState_Ready;
    LOGD("download task state=%d, %s, %d", dlInstance.state, actLiveUrl.str, localHttp.blockQuit);
    audioBufWrRest();

    return 0;
}

void downloadTaskStop(void *que)
{
    if (0 == dlInstance.thread) {
        dlInstance.state = TaskState_Unknown;
        LOGE("dlInstance.thread is NULL");
        return;
    }
    if (dlInstance.state == TaskState_Unknown) {
        LOGW("dlInstance.state is TaskState_Unknown");
        return;
    }
    localHttp.blockQuit = 1;
    dlInstance.state = TaskState_Stoped;
    configASSERT(que);
    xQueueSend(que, &localHttp.blockQuit, 0);

    LOGD("download task +++++ blockQuit=%d, state=%d", localHttp.blockQuit, dlInstance.state);
    while ((TaskState_Finished != dlInstance.state)
           && (TaskState_Idle != dlInstance.state)) {
        if ((TaskState_Idle == dlInstance.state)) {            
            localHttp.blockQuit = 0;
            break;
        }
        vTaskDelay(10 / portTICK_RATE_MS);
    }
    audioBufWrRest();
    xQueueReset(que);
    LOGD("download task ------ blockQuit=%d, state=%d", localHttp.blockQuit, dlInstance.state);
}

void downloadTaskDestroy()
{
    if (0 == dlInstance.thread) {
        dlInstance.state = TaskState_Unknown;
        LOGE("dlInstance.thread is NULL");
        return;
    }

    LOGD("download task +++++  blockQuit=%d, state=%d", localHttp.blockQuit, dlInstance.state);
    if (0 == dlInstance.threadQuit) {
        localHttp.blockQuit = 1;
        dlInstance.threadQuit = 1;
        dlInstance.state = TaskState_Stoped;
        while (1) {
            if ((dlInstance.threadQuit == -1)) {                
                localHttp.blockQuit = 0;
                break;
            }
            vTaskDelay(200 / portTICK_RATE_MS);
        }
    }
    if (localHttp.reqBuf) free(localHttp.reqBuf);
    if (localHttp.recvBuf) free(localHttp.recvBuf);
    if (localHttp.uri.str) free(localHttp.uri.str);

    memset(&localHttp, 0, sizeof(localHttp));
    localHttp.reqBuf = NULL;
    localHttp.recvBuf = NULL;
    localHttp.uri.str = NULL;
    dlInstance.state = TaskState_Unknown;
    dlInstance.thread = NULL;
//    mutex_destroy(&downloadMutex);
//    downloadMutex = NULL;
    LOGD("download task ------ blockQuit=%d, state=%d", localHttp.blockQuit, dlInstance.state);
}

