/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <sys/param.h>
#include "esp_attr.h"
#include "esp_cache.h"
#include "esp_check.h"
#include "esp_dma_utils.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_memory_utils.h"
#include "esp_private/esp_cache_private.h"
#include "esp_private/periph_ctrl.h"

#if CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif
#include "hal/aes_hal.h"

#include "esp_aes_dma_priv.h"
#include "esp_aes_internal.h"
#include "esp_crypto_dma.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "mbedtls/aes.h"
#include "mbedtls/platform_util.h"

#if SOC_AES_SUPPORT_GCM
#include "aes/esp_aes_gcm.h"
#endif

/* Max size of each chunk to process when output buffer is in unaligned external ram
   must be a multiple of block size
*/
#define AES_MAX_CHUNK_WRITE_SIZE 1600

/* Input over this length will yield and wait for interrupt instead of
   busy-waiting, 30000 bytes is approx 0.5 ms */
#define AES_DMA_INTR_TRIG_LEN 2000

/* With buffers in PSRAM (worst condition) we still achieve a speed of 4 MB/s
   thus a 2 second timeout value should be sufficient for even very large buffers.
 */
#define AES_WAIT_INTR_TIMEOUT_MS 2000

#if defined(CONFIG_MBEDTLS_AES_USE_INTERRUPT)
static SemaphoreHandle_t op_complete_sem;
#if defined(CONFIG_PM_ENABLE)
static esp_pm_lock_handle_t s_pm_cpu_lock;
static esp_pm_lock_handle_t s_pm_sleep_lock;
#endif
#endif

static const char *TAG = "esp-aes";

static bool s_check_dma_capable(const void *p)
{
    bool is_capable = false;
#if CONFIG_SPIRAM
    is_capable |= esp_ptr_dma_ext_capable(p);
#endif
    is_capable |= esp_ptr_dma_capable(p);

    return is_capable;
}

#if defined (CONFIG_MBEDTLS_AES_USE_INTERRUPT)
static IRAM_ATTR void esp_aes_complete_isr(void *arg)
{
    BaseType_t higher_woken;
    aes_hal_interrupt_clear();
    xSemaphoreGiveFromISR(op_complete_sem, &higher_woken);
    if (higher_woken) {
        portYIELD_FROM_ISR();
    }
}

void esp_aes_intr_alloc(void)
{
    if (op_complete_sem == NULL) {
        const int isr_flags = esp_intr_level_to_flags(CONFIG_MBEDTLS_AES_INTERRUPT_LEVEL);

        esp_err_t ret = esp_intr_alloc(ETS_AES_INTR_SOURCE, isr_flags, esp_aes_complete_isr, NULL, NULL);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to allocate AES interrupt %d", ret);
            // This should be treated as fatal error as this API would mostly
            // be invoked within mbedTLS interface. There is no way for the system
            // to proceed if the AES interrupt allocation fails here.
            abort();
        }

        static StaticSemaphore_t op_sem_buf;
        op_complete_sem = xSemaphoreCreateBinaryStatic(&op_sem_buf);
        // Static semaphore creation is unlikely to fail but still basic sanity
        assert(op_complete_sem != NULL);
    }
}

static esp_err_t esp_aes_isr_initialise( void )
{
    aes_hal_interrupt_clear();
    aes_hal_interrupt_enable(true);

    /* AES is clocked proportionally to CPU clock, take power management lock */
#ifdef CONFIG_PM_ENABLE
    if (s_pm_cpu_lock == NULL) {
        if (esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "aes_sleep", &s_pm_sleep_lock) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create PM sleep lock");
            return ESP_FAIL;
        }
        if (esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "aes_cpu", &s_pm_cpu_lock) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create PM CPU lock");
            return ESP_FAIL;
        }
    }
    esp_pm_lock_acquire(s_pm_cpu_lock);
    esp_pm_lock_acquire(s_pm_sleep_lock);
#endif

    return ESP_OK;
}
#endif // CONFIG_MBEDTLS_AES_USE_INTERRUPT

static inline void esp_aes_wait_dma_done(crypto_dma_desc_t *output)
{
    /* Wait for DMA write operation to complete */
    while (1) {
        if ( esp_aes_dma_done(output) ) {
            break;
        }
    }
}

/* Wait for AES hardware block operation to complete */
static int esp_aes_dma_wait_complete(bool use_intr, crypto_dma_desc_t *output_desc_tail)
{
#if defined (CONFIG_MBEDTLS_AES_USE_INTERRUPT)
    if (use_intr) {
        if (!xSemaphoreTake(op_complete_sem, AES_WAIT_INTR_TIMEOUT_MS / portTICK_PERIOD_MS)) {
            /* indicates a fundamental problem with driver */
            ESP_LOGE(TAG, "Timed out waiting for completion of AES Interrupt");
            return -1;
        }
#ifdef CONFIG_PM_ENABLE
        esp_pm_lock_release(s_pm_cpu_lock);
        esp_pm_lock_release(s_pm_sleep_lock);
#endif  // CONFIG_PM_ENABLE
    }
#endif
    /* Checking this if interrupt is used also, to avoid
       issues with AES fault injection
    */
    aes_hal_wait_done();

    esp_aes_wait_dma_done(output_desc_tail);
    return 0;
}


/* Output buffers in external ram needs to be 16-byte aligned and DMA can't access input in the iCache mem range,
   reallocate them into internal memory and encrypt in chunks to avoid
   having to malloc too big of a buffer

  The function esp_aes_process_dma_ext_ram zeroises the output buffer in the case of memory allocation failure.
*/

