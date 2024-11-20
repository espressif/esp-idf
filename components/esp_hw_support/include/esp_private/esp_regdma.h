/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_assert.h"
#include "esp_macros.h"
#include "esp_err.h"
#include "esp_bit_defs.h"
#include "soc/soc_caps.h"
#include "soc/regdma.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PAU_SUPPORTED
#include "hal/pau_types.h"

/**
 * @brief Create a REGDMA continuous type linked list node without retention buffer and the retention buffer is passed in by the caller
 * @param  backup  Register address to be backed up by REGDMA
 * @param  buff    Retention buffer, it needs to be allocated by the caller and passed in by this argument
 * @param  len     Number of registers to be backed up
 * @param  restore Register address to be restored by REGDMA
 * @param  next    The next REGDMA linked list node
 * @param  skip_b  Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r  Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id      REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module  The module identifier of the current linked list node
 * @return         Created REGDMA linked list node pointer
 */
void *regdma_link_new_continuous(void *backup, void *buff, int len, void *restore, void *next, bool skip_b, bool skip_r, int id, int module);

/**
 * @brief Create a REGDMA addr_map type linked list node without retention buffer and the retention buffer is passed in by the caller
 * @param  backup  Register address to be backed up by REGDMA
 * @param  buff    Retention buffer, it needs to be allocated by the caller and passed in by this argument
 * @param  bitmap  The register bitmap that needs to be backed up and restored. when the bitmap corresponding to the
 *                 register is 1, it needs to be backed up or restored, otherwise the corresponding register is skipped.
 * @param  len     Number of registers to be backed up
 * @param  restore Register address to be restored by REGDMA
 * @param  next    The next REGDMA linked list node
 * @param  skip_b  Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r  Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id      REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module  The module identifier of the current linked list node
 * @return         Created REGDMA linked list node pointer
 */
void *regdma_link_new_addr_map(void *backup, void *buff, uint32_t bitmap[4], int len, void *restore, void *next, bool skip_b, bool skip_r, int id, int module);

/**
 * @brief  Create a REGDMA write type linked list node without retention buffer and the retention buffer is passed in by the caller
 * @param  backup Register address to be backed up by REGDMA
 * @param  value  The value to be written to the register
 * @param  mask   The mask of value
 * @param  next   The next REGDMA linked list node
 * @param  skip_b Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id     REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module The module identifier of the current linked list node
 * @return        Created REGDMA linked list node pointer
 */
void *regdma_link_new_write(void *backup, uint32_t value, uint32_t mask, void *next, bool skip_b, bool skip_r, int id, int module);

/**
 * @brief Create a REGDMA write type linked list node without retention buffer and the retention buffer is passed in by the caller
 * @param  backup Register address to be backed up by REGDMA
 * @param  value  The register value that needs to be waited for
 * @param  mask   The mask of value
 * @param  next   The next REGDMA linked list node
 * @param  skip_b Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id     REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module The module identifier of the current linked list node
 * @return        Created REGDMA linked list node pointer
 */
void *regdma_link_new_wait(void *backup, uint32_t value, uint32_t mask, void *next, bool skip_b, bool skip_r, int id, int module);

/**
 * @brief Create a REGDMA continuouos branch list node without retention buffer and the retention buffer is passed in by the caller
 * @param  backup  Register address to be backed up by REGDMA
 * @param  buff    Retention buffer, it needs to be allocated by the caller and passed in by this argument
 * @param  len     Number of registers to be backed up
 * @param  restore Register address to be restored by REGDMA
 * @param  next    The next REGDMA linked list node (supports up to 4 next pointers)
 * @param  skip_b  Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r  Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id      REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module  The module identifier of the current linked list node
 * @return         Created REGDMA linked list node pointer
 */
void *regdma_link_new_branch_continuous(void *backup, void *buff, int len, void *restore, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, int module);

/**
 * @brief Create a REGDMA addr_map branch list node without retention buffer and the retention buffer is passed in by the caller
 * @param  backup  Register address to be backed up by REGDMA
 * @param  buff    Retention buffer, it needs to be allocated by the caller and passed in by this argument
 * @param  bitmap  The register bitmap that needs to be backed up and restored. when the bitmap corresponding to the
 *                 register is 1, it needs to be backed up or restored, otherwise the corresponding register is skipped.
 * @param  len     Number of registers to be backed up
 * @param  restore Register address to be restored by REGDMA
 * @param  next    The next REGDMA linked list node (supports up to 4 next pointers)
 * @param  skip_b  Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r  Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id      REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module  The module identifier of the current linked list node
 * @return         Created REGDMA linked list node pointer
 */
