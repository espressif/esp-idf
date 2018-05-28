/* Customer ID=11656; Build=0x5f626; Copyright (c) 2005-2014 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of 
 * Cadence Design Systems, Inc. They may not be modified, copied, reproduced, 
 * distributed, or disclosed to third parties in any manner, medium, or form, 
 * in whole or in part, without the prior written consent of Cadence Design 
 * Systems Inc.
 */
#ifndef __UDMA_H__
#define __UDMA_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Size of the uDMA descriptor */
#define UDMA_DESC_STRUCT_SIZE 32

/* Request attribute is a bit vector passed to the udma functions - udma_copy,
 * udma_2d_copy, udma_add_descs. 
 * Bit 0 : 1 - trigger an interrupt when done, else do nothing
 * Bit 1 : 0 - retry the failed request; abort after programmer specified 
 *             number of retries. Defaults to abort with no retries.
 *         1 - abort the failed request (after retries) and all pending requests
 */
#define UDMA_DONE_INTERRUPT  0x1
#define UDMA_ERROR_ABORT     0x0
#define UDMA_ERROR_ABORT_ALL 0x2

/* Enum representing various udma error conditions, udma status, and
 * return values 
 */
typedef enum {
  UDMA_OK                     =  0,
  UDMA_ERROR_QUEUE_FULL       =  1,  
  UDMA_ERROR_BAD_DESC         =  2,  
  UDMA_ERROR_DRAM_CROSSING    =  3,
  UDMA_ERROR_PIF_ADDR_BUS     =  4,
  UDMA_ERROR_PIF_DATA_BUS     =  5,
  UDMA_REQ_PENDING            =  6,
  UDMA_REQ_DONE               =  7,
  UDMA_ERROR_BAD_REQUEST      =  8,
  UDMA_ERROR_INVALID_ARG      = 11,
  UDMA_ERROR_INIT             = 12,
  UDMA_ERROR_INTERNAL         = -1
} udma_status_t;

#ifndef __UDMA_INTERNAL_H__
/* Opaque structure describing a uDMA descriptor */
struct udma_desc_struct {
  char _[UDMA_DESC_STRUCT_SIZE];
}  __attribute__ ((aligned (UDMA_DESC_STRUCT_SIZE)));
#endif

typedef struct udma_desc_struct udma_desc_t;

/* Initialize the udma control structure, the uDMA registers with
 * the descriptor queue addresses, and the uDMA sync and error interrupt 
 * handler. This function needs to be invoked prior to using the uDMA.
 *
 * xmp_udma_sync_intr  : Processor interrupt number to flag udma done
 * xmp_udma_error_intr : Processor interrupt number to flag udma error
 * 
 * Returns UDMA_ERROR_INIT if there was an error during initialization else
 * returns UDMA_OK.
 */
extern udma_status_t 
udma_init(uint32_t xmp_udma_sync_intr, uint32_t xmp_udma_error_intr);

/* Performs a copy of a linear block of size bytes from the src
 * to the dest address. If the call returns UDMA_OK, status is set to 
 * UDMA_REQ_PENDING or UDMA_REQ_DONE. If there is a dma error, the error code,
 * which could be one of UDMA_ERROR_BAD_DESC, UDMA_ERROR_DRAM_CROSSING, 
 * UDMA_ERROR_PIF_ADDR_BUS, UDMA_ERROR_PIF_DATA_BUS is returned in the status. 
 * Status is set to UDMA_REQ_DONE if the dma completes normally.
 * On completion, the callback function is invoked with the callback_data
 * and status as parameters. Note, the callback is always invoked even if 
 * there is a dma error.
 *
 * src           : src address of the copy
 * dest          : dest address of the copy
 * size          : number of bytes to copy
 * callback_data : optional data to be passed to callback_func
 * callback_func : optional callback after copy is done
 * request_attr  : attribute defining how to process this request 
 *                 (see description of the request attribute in top of udma.h)
 * status        : track status of the copy; this gets also passed
 *                 as the second argument to the callback_func
 * 
 * Returns UDMA_ERROR_QUEUE_FULL if no more requests can be added, else
 * returns UDMA_OK.
 */
extern udma_status_t
udma_copy(void *dest,
          void *src,
          size_t size,
          void *callback_data,
          void (*callback_func)(void *, udma_status_t *status),
          uint32_t request_attr,
          udma_status_t *status);

/* Performs a copy of a 2D block of data from the src to the dest
 * address. If the call returns UDMA_OK, status is set to UDMA_REQ_PENDING or
 * UDMA_REQ_DONE. If there is a dma error, the error code,
 * which could be one of UDMA_ERROR_BAD_DESC, UDMA_ERROR_DRAM_CROSSING, 
 * UDMA_ERROR_PIF_ADDR_BUS, UDMA_ERROR_PIF_DATA_BUS is returned in the status. 
 * Status is set to UDMA_REQ_DONE if the dma completes normally.
 * On completion, the callback function is invoked with the callback_data
 * and status as parameters. Note, the callback is always invoked even if 
 * there is a dma error.
 *
 * src           : src address of the copy 
 * dest          : dest address of the copy
 * row_size      : number of bytes per row to copy
 * num_rows      : number of rows to copy
 * src_pitch     : src pitch
 * dest_pitch    : dest pitch
 * callback_data : optional data to be passed to callback_func
 * callback_func : optional callback after copy is done
 * request_attr  : attribute defining how to process this request 
 *                 (see description of the request attribute in top of udma.h)
 * status        : track status of the copy; this gets also passed
 *                 as the second argument to the callback_func
 *  
 * Returns UDMA_ERROR_QUEUE_FULL if no more requests can be added, else
 * returns UDMA_OK.
 */
