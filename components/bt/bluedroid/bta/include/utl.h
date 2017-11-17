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
 *  Basic utility functions.
 *
 ******************************************************************************/
#ifndef UTL_H
#define UTL_H

#include "bt_types.h"
// #include "bt_utils.h"

/*****************************************************************************
**  Constants
*****************************************************************************/
/*** class of device settings ***/
#define BTA_UTL_SET_COD_MAJOR_MINOR     0x01
#define BTA_UTL_SET_COD_SERVICE_CLASS   0x02 /* only set the bits in the input */
#define BTA_UTL_CLR_COD_SERVICE_CLASS   0x04
#define BTA_UTL_SET_COD_ALL             0x08 /* take service class as the input (may clear some set bits!!) */
#define BTA_UTL_INIT_COD                0x0a

/*****************************************************************************
**  Type Definitions
*****************************************************************************/

/** for utl_set_device_class() **/
typedef struct {
    UINT8       minor;
    UINT8       major;
    UINT16      service;
} tBTA_UTL_COD;


#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/

/*******************************************************************************
**
** Function         utl_str2int
**
** Description      This utility function converts a character string to an
**                  integer.  Acceptable values in string are 0-9.  If invalid
**                  string or string value too large, -1 is returned.
**
**
** Returns          Integer value or -1 on error.
**
*******************************************************************************/
extern INT16 utl_str2int(const char *p_s);

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
extern int utl_strucmp(const char *p_s, const char *p_t);

/*******************************************************************************
**
** Function         utl_itoa
**
** Description      This utility function converts a UINT16 to a string.  The
**                  string is NULL-terminated.  The length of the string is
**                  returned.
**
**
** Returns          Length of string.
**
*******************************************************************************/
extern UINT8 utl_itoa(UINT16 i, char *p_s);

/*******************************************************************************
**
** Function         utl_freebuf
**
** Description      This function calls osi_free to free the buffer passed
**                  in, if buffer pointer is not NULL, and also initializes
**                  buffer pointer to NULL.
**
**
** Returns          Nothing.
**
*******************************************************************************/
extern void utl_freebuf(void **p);

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
extern BOOLEAN utl_set_device_class(tBTA_UTL_COD *p_cod, UINT8 cmd);

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
extern BOOLEAN utl_isintstr(const char *p_s);

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
extern BOOLEAN utl_isdialstr(const char *p_s);

#ifdef __cplusplus
}
#endif

#endif /* UTL_H */
