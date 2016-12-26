#ifndef HTTPD_COM_H
#define HTTPD_COM_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "httpd-com.h"

typedef struct RtosConnType RtosConnType;
typedef RtosConnType* ConnTypePtr;
#define httpd_printf(fmt, ...) do { \
    ets_printf(fmt, ##__VA_ARGS__); \
    } while(0)

#endif