void *regdma_link_new_branch_addr_map(void *backup, void *buff, uint32_t bitmap[4], int len, void *restore, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, int module);

/**
 * @brief Create a REGDMA write branch list node without retention buffer and the retention buffer is passed in by the caller
 * @param  backup Register address to be backed up by REGDMA
 * @param  value  The value to be written to the register
 * @param  mask   The mask of value
 * @param  next   The next REGDMA linked list node (supports up to 4 next pointers)
 * @param  skip_b Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id     REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module The module identifier of the current linked list node
 * @return        Created REGDMA linked list node pointer
 */
void *regdma_link_new_branch_write(void *backup, uint32_t value, uint32_t mask, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, int module);

/**
 * @brief Create a REGDMA wait branch list node without retention buffer and the retention buffer is passed in by the caller
 * @param  backup Register address to be backed up by REGDMA
 * @param  value  The register value that needs to be waited for
 * @param  mask   The mask of value
 * @param  next   The next REGDMA linked list node (supports up to 4 next pointers)
 * @param  skip_b Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id     REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module The module identifier of the current linked list node
 * @return        Created REGDMA linked list node pointer
 */
void *regdma_link_new_branch_wait(void *backup, uint32_t value, uint32_t mask, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, int module);

/**
 * @brief Create a default REGDMA continuous type linked list node with retention buffer
 * @param  backup  Register address to be backed up by REGDMA
 * @param  len     Number of registers to be backed up
 * @param  restore Register address to be restored by REGDMA
 * @param  next    The next REGDMA linked list node
 * @param  skip_b  Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r  Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id      REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module  The module identifier of the current linked list node
 * @return         Created REGDMA linked list node pointer
 */
void *regdma_link_new_continuous_default(void *backup, int len, void *restore, void *next, bool skip_b, bool skip_r, int id, int module);

/**
 * @brief Create a default REGDMA addr_map type linked list node with retention buffer
 * @param  backup  Register address to be backed up by REGDMA
 * @param  bitmap  The register bitmap that needs to be backed up and restored. when the bitmap corresponding to the
 *                 register is 1, it needs to be backed up or restored, otherwise the corresponding register is skipped.
 * @param  len     Number of registers to be backed up
 * @param  restore Register address to be restored by REGDMA
 * @param  next    The next REGDMA linked list node
 * @param  skip_b  Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r  Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id      REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module  The module identifier of the current linked list node
 * @return         Created REGDMA linked list node pointer
 */
void *regdma_link_new_addr_map_default(void *backup, uint32_t bitmap[4], int len, void *restore, void *next, bool skip_b, bool skip_r, int id, int module);

/**
 * @brief Create a default REGDMA write type linked list node with retention buffer
 * @param  backup Register address to be backed up by REGDMA
 * @param  value  The register value that needs to be waited for
 * @param  mask   The mask of value
 * @param  next   The next REGDMA linked list node
 * @param  skip_b Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id     REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module The module identifier of the current linked list node
 * @return        Created REGDMA linked list node pointer
 */
void *regdma_link_new_write_default(void *backup, uint32_t value, uint32_t mask, void *next, bool skip_b, bool skip_r, int id, int module);

/**
 * @brief Create a default REGDMA wait type linked list node with retention buffer
 * @param  backup Register address to be backed up by REGDMA
 * @param  value  The register value that needs to be waited for
 * @param  mask   The mask of value
 * @param  next   The next REGDMA linked list node
 * @param  skip_b Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id     REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module The module identifier of the current linked list node
 * @return        Created REGDMA linked list node pointer
 */
void *regdma_link_new_wait_default(void *backup, uint32_t value, uint32_t mask, void *next, bool skip_b, bool skip_r, int id, int module);

/**
 * @brief Create a default REGDMA continuous branch list node with retention buffer
 * @param  backup  Register address to be backed up by REGDMA
 * @param  len     Number of registers to be backed up
 * @param  restore Register address to be restored by REGDMA
 * @param  next    The next REGDMA linked list node (supports up to 4 next pointers)
 * @param  skip_b  Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r  Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id      REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module  The module identifier of the current linked list node
 * @return         Created REGDMA linked list node pointer
 */
