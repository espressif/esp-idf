/******************************************************************************
 *
 *  Copyright (C) 2003-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  This file contains utility functions.
 *
 ******************************************************************************/
#include <stddef.h>
#include "utl.h"
#include "gki.h"
#include "btm_api.h"

/*******************************************************************************
**
** Function         utl_str2int
**
** Description      This utility function converts a character string to an
**                  integer.  Acceptable values in string are 0-9.  If invalid
**                  string or string value too large, -1 is returned.  Leading
**                  spaces are skipped.
**
**
** Returns          Integer value or -1 on error.
**
*******************************************************************************/
INT16 utl_str2int(const char *p_s)
{
    INT32   val = 0;

    for (;*p_s == ' ' && *p_s != 0; p_s++);

    if (*p_s == 0) return -1;

    for (;;)
    {
        if ((*p_s < '0') || (*p_s > '9')) return -1;

        val += (INT32) (*p_s++ - '0');

        if (val > 32767) return -1;

        if (*p_s == 0)
        {
            return (INT16) val;
        }
        else
        {
            val *= 10;
        }
    }
}

/*******************************************************************************
**
** Function         utl_strucmp
**
** Description      This utility function compares two strings in uppercase.
**                  String p_s must be uppercase.  String p_t is converted to
**                  uppercase if lowercase.  If p_s ends first, the substring
**                  match is counted as a match.
**
**
** Returns          0 if strings match, nonzero otherwise.
**
*******************************************************************************/
int utl_strucmp(const char *p_s, const char *p_t)
{
    char c;

    while (*p_s && *p_t)
    {
        c = *p_t++;
        if (c >= 'a' && c <= 'z')
        {
            c -= 0x20;
        }
        if (*p_s++ != c)
        {
            return -1;
        }
    }
    /* if p_t hit null first, no match */
    if (*p_t == 0 && *p_s != 0)
    {
        return 1;
    }
    /* else p_s hit null first, count as match */
    else
    {
        return 0;
    }
}

/*******************************************************************************
**
** Function         utl_itoa
**
** Description      This utility function converts a UINT16 to a string.  The
**                  string is NULL-terminated.  The length of the string is
**                  returned;
**
**
** Returns          Length of string.
**
*******************************************************************************/
UINT8 utl_itoa(UINT16 i, char *p_s)
{
    UINT16  j, k;
    char    *p = p_s;
    BOOLEAN fill = FALSE;

    if (i == 0)
    {
        /* take care of zero case */
        *p++ = '0';
    }
    else
    {
        for(j = 10000; j > 0; j /= 10)
        {
            k = i / j;
            i %= j;
            if (k > 0 || fill)
            {
              *p++ = k + '0';
              fill = TRUE;
            }
        }
    }
    *p = 0;
    return (UINT8) (p - p_s);
}

/*******************************************************************************
**
** Function         utl_freebuf
**
** Description      This function calls GKI_freebuf to free the buffer passed
**                  in, if buffer pointer is not NULL, and also initializes
**                  buffer pointer to NULL.
**
**
** Returns          Nothing.
**
*******************************************************************************/
void utl_freebuf(void **p)
{
    if (*p != NULL)
    {
        GKI_freebuf(*p);
        *p = NULL;
    }
}


/*******************************************************************************
**
** Function         utl_set_device_class
**
** Description      This function updates the local Device Class.
**
** Parameters:
**                  p_cod   - Pointer to the device class to set to
**
**                  cmd     - the fields of the device class to update.
**                            BTA_UTL_SET_COD_MAJOR_MINOR, - overwrite major, minor class
**                            BTA_UTL_SET_COD_SERVICE_CLASS - set the bits in the input
**                            BTA_UTL_CLR_COD_SERVICE_CLASS - clear the bits in the input
**                            BTA_UTL_SET_COD_ALL - overwrite major, minor, set the bits in service class
**                            BTA_UTL_INIT_COD - overwrite major, minor, and service class
**
** Returns          TRUE if successful, Otherwise FALSE
**
*******************************************************************************/
BOOLEAN utl_set_device_class(tBTA_UTL_COD *p_cod, UINT8 cmd)
{
    UINT8 *dev;
    UINT16 service;
    UINT8  minor, major;
    DEV_CLASS dev_class;

    dev = BTM_ReadDeviceClass();
    BTM_COD_SERVICE_CLASS( service, dev );
    BTM_COD_MINOR_CLASS(minor, dev );
    BTM_COD_MAJOR_CLASS(major, dev );

    switch(cmd)
    {
    case BTA_UTL_SET_COD_MAJOR_MINOR:
        minor = p_cod->minor & BTM_COD_MINOR_CLASS_MASK;
        major = p_cod->major & BTM_COD_MAJOR_CLASS_MASK;
        break;

    case BTA_UTL_SET_COD_SERVICE_CLASS:
        /* clear out the bits that is not SERVICE_CLASS bits */
        p_cod->service &= BTM_COD_SERVICE_CLASS_MASK;
        service = service | p_cod->service;
        break;

    case BTA_UTL_CLR_COD_SERVICE_CLASS:
        p_cod->service &= BTM_COD_SERVICE_CLASS_MASK;
        service = service & (~p_cod->service);
        break;

    case BTA_UTL_SET_COD_ALL:
        minor = p_cod->minor & BTM_COD_MINOR_CLASS_MASK;
        major = p_cod->major & BTM_COD_MAJOR_CLASS_MASK;
        p_cod->service &= BTM_COD_SERVICE_CLASS_MASK;
        service = service | p_cod->service;
        break;

    case BTA_UTL_INIT_COD:
        minor = p_cod->minor & BTM_COD_MINOR_CLASS_MASK;
        major = p_cod->major & BTM_COD_MAJOR_CLASS_MASK;
        service = p_cod->service & BTM_COD_SERVICE_CLASS_MASK;
        break;

    default:
        return FALSE;
    }

    /* convert the fields into the device class type */
    FIELDS_TO_COD(dev_class, minor, major, service);

    if (BTM_SetDeviceClass(dev_class) == BTM_SUCCESS)
        return TRUE;

    return FALSE;
}

/*******************************************************************************
**
** Function         utl_isintstr
**
** Description      This utility function checks if the given string is an
**                  integer string or not
**
**
** Returns          TRUE if successful, Otherwise FALSE
**
*******************************************************************************/
BOOLEAN utl_isintstr(const char *p_s)
{
    UINT16 i = 0;

    for(i=0; p_s[i] != 0; i++)
    {
        if(((p_s[i] < '0') || (p_s[i] > '9')) && (p_s[i] != ';'))
            return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
**
** Function         utl_isdialstr
**
** Description      This utility function checks if the given string contains
**                  only dial digits or not
**
**
** Returns          TRUE if successful, Otherwise FALSE
**
*******************************************************************************/
BOOLEAN utl_isdialstr(const char *p_s)
{
    UINT16 i = 0;

    for(i=0; p_s[i] != 0; i++)
    {
        if(!(((p_s[i] >= '0') && (p_s[i] <= '9'))
            || (p_s[i] == '*') || (p_s[i] == '+') || (p_s[i] == '#') || (p_s[i] == ';')
            || ((p_s[i] >= 'A') && (p_s[i] <= 'C'))
            || ((p_s[i] == 'p') || (p_s[i] == 'P')
            || (p_s[i] == 'w') || (p_s[i] == 'W'))))
            return FALSE;
    }

    return TRUE;
}


