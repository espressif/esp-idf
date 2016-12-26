/*
Copyright 2006 - 2011 Intel Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


/*
* Implements additional string functionality.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "MyString.h"

#ifdef UNDER_CE
#define strnicmp _strnicmp
#endif

#ifdef _POSIX
#define strnicmp strncasecmp
#endif

int EndsWith(const char* str, const char* endsWith, int ignoreCase)
{
    int strLen, ewLen, offset;
    int cmp = 0;

    strLen = (int)strlen(str);
    ewLen = (int)strlen(endsWith);
    if (ewLen > strLen) return 0;
    offset = strLen - ewLen;

    if (ignoreCase != 0) {
        cmp = strncmp(str + offset, endsWith, ewLen);
    } else {
        cmp = strncmp(str + offset, endsWith, ewLen);
    }

    return cmp == 0 ? 1 : 0;
}

int IndexOf(const char* str, const char* findThis)
{
    int i, j, strLen, ftLen;

    strLen = (int)strlen(str);
    ftLen = (int)strlen(findThis);

    if (ftLen <= strLen) {
        for (i = 0; i < strLen; i++) {
            for (j = 0; j < ftLen; j++) {
                if (str[i + j] != findThis[j]) break;
            }
            if (j == ftLen) return i;
        }
    }

    return -1;
}

int LastIndexOf(const char* str, const char* findThis)
{
    int i, j, strLen, ftLen;

    strLen = (int)strlen(str);
    ftLen = (int)strlen(findThis);

    if (ftLen <= strLen) {
        for (i = strLen - ftLen; i >= 0; i--) {
            for (j = 0; j < ftLen; j++) {
                if (str[i + j] != findThis[j]) break;
            }
            if (j == ftLen) return i;
        }
    }

    return -1;
}

int StartsWith(const char* str, const char* startsWith, int ignoreCase)
{
    int cmp;

    if (ignoreCase != 0) {
         cmp = strncmp(str, startsWith, (int)strlen(startsWith));
    } else {
        cmp = strncmp(str, startsWith, (int)strlen(startsWith));
    }

    return cmp == 0 ? 1 : 0;
}

void debug_func3()
{
    printf("debug_func3...\r\n");
}