void *regdma_link_new_branch_continuous_default(void *backup, int len, void *restore, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, int module);

/**
 * @brief Create a default REGDMA addr_map branch list node with retention buffer
 * @param  backup  Register address to be backed up by REGDMA
 * @param  bitmap  The register bitmap that needs to be backed up and restored. when the bitmap corresponding to the
 *                 register is 1, it needs to be backed up or restored, otherwise the corresponding register is skipped.
 * @param  len     Number of registers to be backed up
 * @param  restore Register address to be restored by REGDMA
 * @param  next    The next REGDMA linked list node (supports up to 4 next pointers)
 * @param  skip_b  Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r  Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id      REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module  The module identifier of the current linked list node
 * @return         Created REGDMA linked list node pointer
 */
void *regdma_link_new_branch_addr_map_default(void *backup, uint32_t bitmap[4], int len, void *restore, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, int module);

/**
 * @brief Create a default REGDMA write branch list node with retention buffer
 * @param  backup Register address to be backed up by REGDMA
 * @param  value  The value to be written to the register
 * @param  mask   The mask of value
 * @param  next   The next REGDMA linked list node (supports up to 4 next pointers)
 * @param  skip_b Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id     REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module The module identifier of the current linked list node
 * @return        Created REGDMA linked list node pointer
 */
void *regdma_link_new_branch_write_default(void *backup, uint32_t value, uint32_t mask, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, int module);

/**
 * @brief Create a default REGDMA wait branch list node with retention buffer
 * @param  backup Register address to be backed up by REGDMA
 * @param  value  The register value that needs to be waited for
 * @param  mask   The mask of value
 * @param  next   The next REGDMA linked list node (supports up to 4 next pointers)
 * @param  skip_b Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id     REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module The module identifier of the current linked list node
 * @return        Created REGDMA linked list node pointer
 */
void *regdma_link_new_branch_wait_default(void *backup, uint32_t value, uint32_t mask, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, int module);

/**
 * @brief Create and initialize a REGDMA linked list node through configuration parameters
 * @param  config REGDMA linked node configuration parameters
 * @param  branch Is it a branch node
 * @param  module The module identifier of the current linked list node
 * @param  nentry The number of next pointers
 * @param  args   next pointer, Since REGDMA supports 4 entries, it supports up to 4 variable parameter next pointers, and more will be ignored
 * @return        Initialized REGDMA linked list head node pointer
 */
void *regdma_link_init(const regdma_link_config_t *config, bool branch, int module, int nentry, ...);

/**
 * @brief Get REGDMA linked list node mode through configuration parameters
 * @param  config REGDMA linked node configuration parameters
 * @return        REGDMA linked list node mode
 */
regdma_link_mode_t regdma_link_get_config_mode(const regdma_link_config_t *config);

/**
 * @brief Recurse the REGDMA linked list and call the hook subroutine for each node
 * @param  link  The REGDMA linkded list head pointer
 * @param  entry For nodes that support branching, use the branch specified by entry argument recursively
 * @param  hook  Subroutines called during recursion, argument 1 is the pointer to the
 *               recursive node object, argument 2 is the entry to which the node belongs
 *               and the argument 3 is the position of the node in the current linked
 *               list (from head to tail, the position of the head node is 0)
 * @return       The REGDMA linked list node pointer indicated by the link argument
 */
void *regdma_link_recursive(void *link, int entry, void (*hook)(void *, int, int));

/**
 * @brief Find the linked list node object by node position
 * @param  link  The REGDMA linkded list head pointer
 * @param  entry For nodes that support branching, use the branch specified by entry argument recursively
 * @param  pos   Node position
 * @return       The linked list node object pointer or NULL
 */
void *regdma_find_link_by_pos(void *link, int entry, int pos);

/**
 * @brief Find the linked list node object by node unique identifier
 * @param  link  The REGDMA linkded list head pointer
 * @param  entry For nodes that support branching, use the branch specified by entry argument recursively
 * @param  id    REGDMA linked list node unique identifier
 * @return       The linked list node object pointer or NULL
 */
void *regdma_find_link_by_id(void *link, int entry, int id);

/**
 * @brief Destroy the REGDMA linked list indicated by the entry argument
 * @param link  The REGDMA linkded list head pointer
 * @param entry For nodes that support branching, use the branch specified by entry argument recursively
 */
