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
#ifndef __STORAGE_H__
#define __STORAGE_H__

//*****************************************************************************
//
// Make sure all of the definitions in this header have a C binding.
//
//*****************************************************************************

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Initialize the specified buffer
 *
 * @attention : The specified buffer is cyclic.
 *
 * @return 0:Successed; others:failed.
 */

int audioBufInit();

/**
 * @brief Uninitialize buffer
 *
 * @return 0:Successed; others:failed.
 */
void audioBufUninit();


/**
 * @brief Retrieves the size of the specified buffer, in bytes.
 *
 * @return > 0: Size of the buffer was filled.
 */
int audioBufFillGet();

/**
 * @brief Reads data from the specified buffer.
 *
 * @para [out] dest: A pointer to the buffer that receives the data read from specified buffer.
 *       [in ] n   : The maximum number of bytes to be read.
 *       [in] t   : Timeout portMAX_DELAY-block; others,unblock.
 *
 * @attention : The specified buffer is cyclic.
 *
 * @return > 0: Actually number of bytes has read. 0: buffer is too low,failed.
 */
int audioBufRead(char *dest, int n, uint32_t t);


/**
 * @brief Writes data to the specified buffer.
 *
 * @para [in] src : A pointer to the buffer containing the data to be written to the specified buffer.
 *       [in] n   : The number of bytes to be written to the buffer.
 *       [in] t   : Timeout portMAX_DELAY-block; others,unblock.
 *
 * @attention : The specified buffer is cyclic.
 *
 * @return 0:Failed,no more spaces for store; > 0:Actually number of bytes has write.
 */
int audioBufWrite(char *src, int n, uint32_t t);

/**
 * @brief Reset the write and read buffer pointer.
 *
 * @return None.
 */
void audioBufRestAll();


/**
 * @brief Reset the read buffer address.
 *
 * @return None.
 */
void audioBufRdRest();

/**
 * @brief Reset the read buffer address.
 *
 * @return None.
 */
void audioBufWrRest();

/**
 * @brief Get the buffer read and write position.
 *
 * @return None.
 */
void audioBufPosGet(uint32_t *rdPos, uint32_t *wrPos);

/**
 * @brief Set the buffer read and write position.
 *
 * @return None.
 */
void audioBufRdPosSet(uint32_t *rdPos);

/**
 * @brief  Set quit queue for read block.
 *
 * @return None.
 */
void audioBufRdQuitCfg(void * que);

/**
 * @brief Set quit queue for write block.
 *
 * @return None.
 */
void audioBufWrQuitCfg(void * que);


#ifdef __cplusplus
}
#endif

#endif //  __STORAGE_H__