extern udma_status_t 
udma_2d_copy(void *dest,
             void *src, 
             size_t row_size, 
             uint32_t num_rows,
             uint32_t src_pitch, 
             uint32_t dest_pitch,
             void *callback_data,
             void (*callback_func)(void *, udma_status_t *status),
             uint32_t request_attr,
             udma_status_t *status);

/* Process requests that are done. Any callbacks associated
 * with the completed requests gets invoked. If there are any errors,
 * the error request is (and any pending request based on the request attribute)
 * cancelled and the error code is returned in the status associated with 
 * all such cancelled requests. Callbacks associated with the cancelled
 * requests are also invoked. If all requests terminate normally, the status 
 * of the completed requests are set to UDMA_REQ_DONE.
 * 
 * Returns void
 */
extern void 
udma_process_requests();

/* Sets the udma max PIF block size
 *
 * max_block_size : max block size to be set
 *
 * Returns UDMA_ERROR_INVALID_ARG if block_size is > 3, else returns UDMA_OK
 */
udma_status_t
udma_set_max_block_size(uint32_t block_size);

/* Sets the udma max outstanding PIF requests
 *
 * max_outstanding : max outstanding PIF requests
 *
 * Returns UDMA_ERROR_INVALID_ARG if max_outstanding is not between 1 and 16
 * else returns UDMA_OK
 */
udma_status_t
udma_set_max_outstanding(uint32_t max_outstanding);

/* Initialize a uDMA descriptor using the copy parameters. The descriptor
 * is then queued separately using udma_add_desc
 *
 * src                   : src address of the copy
 * dest                  : dest address of the copy
 * row_size              : number of bytes per row to copy
 * num_rows              : number of rows to copy
 * src_pitch             : src pitch
 * dest_pitch            : dest pitch
 * notify_with_interrupt : If 1, interrupt when dma is done with this descriptor
 *                         if 0, do nothing, else undefined
 *
 * Returns void
 */
extern void
udma_set_desc(void *src,
              void *dest,
              size_t row_size, 
              uint32_t num_rows,
              uint32_t src_pitch, 
              uint32_t dest_pitch,
              uint32_t notify_with_interrupt,
              udma_desc_t *desc);

/* Add multiple uDMA descriptors to the descriptor queue. If the call returns
 * UDMA_OK, the status is set to UDMA_REQ_PENDING or UDMA_REQ_DONE.
 * If there is a dma error, the error code, which could be one of
 * UDMA_ERROR_BAD_DESC, UDMA_ERROR_DRAM_CROSSING, UDMA_ERROR_PIF_ADDR_BUS,
 * UDMA_ERROR_PIF_DATA_BUS is returned in the status. Status is set
 * to UDMA_REQ_DONE, if the dma completes normally. 
 * On completion, the callback function is invoked with the callback_data
 * and status as parameters. Note, the callback is always invoked even if 
 * there is a dma error.
 *
 * desc          : descriptors to be added
 * num_desc      : number of descriptors to be added
 * callback_data : optional data to be passed to callback_func
 * callback_func : optional callback after copy is done
 * request_attr  : attribute defining how to process this request 
 *                 (see description of the request attribute in top of udma.h)
 *                 Note, bit 0 (for enabling interrupt) is ignored in this call.
 *                 To interrupt on dma completion, set the
 *                 notify_with_interrupt parameter when creating descriptors
 *                 using udma_set_desc.
 * status        : track status of the copy; this gets also passed
 *                 as the second argument to the callback_func
 *
 * Returns UDMA_ERROR_QUEUE_FULL if no more descriptors can be added,
 * UDMA_ERROR_INVALID_ARG if num_descs == 0, else return UDMA_OK
 */
udma_status_t
udma_add_descs(udma_desc_t *descs,
               uint32_t num_descs,
               void *callback_data,
               void (*callback_func)(void *, udma_status_t *status),
               uint32_t request_attr,
               udma_status_t *status);

/* Wait for udma copy request to complete. Could spin wait or goto waiti
 * based on the sleep_wait parameter. Once the request is done, the callback 
 * associated with this request and any prior completed requests are handled.
 * Error code, if any, is returned in the status s, else s is set to 
 * UDMA_REQ_DONE.
 *
 * s          : status to wait for
 * sleep_wait : sleep wait if true, else spin waits
 *
 * Returns void
 */
extern void
udma_wait_request(volatile udma_status_t *s, uint32_t sleep_wait);

/* Inlined function to set the src, dest address of the descriptor
 *
 * src  : src address of the uDMA 
 * dest : dest address of the uDMA
 * desc : descriptor to be modified
 *
 * Returns void
 */
void static inline 
udma_set_desc_addrs(void *src, void *dest, udma_desc_t *desc) {
  uint32_t *d = (uint32_t *)desc;
  *d = (uintptr_t)src;
  *(d+1) = (uintptr_t)dest;
}

/* Sets the number of retries for a failed dma request
 *
 * max_retries : max number of retries
 * 
 * Sets the max number of retries for a failed dma request. The default is 0,
 * i.e, no retries
 */
void
udma_set_max_retries(uint32_t max_retries);

#ifdef __cplusplus
}
#endif

#endif /* __UDMA_H__ */