void regdma_link_destroy(void *link, int entry);

/**
 * @brief Generate the statistics information of the REGDMA linked list indicated by the entry argument
 * @param link  The REGDMA linkded list head pointer
 * @param entry For nodes that support branching, use the branch specified by entry argument recursively
 */
void regdma_link_stats(void *link, int entry);

/**
 * @brief Set the value and mask of write or wait type REGDMA linked list node
 * @param link  Write or wait type REGDMA linked list node pointer
 * @param value The value to be written to the register
 * @param mask  The mask of value
 */
void regdma_link_set_write_wait_content(void *link, uint32_t value, uint32_t mask);

/**
 * @brief Dump all node information of the REGDMA linked list indicated by the entry argument
 * @param link  The REGDMA linkded list head pointer
 * @param entry For nodes that support branching, use the branch specified by entry argument recursively
 */
void regdma_link_dump(FILE *out, void *link, int entry);

/**
 * @brief Update REGDMA linked list node next pointers
 * @param link  The pointer of the REGDMA linked list node whose next field will be modified
 * @param nentry The number of next pointers
 */
void regdma_link_update_next(void *link, int nentry, ...);

/**
 * @brief Get all node entry reference bitmaps from the start of the link argument to the
 * end of the tail argument in the REGDMA linked list indicated by the entry argument
 * @param  link  The REGDMA linkded list head pointer
 * @param  tail  The REGDMA linkded list tail pointer
 * @param  entry For nodes that support branching, use the branch specified by entry argument recursively
 * @return       The entry reference bitmap of all nodes starting from the link argument to the end of the tail argument
 */
uint32_t regdma_link_get_owner_bitmap(void *link, void *tail, int entry);

/**
 * @brief Find the head node of the specified module in the REGDMA linked list indicated by the
 * entry argument starting from the link argument to the end of the tail argument
 * @param  link   The REGDMA linkded list head pointer
 * @param  tail   The REGDMA linkded list tail pointer
 * @param  entry  For nodes that support branching, use the branch specified by entry argument recursively
 * @param  module Module bitmap Identification
 * @return        The found head node pointer or NULL
 */
void *regdma_find_module_link_head(void *link, void *tail, int entry, int module);

/**
 * @brief Find the tail node of the specified module in the REGDMA linked list indicated by the
 * entry argument starting from the link argument to the end of the tail argument
 * @param  link   The REGDMA linkded list head pointer
 * @param  tail   The REGDMA linkded list tail pointer
 * @param  entry  For nodes that support branching, use the branch specified by entry argument recursively
 * @param  module Module bitmap Identification
 * @return        The found tail node pointer or NULL
 */
void *regdma_find_module_link_tail(void *link, void *tail, int entry, int module);

/**
 * @brief Find the tail node of the previous module of the specified module in the REGDMA linked list
 * indicated by the entry argument starting from the link argument to the end of the tail argument
 * @param  link   The REGDMA linkded list head pointer
 * @param  tail   The REGDMA linkded list tail pointer
 * @param  entry  For nodes that support branching, use the branch specified by entry argument recursively
 * @param  module Module bitmap Identification
 * @return        The found tail node pointer or NULL
 */
void *regdma_find_prev_module_link_tail(void *link, void *tail, int entry, int module);

/**
 * @brief Find the head node of the next module of the specified module in the REGDMA linked list
 * indicated by the entry argument starting from the link argument to the end of the tail argument
 * @param  link   The REGDMA linkded list head pointer
 * @param  tail   The REGDMA linkded list tail pointer
 * @param  entry  For nodes that support branching, use the branch specified by entry argument recursively
 * @param  module Module bitmap Identification
 * @return        The found head node pointer or NULL
 */
void *regdma_find_next_module_link_head(void *link, void *tail, int entry, int module);

#define regdma_link_init_safe(pcfg, branch, module, ...)    regdma_link_init((pcfg), (branch), (module), ESP_VA_NARG(__VA_ARGS__), ##__VA_ARGS__)

#define regdma_link_update_next_safe(link, ...)             regdma_link_update_next((link), ESP_VA_NARG(__VA_ARGS__), ##__VA_ARGS__)

#endif // SOC_PAU_SUPPORTED

#ifdef __cplusplus
}
#endif