static int esp_aes_process_dma_ext_ram(esp_aes_context *ctx, const unsigned char *input, unsigned char *output, size_t len, uint8_t *stream_out, bool realloc_input, bool realloc_output)
{
    size_t chunk_len;
    int ret = 0;
    int offset = 0;
    unsigned char *input_buf = NULL;
    unsigned char *output_buf = NULL;
    const unsigned char *dma_input;
    chunk_len = MIN(AES_MAX_CHUNK_WRITE_SIZE, len);

    if (realloc_input) {
        input_buf = heap_caps_malloc(chunk_len, MALLOC_CAP_DMA);

        if (input_buf == NULL) {
            mbedtls_platform_zeroize(output, len);
            ESP_LOGE(TAG, "Failed to allocate memory");
            return -1;
        }
    }

    if (realloc_output) {
        output_buf = heap_caps_malloc(chunk_len, MALLOC_CAP_DMA);

        if (output_buf == NULL) {
            mbedtls_platform_zeroize(output, len);
            ESP_LOGE(TAG, "Failed to allocate memory");
            return -1;
        }
    } else {
        output_buf = output;
    }

    while (len) {
        chunk_len = MIN(AES_MAX_CHUNK_WRITE_SIZE, len);

        /* If input needs realloc then copy it, else use the input with offset*/
        if (realloc_input) {
            memcpy(input_buf, input + offset, chunk_len);
            dma_input = input_buf;
        } else {
            dma_input = input + offset;
        }

        if (esp_aes_process_dma(ctx, dma_input, output_buf, chunk_len, stream_out) != 0) {
            ret = -1;
            goto cleanup;
        }

        if (realloc_output) {
            memcpy(output + offset, output_buf, chunk_len);
        } else {
            output_buf = output + offset + chunk_len;
        }

        len -= chunk_len;
        offset += chunk_len;
    }

cleanup:

    if (realloc_input) {
        free(input_buf);
    }
    if (realloc_output) {
        free(output_buf);
    }

    return ret;
}

/** Append a descriptor to the chain, set head if chain empty
 *
 * @param[out] head Pointer to the first/head node of the DMA descriptor linked list
 * @param item Pointer to the DMA descriptor node that has to be appended
 */
static inline void dma_desc_append(crypto_dma_desc_t **head, crypto_dma_desc_t *item)
{
    crypto_dma_desc_t *it;
    if (*head == NULL) {
        *head = item;
        return;
    }

    it = *head;

    while (it->next != 0) {
        it = (crypto_dma_desc_t *)it->next;
    }
    it->dw0.suc_eof = 0;
    it->next = item;

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    ESP_ERROR_CHECK(esp_cache_msync(it, sizeof(crypto_dma_desc_t), ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED));
#endif
}

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE

#define ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))
#define ALIGN_DOWN(num, align)  ((num) & ~((align) - 1))
#define AES_DMA_ALLOC_CAPS      (MALLOC_CAP_DMA | MALLOC_CAP_8BIT)

static inline void *aes_dma_calloc(size_t num, size_t size, uint32_t caps, size_t *actual_size)
{
    void *ptr = NULL;
    esp_dma_mem_info_t dma_mem_info = {
        .extra_heap_caps = caps,
        .dma_alignment_bytes = DMA_DESC_MEM_ALIGN_SIZE,
    };
    esp_dma_capable_calloc(num, size, &dma_mem_info, &ptr, actual_size);
    return ptr;
}

static inline size_t get_cache_line_size(const void *addr)
{
    esp_err_t ret = ESP_FAIL;
    size_t cache_line_size = 0;

#if (CONFIG_SPIRAM && SOC_PSRAM_DMA_CAPABLE)
    if (esp_ptr_external_ram(addr)) {
        ret = esp_cache_get_alignment(MALLOC_CAP_SPIRAM, &cache_line_size);
    } else
#endif
    {
        ret = esp_cache_get_alignment(MALLOC_CAP_DMA, &cache_line_size);
    }

    if (ret != ESP_OK) {
        return 0;
    }

    return cache_line_size;
}

static inline esp_err_t dma_desc_link(crypto_dma_desc_t *dmadesc, size_t crypto_dma_desc_num, size_t cache_line_size)
{
    esp_err_t ret = ESP_OK;
    for (int i = 0; i < crypto_dma_desc_num; i++) {
        dmadesc[i].dw0.suc_eof = ((i == crypto_dma_desc_num - 1) ? 1 : 0);
        dmadesc[i].next = ((i == crypto_dma_desc_num - 1) ? NULL : &dmadesc[i+1]);
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        /*  Write back both input buffers and output buffers to clear any cache dirty bit if set
            If we want to remove `ESP_CACHE_MSYNC_FLAG_UNALIGNED` aligned flag then we need to pass
            cache msync size = ALIGN_UP(dma_desc.size, cache_line_size), instead of dma_desc.size
            Keeping the `ESP_CACHE_MSYNC_FLAG_UNALIGNED` flag just because it should not look like
            we are syncing extra bytes due to ALIGN_UP'ed size but just the number of bytes that
            are needed in the operation. */
        ret = esp_cache_msync(dmadesc[i].buffer, dmadesc[i].dw0.length, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
        if (ret != ESP_OK) {
            return ret;
        }
    }
    ret = esp_cache_msync(dmadesc, ALIGN_UP(crypto_dma_desc_num * sizeof(crypto_dma_desc_t), cache_line_size), ESP_CACHE_MSYNC_FLAG_DIR_C2M);
#else
    }
#endif
    return ret;
}

static inline void dma_desc_populate(crypto_dma_desc_t *dmadesc, const uint8_t *data, size_t len, int max_desc_size, size_t index)
{
    int dmachunklen = 0;

    while (len) {
        dmachunklen = len;
        if (dmachunklen > max_desc_size) {
            dmachunklen = max_desc_size;
        }
        dmadesc[index].dw0.size = dmachunklen;
        dmadesc[index].dw0.length = dmachunklen;
        dmadesc[index].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        dmadesc[index].buffer = (void *)data;
        len -= dmachunklen;
        data += dmachunklen;
        index++;
    }
}

/**
 * @brief Function that allocates and populates a given number of DMA descriptors to form a DMA descriptor linked list
 *
 * @param buffer Data (can be unaligned) buffer that is to be operated upon in an AES operation (ciphertext or plaintext)
 * @param len Length of the above data buffer
 * @param start_alignment_buffer The buffer which the first DMA descriptor points to for processing start_alignment length of bytes from the above buffer
 * @param end_alignment_buffer The buffer which the last DMA descriptor points to for processing end_alignment length of bytes from the above buffer
 * @param alignment_buffer_size Size of an alignment buffer
 * @param cache_line_size Size of cache line that is needed to align the buffers and DMA descriptors before cache sync
 * @param[out] start_alignment The least number of bytes from the start of the buffer that are unaligned to the Cache requirements (L1 Cache alignments)
 * @param[out] end_alignment The number of bytes at the end of the buffer aligned up to AES_BLOCK_BYTES that are unaligned to the Cache requirements (L1 Cache alignments)
 * @param[out] dma_descs Pointer to the list of DMA descriptors that are needed to be populated
 * @param[out] dma_desc_num Number of DMA descriptors that are needed to be allocated
 */
