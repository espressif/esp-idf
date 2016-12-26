#ifndef HTTPD_PLATFORM_H
#define HTTPD_PLATFORM_H
#include "httpd-com.h"

#define WEB_SERVER_TASK_PRIO   6
int httpdPlatSendData(ConnTypePtr conn, char *buff, int len);
void httpdPlatDisconnect(ConnTypePtr conn);
void httpdPlatDisableTimeout(ConnTypePtr conn);
void httpdPlatInit(int port, int maxConnCt);
int httpdPlatUninit(void) ;


#endif
