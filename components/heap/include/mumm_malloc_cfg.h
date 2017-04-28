/*
 * Configuration for mumm_malloc in IDF
 *
 * Unlike umm_malloc, this config doesn't include
 * much heap configuration - just compiler configuration
 */

#ifndef _MUMM_MALLOC_CFG_H
#define _MUMM_MALLOC_CFG_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/* A couple of macros to make packing structures less compiler dependent */

#define MUMM_H_ATTPACKPRE
#define MUMM_H_ATTPACKSUF __attribute__((__packed__))

#define MUMM_INFO

#ifdef MUMM_INFO
  typedef struct MUMM_HEAP_INFO_t {
    unsigned short int totalEntries;
    unsigned short int usedEntries;
    unsigned short int freeEntries;

    unsigned short int totalBlocks;
    unsigned short int usedBlocks;
    unsigned short int freeBlocks;

    unsigned short int maxFreeContiguousBlocks;

	unsigned short int blockSize;
	unsigned short int numBlocks; /* configured, not counted */
  }
  MUMM_HEAP_INFO;

void *mumm_info( mumm_heap_handle heap, void *ptr, int force, MUMM_HEAP_INFO* info );
size_t mumm_free_heap_size( mumm_heap_handle heap );

#else
#endif

#define MUMM_CRITICAL_ENTRY(PLOCK) taskENTER_CRITICAL((portMUX_TYPE *)(PLOCK))
#define MUMM_CRITICAL_EXIT(PLOCK)  taskEXIT_CRITICAL((portMUX_TYPE *)(PLOCK))

/*
 * -D MUMM_INTEGRITY_CHECK :
 *
 * Enables heap integrity check before any heap operation. It affects
 * performance, but does NOT consume extra memory.
 *
 * If integrity violation is detected, the message is printed and user-provided
 * callback is called: `UMM_HEAP_CORRUPTION_CB()`
 *
 * Note that not all buffer overruns are detected: each buffer is aligned by
 * 4 bytes, so there might be some trailing "extra" bytes which are not checked
 * for corruption.
 */

//#define MUMM_INTEGRITY_CHECK

#ifdef MUMM_INTEGRITY_CHECK
   int mumm_integrity_check( mumm_heap_handle heap );
#  define INTEGRITY_CHECK(HEAP) mumm_integrity_check(HEAP)
   extern void mumm_corruption(void);
#  define MUMM_HEAP_CORRUPTION_CB(heap) printf( "Heap Corruption in heap %p!\n", heap )
#else
#  define INTEGRITY_CHECK(HEAP) 0
#endif

/*
 * -D MUMM_POISON :
 *
 * Enables heap poisoning: add predefined value (poison) before and after each
 * allocation, and check before each heap operation that no poison is
 * corrupted.
 *
 * Other than the poison itself, we need to store exact user-requested length
 * for each buffer, so that overrun by just 1 byte will be always noticed.
 *
 * Customizations:
 *
 *    UMM_POISON_SIZE_BEFORE:
 *      Number of poison bytes before each block, e.g. 2
 *    UMM_POISON_SIZE_AFTER:
 *      Number of poison bytes after each block e.g. 2
 *    UMM_POISONED_BLOCK_LEN_TYPE
 *      Type of the exact buffer length, e.g. `short`
 *
 * NOTE: each allocated buffer is aligned by 4 bytes. But when poisoning is
 * enabled, actual pointer returned to user is shifted by
 * `(sizeof(UMM_POISONED_BLOCK_LEN_TYPE) + UMM_POISON_SIZE_BEFORE)`.
 * It's your responsibility to make resulting pointers aligned appropriately.
 *
 * If poison corruption is detected, the message is printed and user-provided
 * callback is called: `UMM_HEAP_CORRUPTION_CB()`
 */

//#define MUMM_POISON_CHECK

#define UMM_POISON_SIZE_BEFORE 4
#define UMM_POISON_SIZE_AFTER 4
#define UMM_POISONED_BLOCK_LEN_TYPE short

#ifdef MUMM_POISON_CHECK
void *mumm_poison_malloc( mumm_heap_handle heap, size_t size );
void *mumm_poison_calloc( mumm_heap_handle heap, size_t num, size_t size );
void *mumm_poison_realloc( mumm_heap_handle heap, void *ptr, size_t size );
void  mumm_poison_free( mumm_heap_handle heap, void *ptr );
int   mumm_poison_check( mumm_heap_handle heap );
#  define POISON_CHECK(HEAP) mumm_poison_check(HEAP)
#else
#  define POISON_CHECK(HEAP) 0
#endif

#endif /* _MUMM_MALLOC_CFG_H */
