#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "inc/M3u8.h"

static int read_line_from_m3u8_info(char *m3u8_info, char *line_buffer, int buffer_size);
static int m3u8_info_eof();
static int parseSegFromUrl(M3u8Segment* tsp);
static char * strdupmy(const char *s);
unsigned int length;
unsigned int offset;

//////////////////////////////////////////////////////////////////////////////////////
char* trim_str(char *str)
{
    char* tmp = str;

    while (*str++ != '\r') {
        if (*str == '\0' || *str == '\n') {
            break;
        }
    }
    *str = '\0';
    return tmp;
}

// Must to be free memory
static char * strdupmy(const char *s)
{
    size_t  len = strlen(s) + 1;
    void *new = malloc(len);
    if (new == NULL)
        return NULL;
    return (char *)memcpy(new, s, len);
}

char* split_url(char* url, char* seg_path, char* seg_name)
{
    char* temp1;
    char rateStr[10] = {0};
    int len = strlen(url);
    while (url) {
        temp1 = strrchr(url, '/');
        if (temp1 == NULL) {
            printf("ERR:strrchr=NULL,%s,%d\n", __FUNCTION__, __LINE__);
            return NULL;
        }
        temp1 += 1;
        strncpy(seg_path, url, temp1 - url);
        strncpy(seg_name, temp1, len - (temp1 - url));
        return seg_path;
    }

    return NULL;
}

int is_blank(char *str)
{
    if (0 == strlen(str))
        return 1;
    else
        return 0;
}

static int read_line_from_m3u8_info(char *m3u8_info, char *line_buffer, int buffer_size)
{
    int start;
    int end = -1;
    char cur_char = '\0';
    int copy_size;

    start = offset;
    while (offset < length) {
        cur_char = m3u8_info[offset];
        if (cur_char == '\r' || cur_char == '\n') {
            end = offset;
            offset++;
            break;
        }
        offset++;
    }
    if (end == -1) {
        end = length;
    }
    if (cur_char == '\r') {
        offset++;
    }
    memset(line_buffer, 0, buffer_size);
    copy_size = end - start;
    if (copy_size > buffer_size - 1) {
        copy_size = buffer_size - 1;
    }
    strncpy(line_buffer, m3u8_info + start, copy_size);

    return 1;
}

static int m3u8_info_eof()
{
    if (offset >= length) {
        return 1;
    } else {
        return 0;
    }
}

