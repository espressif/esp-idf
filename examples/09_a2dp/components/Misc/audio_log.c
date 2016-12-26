/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP32 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "audio_log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "soc/spi_reg.h"

//*****************************************************************************
//
// Make sure all of the definitions in this header have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif
char tmp[1500] = {0};
/**
 * @brief print out log information.
 *
 */
void __log(const char *level, const char * func, uint32_t line, const char * restrict format, ...)
{
    if (level != NULL) {
        //memset(tmp, 0, sizeof(tmp));
        va_list ap;
        va_start(ap, format);
        (void)vsnprintf(tmp, sizeof(tmp), format, ap);
        tmp[1500-1] = '\0';
        va_end(ap);
        printf("[%s]%s(%d):%s\r\n", level, func, line,tmp);
    }
}

#ifdef __cplusplus
}
#endif
