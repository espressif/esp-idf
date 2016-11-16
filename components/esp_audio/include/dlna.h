#ifndef _DNLA_H_
#define _DNLA_H_
typedef enum  {
    DLNA_INIT,
    DLNA_START,
    DLNA_STOP,
    DLNA_MEDIA_PLAYING,
    DLNA_MEDIA_PAUSED,
    DLNA_MEDIA_ERROR,
    DLNA_MEDIA_STOPPED,
    DLNA_MEDIA_ENDED,
    DLNA_UNINIT
} EspDlnaState;


typedef void (*dnlaStateCallback)(EspDlnaState state);

typedef enum {
    ESPDLNA_ERROR = -1,
    ESPDLNA_OK = 0
} EspDlnaErr;

typedef struct {
    char FriendlyName[128];
    char UDN[64];
    char SerialNumber[64];
    int NotifyCycleSeconds;
    unsigned short PortNum;
} EspDlnaAttr;

void dlna_task(void *pvParameters);
EspDlnaErr EspDlnaInit(EspDlnaAttr *attr, dnlaStateCallback cb);
EspDlnaErr EspDlnaUnInit(void);
EspDlnaErr EspDlnaStart(void);
EspDlnaErr EspDlnaStop(void);
#endif