int parseSegFromUrl(M3u8Segment* tsp)
{
    int len = strlen(tsp->url);
    int ret = -1;
    char *segment_path = (char*)malloc(len);
    char *segment_name = (char*)malloc(len);
    if ((segment_path == NULL) | (segment_name == NULL)) {
        goto SAVEEXIT;
    }
    memset(segment_path, 0, len);
    memset(segment_name, 0, len);
    split_url(tsp->url, segment_path, segment_name);
    //http://live.xmcdn.com/192.168.3.134/live/12/24/160411_000003_15f9.ts
    //http://live.xmcdn.com/192.168.3.134/live/75/24/160413_082919_12b0.aac
    char* temp1 = strchr(segment_name, '.');
    char* temp2 = strrchr(segment_name, '_');
    if ((temp1 == NULL) | (temp2 == NULL)) {
        printf("ERR:strrchr=NULL,%s,%d\n", __FUNCTION__, __LINE__);
        goto SAVEEXIT;
    }
    *temp1 = '\0';
    temp1++;
    if (strncmp(temp1, "aac", 3) == 0) {
        tsp->fmt = AUDIOFMT_AAC;
    } else if (strncmp(temp1, "ts", 3) == 0) {
        tsp->fmt = AUDIOFMT_TS;
    } else {
        tsp->fmt = AUDIOFMT_UNKNOWN;
    }
    ///subSeq
    temp2++;
    tsp->seq.sub = strtoul(temp2, 0, 16);
    ////mainSeq
    temp2--;
    *temp2 = '\0';
    temp1 = strrchr(segment_name, '_');
    if (temp1 == NULL) {
        printf("ERR:strrchr=NULL,%s,%d\n", __FUNCTION__, __LINE__);
        goto SAVEEXIT;
    }
    temp1++;
    tsp->seq.main = strtoul(temp1, 0, 16);
    ///
    temp2 = strchr(segment_name, '_');
    if (temp2 == NULL) {
        printf("ERR:strchr=NULL,%s,%d\n", __FUNCTION__, __LINE__);
        goto SAVEEXIT;
    }
    *temp2 = '\0';
    tsp->seq.date = strtoul(segment_name, 0, 16);

    temp1 = strrchr(segment_path, '/');
    if (temp1 == NULL) {
        printf("ERR:strrchr=NULL,%s,%d\n", __FUNCTION__, __LINE__);
        goto SAVEEXIT;
    }
    *temp1 = '\0';
    temp1 = strrchr(segment_path, '/');
    if (temp1 == NULL) {
        printf("ERR:strrchr=NULL,%s,%d\n", __FUNCTION__, __LINE__);
        goto SAVEEXIT;
    }
    temp1++;
    tsp->rate = atoi(temp1);
    ret = 0;
SAVEEXIT:
    if (segment_path) {
        free(segment_path);
    }
    if (segment_name) {
        free(segment_name);
    }
    return ret;
}
#define ONE_LINE_SIZE  1024
M3u8Info *m3u8Parser(char *m3u8Str, M3u8Info* m3u8)
{
    int has_segment = 0;
    if (NULL == m3u8) {
        printf("malloc for M3u8Info failed.");
        return NULL;
    }

    char *line = malloc(ONE_LINE_SIZE);
    if (NULL == line) {
        printf("zalloc for line failed.");
        return NULL;
    }
    memset(line, 0, ONE_LINE_SIZE);
    memset(m3u8, 0, sizeof(M3u8Info));
    length = strlen(m3u8Str);
    offset = 0;

    while (!m3u8_info_eof()) {
        int result = 0;
        char *str = NULL;
        if (m3u8->segCount >= MAX_SEGMENT_COUNT) {
            printf("segCount >= MAX_SEGMENT_COUNT[%d].", MAX_SEGMENT_COUNT);
            break;
        }
        result = read_line_from_m3u8_info(m3u8Str, line, ONE_LINE_SIZE);
        if (!result) {
            printf("ERR:read line error from m3u8 info.");
            if (line) free(line);
            return NULL;
        }
        if (is_blank(line)) {
            continue;
        }
        str = trim_str(line);
        if (has_segment) {
            int size = strlen(str) + 1;
            m3u8->segList[m3u8->segCount].seq.sub = m3u8->mediaSeq + m3u8->segCount;
            // m3u8->segList[m3u8->segCount - 1].url = (char *)malloc(size);
            memset(m3u8->segList[m3u8->segCount].url, 0, size);
            strcpy(m3u8->segList[m3u8->segCount].url, str);
            parseSegFromUrl(&m3u8->segList[m3u8->segCount]);
            has_segment = 0;
            m3u8->segCount++;
        } else {
            if (0 == strncmp(str, "#EXTM3U", strlen("#EXTM3U"))) {

            } else if (0 == strncmp(str, "#EXT-X-VERSION:", strlen("#EXT-X-VERSION:"))) {

            } else if (0 == strncmp(str, "#EXT-X-ALLOW-CACHE:", strlen("#EXT-X-ALLOW-CACHE:"))) {

            } else if (0 == strncmp(str, "#EXT-X-TARGETDURATION:", strlen("#EXT-X-TARGETDURATION:"))) {
                char *str_duration = str + strlen("#EXT-X-TARGETDURATION:");
                m3u8->targetDuration = atoi(trim_str(str_duration));
                // printf("Get the TARGETDURATION=%d\n", m3u8->targetDuration);
            } else if (0 == strncmp(str, "#EXT-X-MEDIA-SEQUENCE:", strlen("#EXT-X-MEDIA-SEQUENCE:"))) {
                char *str_sequence = str + strlen("#EXT-X-MEDIA-SEQUENCE:");
                m3u8->mediaSeq = atol(trim_str(str_sequence));
                //  printf("Get the MEDIA-SEQUENCE=%d\n", m3u8->mediaSeq);
            } else if (0 == strncmp(str, "#EXTINF:", strlen("#EXTINF:"))) {
                char *str_segment_duration = NULL;
                int i = 0;
                has_segment = 1;
                str_segment_duration = str + strlen("#EXTINF:");
                while (str_segment_duration[i] != 0) {
                    if ((str_segment_duration[i] < '0') || (str_segment_duration[i] > '9')) {
                        str_segment_duration[i] = 0;
                        break;
                    }
                    i++;
                }
                m3u8->segList[m3u8->segCount].duration = atoi(str_segment_duration);
            } else if (0 == strncmp(str, "#EXT-X-DISCONTINUITY", strlen("#EXT-X-DISCONTINUITY"))) {

            } else if (0 == strncmp(str, "#EXT-X-ENDLIST", strlen("#EXT-X-ENDLIST"))) {

            } else {
                printf("unknown line: %s.", str);
            }
        }
    }
    if (line) free(line);
    return m3u8;
}

uint8_t m3u8CheckValid(const M3u8Seq* new, const M3u8Seq* pre)
{
    if (pre->date < new->date) {
        return 1;
    } else if (pre->date == new->date) {
        if (pre->main < new->main) {
            return 1;
        } else if (pre->main == new->main) {
            if ((pre->sub & 0x0000FFFF) < (new->sub & 0x0000FFFF)) {
                return 1;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    } else {
        return 0;
    }

}

void m3u8Destroy(M3u8Info *m3u8)
{
    int i = 0;
    if (m3u8 == NULL) {
        return;
    }
    free(m3u8);
}

int m3u8_test(char* pStr)
{
    //char *m3u8Str = "#EXTM3U\n"
    //                    "#EXT-X-TARGETDURATION:8\n"
    //                    "#EXT-X-MEDIA-SEQUENCE:92595\n"
    //                    "#EXTINF:8,\n"
    //                    "20121120T182851-04-92595.ts\n"
    //                    "#EXTINF:8,\n"
    //                    "20121120T182851-04-92596.ts\n"
    //                    "#EXTINF:8,\n"
    //                    "20121120T182851-04-92597.ts\n"
    //                    "#EXTINF:8,\n"
    //                    "20121120T182851-04-92598.ts\n";


    for (int i = 0; i < 10000; i++) {
        // M3u8Info *m3u8 = m3u8Parser(pStr, m3u8);
        // m3u8Destroy(m3u8);
    }
    return 1;
}