static esp_err_t generate_descriptor_list(const uint8_t *buffer, const size_t len, uint8_t **start_alignment_buffer, uint8_t **end_alignment_buffer, size_t alignment_buffer_size, size_t cache_line_size, size_t *start_alignment, size_t *end_alignment, crypto_dma_desc_t **dma_descs, size_t *dma_desc_num, bool is_output)
{
    size_t unaligned_start_bytes = 0;
    size_t aligned_block_bytes = 0;
    size_t unaligned_end_bytes = 0;
    size_t dma_descs_needed = 0;

    uint8_t *start_alignment_stream_buffer = NULL;
    uint8_t *end_alignment_stream_buffer = NULL;

    crypto_dma_desc_t *dma_descriptors = NULL;

    if (len == 0) {
        goto ret;
    }

    /* Extra bytes that were needed to be processed for supplying the AES peripheral a padded multiple of 16 bytes input */
    size_t extra_bytes = ALIGN_UP(len, AES_BLOCK_BYTES) - len;

    size_t start_offset = ((intptr_t)buffer & (cache_line_size - 1));

    if (start_offset) {
        unaligned_start_bytes = cache_line_size - start_offset;
    } else {
        unaligned_start_bytes = 0;
    }

    if (unaligned_start_bytes < len) {
        aligned_block_bytes = ALIGN_DOWN((len - unaligned_start_bytes), cache_line_size);
        unaligned_end_bytes = len - unaligned_start_bytes - aligned_block_bytes + extra_bytes;
    } else {
        unaligned_start_bytes = len + extra_bytes;
        unaligned_end_bytes = 0;
        aligned_block_bytes = 0;
    }

    size_t max_desc_size = (is_output) ? ALIGN_DOWN(DMA_DESCRIPTOR_BUFFER_MAX_SIZE_16B_ALIGNED, cache_line_size) : ALIGN_DOWN(DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED, cache_line_size);

    dma_descs_needed = (unaligned_start_bytes ? 1 : 0) + dma_desc_get_required_num(aligned_block_bytes, max_desc_size) + (unaligned_end_bytes ? 1 : 0);

    /* Allocate memory for DMA descriptors of total size aligned up to a multiple of cache line size */
    dma_descriptors = (crypto_dma_desc_t *) aes_dma_calloc(dma_descs_needed, sizeof(crypto_dma_desc_t), MALLOC_CAP_DMA, NULL);
    if (dma_descriptors == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for the array of DMA descriptors");
        return ESP_FAIL;
    }

    size_t populated_dma_descs = 0;

    if (unaligned_start_bytes) {
        start_alignment_stream_buffer = aes_dma_calloc(alignment_buffer_size, sizeof(uint8_t), AES_DMA_ALLOC_CAPS, NULL);
        if (start_alignment_stream_buffer == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for start alignment buffer");
            return ESP_FAIL;
        }

        memset(start_alignment_stream_buffer, 0, unaligned_start_bytes);
        memcpy(start_alignment_stream_buffer, buffer, (unaligned_start_bytes > len) ? len : unaligned_start_bytes);
        memset(start_alignment_stream_buffer + unaligned_start_bytes, 0, alignment_buffer_size - unaligned_start_bytes);

        // add start alignment node to the DMA linked list
        dma_desc_populate(dma_descriptors, start_alignment_stream_buffer, unaligned_start_bytes, max_desc_size, populated_dma_descs);
        populated_dma_descs += (unaligned_start_bytes ? 1 : 0);
    }

    if (aligned_block_bytes) {
        // add "aligned_dma_desc_num" nodes to DMA linked list
        dma_desc_populate(dma_descriptors, buffer + unaligned_start_bytes, aligned_block_bytes, max_desc_size, populated_dma_descs);
        populated_dma_descs += dma_desc_get_required_num(aligned_block_bytes, max_desc_size);
    }

    if (unaligned_end_bytes) {
        end_alignment_stream_buffer = aes_dma_calloc(alignment_buffer_size, sizeof(uint8_t), AES_DMA_ALLOC_CAPS, NULL);
        if (end_alignment_stream_buffer == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for end alignment buffer");
            return ESP_FAIL;
        }

        memset(end_alignment_stream_buffer, 0, unaligned_end_bytes);
        memcpy(end_alignment_stream_buffer, buffer + unaligned_start_bytes + aligned_block_bytes, unaligned_end_bytes - extra_bytes);
        memset(end_alignment_stream_buffer + unaligned_end_bytes, 0, alignment_buffer_size - unaligned_end_bytes);

        // add end alignment node to the DMA linked list
        dma_desc_populate(dma_descriptors, end_alignment_stream_buffer, unaligned_end_bytes, max_desc_size, populated_dma_descs);
        populated_dma_descs += (unaligned_end_bytes ? 1 : 0);
    }

    if (dma_desc_link(dma_descriptors, dma_descs_needed, cache_line_size) != ESP_OK) {
        ESP_LOGE(TAG, "DMA descriptors cache sync C2M failed");
        return ESP_FAIL;
    }

ret:
    if (start_alignment != NULL) {
        *start_alignment = unaligned_start_bytes;
    }

    if (end_alignment != NULL) {
        *end_alignment = unaligned_end_bytes;
    }

    if (dma_desc_num != NULL) {
        *dma_desc_num = dma_descs_needed;
    }

    *dma_descs = dma_descriptors;
    *start_alignment_buffer = start_alignment_stream_buffer;
    *end_alignment_buffer = end_alignment_stream_buffer;

    return ESP_OK;
}

