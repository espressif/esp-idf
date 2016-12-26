#ifndef _M3U8_H_
#define _M3U8_H_
#include "esp_types.h"

#define MAX_SEGMENT_COUNT 3


//
typedef enum {
    AUDIOFMT_UNKNOWN    = 0,
    AUDIOFMT_AAC        = 1,
    AUDIOFMT_TS         = 2,
} AUDIOFMT;

typedef struct {
    size_t  date;
    size_t  main;
    size_t  sub;
} M3u8Seq;
//
typedef struct {
    int     duration;
    int     rate;
    AUDIOFMT fmt;
    char    url[128];
    char    name[256];
    M3u8Seq seq;
} M3u8Segment;

//
typedef struct _M3u8Info {
    int     targetDuration;
    size_t  mediaSeq;
    int     segCount;
    M3u8Segment segList[MAX_SEGMENT_COUNT];
} M3u8Info;


typedef struct _audioUrlObj {
    AUDIOFMT        fmt;
    uint32_t          uflag;  // 0-empty;1-ready;2-using;3-used;
    uint32_t          idx;    // stream sequence
//  uint32_t          totalTime;  // time of music,live:0
    char            *url;
    char            *radioName;
    char            *musicName;
    struct _audioUrlObj *next;
} audioUrlObj;


uint8_t m3u8CheckValid(const M3u8Seq* new, const M3u8Seq* pre);


/**
 * [m3u8_parser Parser the m3u8 string]
 * @param  m3u8_string [To be parser string]
 * @return             [m3u8 object, NULL->failed]
 */
M3u8Info *m3u8Parser(char *m3u8Str, M3u8Info* m3u8);

/**
 * [m3u8_destroy Destroy the m3u8 struct]
 * @param m3u8 [m3u8 struct]
 */
void m3u8Destroy(M3u8Info *m3u8);

/**
 * [m3u8_save description]
 * @param  m3u8                  [m3u8 struct]
 * @param  m3u8_path             [file path]
 * @param  ts_segment_url_prefix [prefix will be fill]
 * @return                       [0->failed;1->successed]
 */
//int m3u8_save(M3u8Info *m3u8, char *m3u8_path, char *ts_segment_url_prefix);


int m3u8_test(char* pStr);

#endif
