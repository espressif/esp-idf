#ifndef PANIC_H
#define PANIC_H

#ifdef __cplusplus
extern "C"
{
#endif

#define PANIC_RSN_NONE 0
#define PANIC_RSN_DEBUGEXCEPTION 1
#define PANIC_RSN_DOUBLEEXCEPTION 2
#define PANIC_RSN_KERNELEXCEPTION 3
#define PANIC_RSN_COPROCEXCEPTION 4
#define PANIC_RSN_INTWDT_CPU0 5
#define PANIC_RSN_INTWDT_CPU1 6
#define PANIC_RSN_CACHEERR 7
#define PANIC_RSN_MAX 7


#ifndef __ASSEMBLER__

#include "esp_err.h"
#include "soc/soc.h"


/**
 * @brief If an OCD is connected over JTAG. set breakpoint 0 to the given function 
 *        address. Do nothing otherwise.
 * @param data  Pointer to the target breakpoint position
 */

void esp_set_breakpoint_if_jtag(void *fn);

#define ESP_WATCHPOINT_LOAD 0x40000000
#define ESP_WATCHPOINT_STORE 0x80000000
#define ESP_WATCHPOINT_ACCESS 0xC0000000

/**
 * @brief Set a watchpoint to break/panic when a certain memory range is accessed.
 *
 * @param no Watchpoint number. On the ESP32, this can be 0 or 1.
 * @param adr Base address to watch
 * @param size Size of the region, starting at the base address, to watch. Must
 *             be one of 2^n, with n in [0..6].
 * @param flags One of ESP_WATCHPOINT_* flags
 *
 * @return ESP_ERR_INVALID_ARG on invalid arg, ESP_OK otherwise
 *
 * @warning The ESP32 watchpoint hardware watches a region of bytes by effectively 
 *          masking away the lower n bits for a region with size 2^n. If adr does
 *          not have zero for these lower n bits, you may not be watching the
 *          region you intended.
 */
esp_err_t esp_set_watchpoint(int no, void *adr, int size, int flags);


/**
 * @brief Clear a watchpoint
 *
 * @param no Watchpoint to clear
 *
 */
void esp_clear_watchpoint(int no);

/**
 * @brief Checks stack pointer in dram
 */
inline static bool esp_stack_ptr_in_dram(uint32_t sp)
{
    //Check if stack ptr is in between SOC_DRAM_LOW and SOC_DRAM_HIGH, and 16 byte aligned.
    return !(sp < SOC_DRAM_LOW + 0x10 || sp > SOC_DRAM_HIGH - 0x10 || ((sp & 0xF) != 0));
}

#if CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
/**
 * @brief Checks stack pointer in external ram
 */
inline static bool esp_stack_ptr_in_extram(uint32_t sp)
{
    //Check if stack ptr is in between SOC_EXTRAM_DATA_LOW and SOC_EXTRAM_DATA_HIGH, and 16 byte aligned.
    return !(sp < SOC_EXTRAM_DATA_LOW + 0x10 || sp > SOC_EXTRAM_DATA_HIGH - 0x10 || ((sp & 0xF) != 0));
}
#endif

/**
 * @brief Checks stack pointer
 */
static inline bool esp_stack_ptr_is_sane(uint32_t sp)
{
#if CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
    return (esp_stack_ptr_in_dram(sp) || esp_stack_ptr_in_extram(sp));
#else
    return esp_stack_ptr_in_dram(sp);
#endif
}
#endif

#ifdef __cplusplus
}
#endif

#endif