int esp_aes_process_dma(esp_aes_context *ctx, const unsigned char *input, unsigned char *output, size_t len, uint8_t *stream_out)
{
    unsigned stream_bytes = len % AES_BLOCK_BYTES; // bytes which aren't in a full block
    unsigned block_bytes = len - stream_bytes;     // bytes which are in a full block
    unsigned blocks = (block_bytes / AES_BLOCK_BYTES) + ((stream_bytes > 0) ? 1 : 0);

    bool use_intr = false;
    bool input_needs_realloc = false;
    bool output_needs_realloc = false;
    int ret = 0;

    assert(len > 0); // caller shouldn't ever have len set to zero
    assert(stream_bytes == 0 || stream_out != NULL); // stream_out can be NULL if we're processing full block(s)

    /* If no key is written to hardware yet, either the user hasn't called
       mbedtls_aes_setkey_enc/mbedtls_aes_setkey_dec - meaning we also don't
       know which mode to use - or a fault skipped the
       key write to hardware. Treat this as a fatal error and zero the output block.
    */
    if (ctx->key_in_hardware != ctx->key_bytes) {
        mbedtls_platform_zeroize(output, len);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    /* DMA cannot access memory in the iCache range, copy input to internal ram */
    if (!s_check_dma_capable(input)) {
        input_needs_realloc = true;
    }

    if (!s_check_dma_capable(output)) {
        output_needs_realloc = true;
    }

    /* If either input or output is unaccessible to the DMA then they need to be reallocated */
    if (input_needs_realloc || output_needs_realloc) {
        return esp_aes_process_dma_ext_ram(ctx, input, output, len, stream_out, input_needs_realloc, output_needs_realloc);
    }

    size_t input_cache_line_size = get_cache_line_size(input);
    size_t output_cache_line_size = get_cache_line_size(output);

    if (input_cache_line_size == 0 || output_cache_line_size == 0) {
        mbedtls_platform_zeroize(output, len);
        ESP_LOGE(TAG, "Getting cache line size failed");
        return -1;
    }

    size_t input_alignment_buffer_size = MAX(2 * input_cache_line_size, AES_BLOCK_BYTES);

    crypto_dma_desc_t *input_desc = NULL;
    uint8_t *input_start_stream_buffer = NULL;
    uint8_t *input_end_stream_buffer = NULL;

    if (generate_descriptor_list(input, len, &input_start_stream_buffer, &input_end_stream_buffer, input_alignment_buffer_size, input_cache_line_size, NULL, NULL, &input_desc, NULL, false) != ESP_OK) {
        mbedtls_platform_zeroize(output, len);
        ESP_LOGE(TAG, "Generating input DMA descriptors failed");
        return -1;
    }

    size_t output_alignment_buffer_size = MAX(2 * output_cache_line_size, AES_BLOCK_BYTES);

    crypto_dma_desc_t *output_desc = NULL;
    uint8_t *output_start_stream_buffer = NULL;
    uint8_t *output_end_stream_buffer = NULL;
    size_t output_start_alignment = 0;
    size_t output_end_alignment = 0;
    size_t output_dma_desc_num = 0;

    if (generate_descriptor_list(output, len, &output_start_stream_buffer, &output_end_stream_buffer, output_alignment_buffer_size, output_cache_line_size, &output_start_alignment, &output_end_alignment, &output_desc, &output_dma_desc_num, true) != ESP_OK) {
        mbedtls_platform_zeroize(output, len);
        ESP_LOGE(TAG, "Generating output DMA descriptors failed");
        return -1;
    }

    crypto_dma_desc_t *out_desc_tail = &output_desc[output_dma_desc_num - 1];

#if defined (CONFIG_MBEDTLS_AES_USE_INTERRUPT)
    /* Only use interrupt for long AES operations */
    if (len > AES_DMA_INTR_TRIG_LEN) {
        use_intr = true;
        if (esp_aes_isr_initialise() != ESP_OK) {
            ESP_LOGE(TAG, "ESP-AES ISR initialisation failed");
            ret = -1;
            goto cleanup;
        }
    } else
#endif
    {
        aes_hal_interrupt_enable(false);
    }

    if (esp_aes_dma_start(input_desc, output_desc) != ESP_OK) {
        ESP_LOGE(TAG, "esp_aes_dma_start failed, no DMA channel available");
        ret = -1;
        goto cleanup;
    }

    aes_hal_transform_dma_start(blocks);

    if (esp_aes_dma_wait_complete(use_intr, out_desc_tail) < 0) {
        ESP_LOGE(TAG, "esp_aes_dma_wait_complete failed");
        ret = -1;
        goto cleanup;
    }

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    if (esp_cache_msync(output_desc, ALIGN_UP(output_dma_desc_num * sizeof(crypto_dma_desc_t), output_cache_line_size), ESP_CACHE_MSYNC_FLAG_DIR_M2C) != ESP_OK) {
        ESP_LOGE(TAG, "Output DMA descriptor cache sync M2C failed");
        ret = -1;
        goto cleanup;
    }
    for (int i = 0; i < output_dma_desc_num; i++) {
        if (esp_cache_msync(output_desc[i].buffer, ALIGN_UP(output_desc[i].dw0.length, output_cache_line_size), ESP_CACHE_MSYNC_FLAG_DIR_M2C) != ESP_OK) {
            ESP_LOGE(TAG, "Output DMA descriptor buffers cache sync M2C failed");
            ret = -1;
            goto cleanup;
        }
    }
#endif

    aes_hal_transform_dma_finish();

    /* Extra bytes that were needed to be processed for supplying the AES peripheral a padded multiple of 16 bytes input */
    size_t extra_bytes = ALIGN_UP(len, AES_BLOCK_BYTES) - len;

    if (output_start_alignment) {
        memcpy(output, output_start_stream_buffer, (output_start_alignment > len) ? len : output_start_alignment);
    }

    if (output_end_alignment) {
        memcpy(output + len - (output_end_alignment - extra_bytes), output_end_stream_buffer, output_end_alignment - extra_bytes);
    }

    if (stream_bytes > 0) {
        if (output_end_alignment) {
            if (output_end_alignment >= AES_BLOCK_BYTES) {
                memcpy(stream_out, output_end_stream_buffer + output_end_alignment - AES_BLOCK_BYTES, AES_BLOCK_BYTES);
            } else {
                size_t to_copy_from_output = AES_BLOCK_BYTES - output_end_alignment;
                memcpy(stream_out, output + len - to_copy_from_output, to_copy_from_output);
                memcpy(stream_out + to_copy_from_output, output_end_stream_buffer, output_end_alignment);
            }
        }
        else if (output_start_alignment >= len) {
            memcpy(stream_out, output_start_stream_buffer + output_start_alignment - AES_BLOCK_BYTES, AES_BLOCK_BYTES);
        }
    }

cleanup:
    if (ret != 0) {
        mbedtls_platform_zeroize(output, len);
    }

    free(input_start_stream_buffer);
    free(input_end_stream_buffer);

    free(output_start_stream_buffer);
    free(output_end_stream_buffer);

    free(input_desc);
    free(output_desc);

    return ret;
}

#if CONFIG_MBEDTLS_HARDWARE_GCM

/* Encrypt/decrypt with AES-GCM the input using DMA
 * The function esp_aes_process_dma_gcm zeroises the output buffer in the case of following conditions:
 * 1. If key is not written in the hardware
 * 2. Memory allocation failures
 * 3. If AES interrupt is enabled and ISR initialisation fails
 * 4. Failure in any of the AES operations
 */
int esp_aes_process_dma_gcm(esp_aes_context *ctx, const unsigned char *input, unsigned char *output, size_t len, const unsigned char *aad, size_t aad_len)
{
    int ret = 0;
    bool use_intr = false;

    /* If no key is written to hardware yet, either the user hasn't called
       mbedtls_aes_setkey_enc/mbedtls_aes_setkey_dec - meaning we also don't
       know which mode to use - or a fault skipped the
       key write to hardware. Treat this as a fatal error and zero the output block.
    */
    if (ctx->key_in_hardware != ctx->key_bytes) {
        mbedtls_platform_zeroize(output, len);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    unsigned stream_bytes = len % AES_BLOCK_BYTES; // bytes which aren't in a full block
    unsigned block_bytes = len - stream_bytes;     // bytes which are in a full block

    unsigned blocks = (block_bytes / AES_BLOCK_BYTES) + ((stream_bytes > 0) ? 1 : 0);

    size_t aad_cache_line_size = get_cache_line_size(aad);
    size_t input_cache_line_size = get_cache_line_size(input);
    size_t output_cache_line_size = get_cache_line_size(output);

    if (aad_cache_line_size == 0 || input_cache_line_size == 0 || output_cache_line_size == 0) {
        mbedtls_platform_zeroize(output, len);
        ESP_LOGE(TAG, "Getting cache line size failed");
        return -1;
    }

    crypto_dma_desc_t *in_desc_head = NULL;
    crypto_dma_desc_t *out_desc_tail = NULL; /* pointer to the final output descriptor */
    crypto_dma_desc_t *aad_desc = NULL, *len_desc = NULL;
    crypto_dma_desc_t *input_desc = NULL;
    crypto_dma_desc_t *output_desc = NULL;

    size_t aad_alignment_buffer_size = MAX(2 * aad_cache_line_size, AES_BLOCK_BYTES);

    uint8_t *aad_start_stream_buffer = NULL;
    uint8_t *aad_end_stream_buffer = NULL;
    size_t aad_dma_desc_num = 0;

    if (generate_descriptor_list(aad, aad_len, &aad_start_stream_buffer, &aad_end_stream_buffer, aad_alignment_buffer_size, aad_cache_line_size, NULL, NULL, &aad_desc, &aad_dma_desc_num, false) != ESP_OK) {
        mbedtls_platform_zeroize(output, len);
        ESP_LOGE(TAG, "Generating aad DMA descriptors failed");
        return -1;
    }

    dma_desc_append(&in_desc_head, aad_desc);

    size_t input_alignment_buffer_size = MAX(2 * input_cache_line_size, AES_BLOCK_BYTES);

    uint8_t *input_start_stream_buffer = NULL;
    uint8_t *input_end_stream_buffer = NULL;
    size_t input_dma_desc_num = 0;

    if (generate_descriptor_list(input, len, &input_start_stream_buffer, &input_end_stream_buffer, input_alignment_buffer_size, input_cache_line_size, NULL, NULL, &input_desc, &input_dma_desc_num, false) != ESP_OK) {
        mbedtls_platform_zeroize(output, len);
        ESP_LOGE(TAG, "Generating input DMA descriptors failed");
        return -1;
    }

    dma_desc_append(&in_desc_head, input_desc);

    size_t output_alignment_buffer_size = MAX(2 * output_cache_line_size, AES_BLOCK_BYTES);

    uint8_t *output_start_stream_buffer = NULL;
    uint8_t *output_end_stream_buffer = NULL;

    size_t output_start_alignment = 0;
    size_t output_end_alignment = 0;
    size_t output_dma_desc_num = 0;

    if (generate_descriptor_list(output, len, &output_start_stream_buffer, &output_end_stream_buffer, output_alignment_buffer_size, output_cache_line_size, &output_start_alignment, &output_end_alignment, &output_desc, &output_dma_desc_num, true) != ESP_OK) {
        mbedtls_platform_zeroize(output, len);
        ESP_LOGE(TAG, "Generating output DMA descriptors failed");
        return -1;
    }

    out_desc_tail = &output_desc[output_dma_desc_num - 1];

    len_desc = aes_dma_calloc(1, sizeof(crypto_dma_desc_t), MALLOC_CAP_DMA, NULL);
    if (len_desc == NULL) {
        mbedtls_platform_zeroize(output, len);
        ESP_LOGE(TAG, "Failed to allocate memory for len descriptor");
        return -1;
    }

    uint32_t *len_buf = aes_dma_calloc(4, sizeof(uint32_t), MALLOC_CAP_DMA, NULL);
    if (len_buf == NULL) {
        mbedtls_platform_zeroize(output, len);
        ESP_LOGE(TAG, "Failed to allocate memory for len buffer");
        return -1;
    }

    len_buf[1] = __builtin_bswap32(aad_len * 8);
    len_buf[3] = __builtin_bswap32(len * 8);

    len_desc->dw0.length = 4 * sizeof(uint32_t);
    len_desc->dw0.size = 4 * sizeof(uint32_t);
    len_desc->dw0.owner = 1;
    len_desc->dw0.suc_eof = 1;
    len_desc->buffer = (void *) len_buf;
    len_desc->next = NULL;

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    if (esp_cache_msync(len_desc->buffer, len_desc->dw0.length, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED) != ESP_OK) {
        ESP_LOGE(TAG, "Length DMA descriptor cache sync C2M failed");
        ret = -1;
        goto cleanup;
    }
    if (esp_cache_msync(len_desc, sizeof(crypto_dma_desc_t), ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED) != ESP_OK) {
        ESP_LOGE(TAG, "Length DMA descriptor cache sync C2M failed");
        ret = -1;
        goto cleanup;
    }
#endif

    dma_desc_append(&in_desc_head, len_desc);

#if defined (CONFIG_MBEDTLS_AES_USE_INTERRUPT)
    /* Only use interrupt for long AES operations */
    if (len > AES_DMA_INTR_TRIG_LEN) {
        use_intr = true;
        if (esp_aes_isr_initialise() != ESP_OK) {
            ESP_LOGE(TAG, "ESP-AES ISR initialisation failed");
            ret = -1;
            goto cleanup;
        }
    } else
#endif
    {
        aes_hal_interrupt_enable(false);
    }

    /* Start AES operation */
    if (esp_aes_dma_start(in_desc_head, output_desc) != ESP_OK) {
        ESP_LOGE(TAG, "esp_aes_dma_start failed, no DMA channel available");
        ret = -1;
        goto cleanup;
    }

    aes_hal_transform_dma_gcm_start(blocks);

    if (esp_aes_dma_wait_complete(use_intr, out_desc_tail) < 0) {
        ESP_LOGE(TAG, "esp_aes_dma_wait_complete failed");
        ret = -1;
        goto cleanup;
    }

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    if (esp_cache_msync(output_desc, ALIGN_UP(output_dma_desc_num * sizeof(crypto_dma_desc_t), output_cache_line_size), ESP_CACHE_MSYNC_FLAG_DIR_M2C) != ESP_OK) {
        ESP_LOGE(TAG, "Output DMA descriptor cache sync M2C failed");
        ret = -1;
        goto cleanup;
    }
    for (int i = 0; i < output_dma_desc_num; i++) {
        if (esp_cache_msync(output_desc[i].buffer, ALIGN_UP(output_desc[i].dw0.length, output_cache_line_size), ESP_CACHE_MSYNC_FLAG_DIR_M2C) != ESP_OK) {
            ESP_LOGE(TAG, "Output DMA descriptor buffers cache sync M2C failed");
            ret = -1;
            goto cleanup;
        }
    }
#endif

    aes_hal_transform_dma_finish();

    /* Extra bytes that were needed to be processed for supplying the AES peripheral a padded multiple of 16 bytes input */
    size_t extra_bytes = ALIGN_UP(len, AES_BLOCK_BYTES) - len;

    if (output_start_alignment) {
        memcpy(output, output_start_stream_buffer, (output_start_alignment > len) ? len : output_start_alignment);
    }

    if (output_end_alignment) {
        memcpy(output + len - (output_end_alignment - extra_bytes), output_end_stream_buffer, output_end_alignment - extra_bytes);
    }

cleanup:
    if (ret != 0) {
        mbedtls_platform_zeroize(output, len);
    }

    free(aad_start_stream_buffer);
    free(aad_end_stream_buffer);
    free(aad_desc);

    free(input_start_stream_buffer);
    free(input_end_stream_buffer);
    free(input_desc);

    free(output_start_stream_buffer);
    free(output_end_stream_buffer);
    free(output_desc);

    free(len_buf);
    free(len_desc);

    return ret;
}

#endif //CONFIG_MBEDTLS_HARDWARE_GCM

#else /* SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE */

/* These are static due to:
 *  * Must be in DMA capable memory, so stack is not a safe place to put them
 *  * To avoid having to malloc/free them for every DMA operation
 */
static DRAM_ATTR crypto_dma_desc_t s_stream_in_desc;
static DRAM_ATTR crypto_dma_desc_t s_stream_out_desc;
static DRAM_ATTR uint8_t s_stream_in[AES_BLOCK_BYTES];
static DRAM_ATTR uint8_t s_stream_out[AES_BLOCK_BYTES];

/**
 * Generate a linked list pointing to a (huge) buffer in an descriptor array.
 *
 * The caller should ensure there is enough size to hold the array, by calling
 * `dma_desc_get_required_num` with the same or less than the max_desc_size argument.
 *
 * @param[out] dmadesc Output of a descriptor array, the head should be fed to the DMA.
 * @param data Buffer for the descriptors to point to.
 * @param len Size (or length for TX) of the buffer
 * @param max_desc_size Maximum length of each descriptor
 * @param isrx The RX DMA may require the buffer to be word-aligned, set to true for a RX link, otherwise false.
 */
static inline void dma_desc_setup_link(crypto_dma_desc_t* dmadesc, const uint8_t *data, int len, int max_desc_size, bool isrx)
{
    int i = 0;
    while (len) {
        int dmachunklen = len;
        if (dmachunklen > max_desc_size) {
            dmachunklen = max_desc_size;
        }
        if (isrx) {
            //Receive needs DMA length rounded to next 32-bit boundary
            dmadesc[i].dw0.size = (dmachunklen + 3) & (~3);
            dmadesc[i].dw0.length = (dmachunklen + 3) & (~3);
        } else {
            dmadesc[i].dw0.size = dmachunklen;
            dmadesc[i].dw0.length = dmachunklen;
        }
        dmadesc[i].buffer = (void *)data;
        dmadesc[i].dw0.suc_eof = 0;
        dmadesc[i].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        dmadesc[i].next = &dmadesc[i + 1];
        len -= dmachunklen;
        data += dmachunklen;
        i++;
    }
    dmadesc[i - 1].dw0.suc_eof = 1; //Mark last DMA desc as end of stream.
    dmadesc[i - 1].next = NULL;
}

/* Encrypt/decrypt the input using DMA
 * The function esp_aes_process_dma zeroises the output buffer in the case of following conditions:
 * 1. If key is not written in the hardware
 * 2. Memory allocation failures
 * 3. If AES interrupt is enabled and ISR initialisation fails
 * 4. Failure in any of the AES operations
 */
int esp_aes_process_dma(esp_aes_context *ctx, const unsigned char *input, unsigned char *output, size_t len, uint8_t *stream_out)
{
    crypto_dma_desc_t *in_desc_head = NULL, *out_desc_head = NULL;
    crypto_dma_desc_t *out_desc_tail = NULL; /* pointer to the final output descriptor */
    crypto_dma_desc_t *block_desc = NULL, *block_in_desc = NULL, *block_out_desc = NULL;
    size_t crypto_dma_desc_num = 0;
    unsigned stream_bytes = len % AES_BLOCK_BYTES; // bytes which aren't in a full block
    unsigned block_bytes = len - stream_bytes;     // bytes which are in a full block
    unsigned blocks = (block_bytes / AES_BLOCK_BYTES) + ((stream_bytes > 0) ? 1 : 0);
    bool use_intr = false;
    bool input_needs_realloc = false;
    bool output_needs_realloc = false;
    int ret = 0;

    assert(len > 0); // caller shouldn't ever have len set to zero
    assert(stream_bytes == 0 || stream_out != NULL); // stream_out can be NULL if we're processing full block(s)

    /* If no key is written to hardware yet, either the user hasn't called
       mbedtls_aes_setkey_enc/mbedtls_aes_setkey_dec - meaning we also don't
       know which mode to use - or a fault skipped the
       key write to hardware. Treat this as a fatal error and zero the output block.
    */
    if (ctx->key_in_hardware != ctx->key_bytes) {
        mbedtls_platform_zeroize(output, len);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    if (block_bytes > 0) {
        /* Flush cache if input in external ram */
#if (CONFIG_SPIRAM && SOC_PSRAM_DMA_CAPABLE)
        if (esp_ptr_external_ram(input)) {
            if (esp_cache_msync((void *)input, len, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED) != ESP_OK) {
                mbedtls_platform_zeroize(output, len);
                ESP_LOGE(TAG, "Cache sync failed for the input in external RAM");
                return -1;
            }
        }
        if (esp_ptr_external_ram(output)) {
            size_t dcache_line_size;
            ret = esp_cache_get_alignment(MALLOC_CAP_SPIRAM, &dcache_line_size);
            if (ret != ESP_OK) {
                return ret;
            }
            if ((((intptr_t)(output) & (dcache_line_size - 1)) != 0) || (block_bytes % dcache_line_size != 0)) {
                // Non aligned ext-mem buffer
                output_needs_realloc = true;
            }
        }
#endif
        /* DMA cannot access memory in the iCache range, copy input to internal ram */
        if (!s_check_dma_capable(input)) {
            input_needs_realloc = true;
        }

        if (!s_check_dma_capable(output)) {
            output_needs_realloc = true;
        }

        /* If either input or output is unaccessible to the DMA then they need to be reallocated */
        if (input_needs_realloc || output_needs_realloc) {
            return esp_aes_process_dma_ext_ram(ctx, input, output, len, stream_out, input_needs_realloc, output_needs_realloc);
        }

        /* Set up dma descriptors for input and output considering the 16 byte alignment requirement for EDMA */
        crypto_dma_desc_num = dma_desc_get_required_num(block_bytes, DMA_DESCRIPTOR_BUFFER_MAX_SIZE_16B_ALIGNED);

        /* Allocate both in and out descriptors to save a malloc/free per function call */
        block_desc = heap_caps_aligned_calloc(8, crypto_dma_desc_num * 2, sizeof(crypto_dma_desc_t), MALLOC_CAP_DMA);
        if (block_desc == NULL) {
            mbedtls_platform_zeroize(output, len);
            ESP_LOGE(TAG, "Failed to allocate memory");
            return -1;
        }

        block_in_desc = block_desc;
        block_out_desc = block_desc + crypto_dma_desc_num;

        // the size field has 12 bits, but 0 not for 4096.
        // to avoid possible problem when the size is not word-aligned, we only use 4096-4 per desc.
        // Maximum size of data in the buffer that a DMA descriptor can hold.
        dma_desc_setup_link(block_in_desc, input, block_bytes, DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED, 0);

        //Limit max inlink descriptor length to be 16 byte aligned, require for EDMA
        dma_desc_setup_link(block_out_desc, output, block_bytes, DMA_DESCRIPTOR_BUFFER_MAX_SIZE_16B_ALIGNED, 0);

        /* Setup in/out start descriptors */
        dma_desc_append(&in_desc_head, block_in_desc);
        dma_desc_append(&out_desc_head, block_out_desc);

        out_desc_tail = &block_out_desc[crypto_dma_desc_num - 1];
    }

    /* Any leftover bytes which are appended as an additional DMA list */
    if (stream_bytes > 0) {

        memset(&s_stream_in_desc, 0, sizeof(crypto_dma_desc_t));
        memset(&s_stream_out_desc, 0, sizeof(crypto_dma_desc_t));

        memset(s_stream_in, 0, AES_BLOCK_BYTES);
        memset(s_stream_out, 0, AES_BLOCK_BYTES);

        memcpy(s_stream_in, input + block_bytes, stream_bytes);

        dma_desc_setup_link(&s_stream_in_desc, s_stream_in, AES_BLOCK_BYTES, DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED, 0);
        dma_desc_setup_link(&s_stream_out_desc, s_stream_out, AES_BLOCK_BYTES, DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED, 0);

        /* Link with block descriptors */
        dma_desc_append(&in_desc_head, &s_stream_in_desc);
        dma_desc_append(&out_desc_head, &s_stream_out_desc);

        out_desc_tail = &s_stream_out_desc;
    }

#if defined (CONFIG_MBEDTLS_AES_USE_INTERRUPT)
    /* Only use interrupt for long AES operations */
    if (len > AES_DMA_INTR_TRIG_LEN) {
        use_intr = true;
        if (esp_aes_isr_initialise() != ESP_OK) {
            ESP_LOGE(TAG, "ESP-AES ISR initialisation failed");
            ret = -1;
            goto cleanup;
        }
    } else
#endif
    {
        aes_hal_interrupt_enable(false);
    }

    if (esp_aes_dma_start(in_desc_head, out_desc_head) != ESP_OK) {
        ESP_LOGE(TAG, "esp_aes_dma_start failed, no DMA channel available");
        ret = -1;
        goto cleanup;
    }

    aes_hal_transform_dma_start(blocks);

    if (esp_aes_dma_wait_complete(use_intr, out_desc_tail) < 0) {
        ESP_LOGE(TAG, "esp_aes_dma_wait_complete failed");
        ret = -1;
        goto cleanup;
    }

#if (CONFIG_SPIRAM && SOC_PSRAM_DMA_CAPABLE)
    if (block_bytes > 0) {
        if (esp_ptr_external_ram(output)) {
            if(esp_cache_msync((void*)output, block_bytes, ESP_CACHE_MSYNC_FLAG_DIR_M2C) != ESP_OK) {
                mbedtls_platform_zeroize(output, len);
                ESP_LOGE(TAG, "Cache sync failed for the output in external RAM");
                return -1;
            }
        }
    }
#endif
    aes_hal_transform_dma_finish();

    if (stream_bytes > 0) {
        memcpy(output + block_bytes, s_stream_out, stream_bytes);
        memcpy(stream_out, s_stream_out, AES_BLOCK_BYTES);
    }

cleanup:
    if (ret != 0) {
        mbedtls_platform_zeroize(output, len);
    }
    free(block_desc);
    return ret;
}

#if CONFIG_MBEDTLS_HARDWARE_GCM

/* Encrypt/decrypt with AES-GCM the input using DMA
 * The function esp_aes_process_dma_gcm zeroises the output buffer in the case of following conditions:
 * 1. If key is not written in the hardware
 * 2. Memory allocation failures
 * 3. If AES interrupt is enabled and ISR initialisation fails
 * 4. Failure in any of the AES operations
 */
int esp_aes_process_dma_gcm(esp_aes_context *ctx, const unsigned char *input, unsigned char *output, size_t len, const unsigned char *aad, size_t aad_len)
{
    crypto_dma_desc_t aad_desc[2] = {};
    crypto_dma_desc_t *aad_head_desc = NULL;
    crypto_dma_desc_t *in_desc_head = NULL, *out_desc_head = NULL, *len_desc = NULL;
    crypto_dma_desc_t *out_desc_tail = NULL; /* pointer to the final output descriptor */
    crypto_dma_desc_t stream_in_desc, stream_out_desc;
    crypto_dma_desc_t *block_desc = NULL, *block_in_desc = NULL, *block_out_desc = NULL;

    uint8_t stream_in_aad[AES_BLOCK_BYTES] = {};
    unsigned stream_bytes_aad = aad_len % AES_BLOCK_BYTES;     // bytes which aren't in a full block
    unsigned block_bytes_aad = aad_len - stream_bytes_aad;     // bytes which are in a full block

    assert(esp_ptr_dma_capable(stream_in_aad));

    if (block_bytes_aad > 0) {
        aad_desc[0].dw0.length = block_bytes_aad;
        aad_desc[0].dw0.size = block_bytes_aad;
        aad_desc[0].dw0.owner = 1;
        aad_desc[0].buffer = (void*)aad;
    }

    if (stream_bytes_aad > 0) {
        memcpy(stream_in_aad, aad + block_bytes_aad, stream_bytes_aad);

        aad_desc[0].next = &aad_desc[1];
        aad_desc[1].dw0.length = AES_BLOCK_BYTES;
        aad_desc[1].dw0.size = AES_BLOCK_BYTES;
        aad_desc[1].dw0.owner = 1;
        aad_desc[1].buffer = (void*)stream_in_aad;
    }

    if (block_bytes_aad > 0) {
        aad_head_desc = &aad_desc[0];
    } else if (stream_bytes_aad > 0) {
        aad_head_desc = &aad_desc[1];
    }

    size_t crypto_dma_desc_num = 0;
    uint32_t len_buf[4] = {};
    uint8_t stream_in[16] = {};
    uint8_t stream_out[16] = {};
    unsigned stream_bytes = len % AES_BLOCK_BYTES; // bytes which aren't in a full block
    unsigned block_bytes = len - stream_bytes;     // bytes which are in a full block

    unsigned blocks = (block_bytes / AES_BLOCK_BYTES) + ((stream_bytes > 0) ? 1 : 0);

    bool use_intr = false;
    int ret = 0;

    /* If no key is written to hardware yet, either the user hasn't called
       mbedtls_aes_setkey_enc/mbedtls_aes_setkey_dec - meaning we also don't
       know which mode to use - or a fault skipped the
       key write to hardware. Treat this as a fatal error and zero the output block.
    */
    if (ctx->key_in_hardware != ctx->key_bytes) {
        mbedtls_platform_zeroize(output, len);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    /* Set up dma descriptors for input and output */
    crypto_dma_desc_num = dma_desc_get_required_num(block_bytes, DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED);

    /* Allocate both in and out descriptors to save a malloc/free per function call, add 1 for length descriptor */
    block_desc = heap_caps_calloc((crypto_dma_desc_num * 2) + 1, sizeof(crypto_dma_desc_t), MALLOC_CAP_DMA);
    if (block_desc == NULL) {
        mbedtls_platform_zeroize(output, len);
        ESP_LOGE(TAG, "Failed to allocate memory");
        return -1;
    }

    block_in_desc = block_desc;
    len_desc = block_desc + crypto_dma_desc_num;
    block_out_desc = block_desc + crypto_dma_desc_num + 1;

    if (aad_head_desc != NULL) {
        dma_desc_append(&in_desc_head, aad_head_desc);
    }

    if (block_bytes > 0) {
        dma_desc_setup_link(block_in_desc, input, block_bytes, DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED, 0);
        dma_desc_setup_link(block_out_desc, output, block_bytes, DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED, 0);

        dma_desc_append(&in_desc_head, block_in_desc);
        dma_desc_append(&out_desc_head, block_out_desc);

        out_desc_tail = &block_out_desc[crypto_dma_desc_num - 1];
    }

    /* Any leftover bytes which are appended as an additional DMA list */
    if (stream_bytes > 0) {
        memcpy(stream_in, input + block_bytes, stream_bytes);

        dma_desc_setup_link(&stream_in_desc, stream_in, AES_BLOCK_BYTES, DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED, 0);
        dma_desc_setup_link(&stream_out_desc, stream_out, AES_BLOCK_BYTES, DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED, 0);

        dma_desc_append(&in_desc_head, &stream_in_desc);
        dma_desc_append(&out_desc_head, &stream_out_desc);

        out_desc_tail = &stream_out_desc;
    }


    len_buf[1] = __builtin_bswap32(aad_len * 8);
    len_buf[3] = __builtin_bswap32(len * 8);

    len_desc->dw0.length = sizeof(len_buf);
    len_desc->dw0.size = sizeof(len_buf);
    len_desc->dw0.owner = 1;
    len_desc->dw0.suc_eof = 1;
    len_desc->buffer = (uint8_t *)len_buf;

    dma_desc_append(&in_desc_head, len_desc);

#if defined (CONFIG_MBEDTLS_AES_USE_INTERRUPT)
    /* Only use interrupt for long AES operations */
    if (len > AES_DMA_INTR_TRIG_LEN) {
        use_intr = true;
        if (esp_aes_isr_initialise() != ESP_OK) {
            ESP_LOGE(TAG, "ESP-AES ISR initialisation failed");
            ret = -1;
            goto cleanup;
        }
    } else
#endif
    {
        aes_hal_interrupt_enable(false);
    }

    /* Start AES operation */
    if (esp_aes_dma_start(in_desc_head, out_desc_head) != ESP_OK) {
        ESP_LOGE(TAG, "esp_aes_dma_start failed, no DMA channel available");
        ret = -1;
        goto cleanup;
    }

    aes_hal_transform_dma_gcm_start(blocks);

    if (esp_aes_dma_wait_complete(use_intr, out_desc_tail) < 0) {
        ESP_LOGE(TAG, "esp_aes_dma_wait_complete failed");
        ret = -1;
        goto cleanup;
    }

    aes_hal_transform_dma_finish();

    if (stream_bytes > 0) {
        memcpy(output + block_bytes, stream_out, stream_bytes);
    }

cleanup:
    if (ret != 0) {
        mbedtls_platform_zeroize(output, len);
    }
    free(block_desc);
    return ret;
}

#endif //CONFIG_MBEDTLS_HARDWARE_GCM
#endif /* SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE */
