/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _crc32_H_
#define _crc32_H_

/**
* @brief This class is used to access crc32 module
*
*/
class crc32
{
public:
    static unsigned int crc32_le(unsigned int crc, unsigned char const *buf, unsigned int len);
};
#endif // _crc32_H_
