/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_EXTMEM_STRUCT_H_
#define _SOC_EXTMEM_STRUCT_H_


#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct extmem_dev_s {
    union {
        struct {
            uint32_t dcache_enable                 :    1;  /*The bit is used to activate the data cache. 0: disable, 1: enable*/
            uint32_t reserved1                     :    1;  /*Reserved*/
            uint32_t dcache_size_mode              :    1;  /*The bit is used to configure cache memory size.0: 32KB, 1: 64KB*/
            uint32_t dcache_blocksize_mode         :    2;  /*The bit is used to configure cache block size.0: 16 bytes, 1: 32 bytes,2: 64 bytes*/
            uint32_t reserved5                     :    27;
        };
        uint32_t val;
    } dcache_ctrl;
    union {
        struct {
            uint32_t dcache_shut_core0_bus         :    1;  /*The bit is used to disable core0 dbus, 0: enable, 1: disable*/
            uint32_t dcache_shut_core1_bus         :    1;  /*The bit is used to disable core1 dbus, 0: enable, 1: disable*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } dcache_ctrl1;
    union {
        struct {
            uint32_t dcache_tag_mem_force_on       :    1;  /*The bit is used to close clock gating of dcache tag memory. 1: close gating, 0: open clock gating.*/
            uint32_t dcache_tag_mem_force_pd       :    1;  /*The bit is used to power dcache tag memory down, 0: follow  rtc_lslp_pd, 1: power down*/
            uint32_t dcache_tag_mem_force_pu       :    1;  /*The bit is used to power dcache tag memory up, 0: follow  rtc_lslp_pd, 1: power up*/
            uint32_t reserved3                     :    29;
        };
        uint32_t val;
    } dcache_tag_power_ctrl;
    union {
        struct {
            uint32_t dcache_prelock_sct0_en        :    1;  /*The bit is used to enable the first section of prelock function.*/
            uint32_t dcache_prelock_sct1_en        :    1;  /*The bit is used to enable the second section of prelock function.*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } dcache_prelock_ctrl;
    uint32_t dcache_prelock_sct0_addr;
    uint32_t dcache_prelock_sct1_addr;
    union {
        struct {
            uint32_t dcache_prelock_sct1_size      :    16;  /*The bits are used to configure the second length of data locking, which is combined with DCACHE_PRELOCK_SCT1_ADDR_REG*/
            uint32_t dcache_prelock_sct0_size      :    16;  /*The bits are used to configure the first length of data locking, which is combined with DCACHE_PRELOCK_SCT0_ADDR_REG*/
        };
        uint32_t val;
    } dcache_prelock_sct_size;
    union {
        struct {
            uint32_t dcache_lock_ena               :    1;  /*The bit is used to enable lock operation. It will be cleared by hardware after lock operation done.*/
            uint32_t dcache_unlock_ena             :    1;  /*The bit is used to enable unlock operation. It will be cleared by hardware after unlock operation done.*/
            uint32_t dcache_lock_done              :    1;  /*The bit is used to indicate unlock/lock operation is finished.*/
            uint32_t reserved3                     :    29;
        };
        uint32_t val;
    } dcache_lock_ctrl;
    uint32_t dcache_lock_addr;
    union {
        struct {
            uint32_t dcache_lock_size              :    16;  /*The bits are used to configure the length for lock operations. The bits are the counts of cache block. It should be combined with DCACHE_LOCK_ADDR_REG.*/
            uint32_t reserved16                    :    16;
        };
        uint32_t val;
    } dcache_lock_size;
    union {
        struct {
            uint32_t dcache_invalidate_ena         :    1;  /*The bit is used to enable invalidate operation. It will be cleared by hardware after invalidate operation done.*/
            uint32_t dcache_writeback_ena          :    1;  /*The bit is used to enable writeback operation. It will be cleared by hardware after writeback operation done.*/
            uint32_t dcache_clean_ena              :    1;  /*The bit is used to enable clean operation. It will be cleared by hardware after clean operation done.*/
            uint32_t dcache_sync_done              :    1;  /*The bit is used to indicate clean/writeback/invalidate operation is finished.*/
            uint32_t reserved4                     :    28;
        };
        uint32_t val;
    } dcache_sync_ctrl;
    uint32_t dcache_sync_addr;
    union {
        struct {
            uint32_t dcache_sync_size              :    23;  /*The bits are used to configure the length for sync operations. The bits are the counts of cache block. It should be combined with DCACHE_SYNC_ADDR_REG.*/
            uint32_t reserved23                    :    9;
        };
        uint32_t val;
    } dcache_sync_size;
    union {
        struct {
            uint32_t dcache_occupy_ena             :    1;  /*The bit is used to enable occupy operation. It will be cleared by hardware after issuing Auot-Invalidate Operation.*/
            uint32_t dcache_occupy_done            :    1;  /*The bit is used to indicate occupy operation is finished.*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } dcache_occupy_ctrl;
    uint32_t dcache_occupy_addr;
    union {
        struct {
            uint32_t dcache_occupy_size            :    16;  /*The bits are used to configure the length for occupy operation. The bits are the counts of cache block. It should be combined with DCACHE_OCCUPY_ADDR_REG.*/
            uint32_t reserved16                    :    16;
        };
        uint32_t val;
    } dcache_occupy_size;
    union {
        struct {
            uint32_t dcache_preload_ena            :    1;  /*The bit is used to enable preload operation. It will be cleared by hardware after preload operation done.*/
            uint32_t dcache_preload_done           :    1;  /*The bit is used to indicate preload operation is finished.*/
            uint32_t dcache_preload_order          :    1;  /*The bit is used to configure the direction of preload operation. 1: descending, 0: ascending.*/
            uint32_t reserved3                     :    29;
        };
        uint32_t val;
    } dcache_preload_ctrl;
    uint32_t dcache_preload_addr;
    union {
        struct {
            uint32_t dcache_preload_size           :    16;  /*The bits are used to configure the length for preload operation. The bits are the counts of cache block. It should be combined with DCACHE_PRELOAD_ADDR_REG..*/
            uint32_t reserved16                    :    16;
        };
        uint32_t val;
    } dcache_preload_size;
    union {
        struct {
            uint32_t dcache_autoload_sct0_ena      :    1;  /*The bits are used to enable the first section for autoload operation.*/
            uint32_t dcache_autoload_sct1_ena      :    1;  /*The bits are used to enable the second section for autoload operation.*/
            uint32_t dcache_autoload_ena           :    1;  /*The bit is used to enable and disable autoload operation. It is combined with dcache_autoload_done. 1: enable, 0: disable. */
            uint32_t dcache_autoload_done          :    1;  /*The bit is used to indicate autoload operation is finished.*/
            uint32_t dcache_autoload_order         :    1;  /*The bits are used to configure the direction of autoload. 1: descending, 0: ascending.*/
            uint32_t dcache_autoload_rqst          :    2;  /*The bits are used to configure trigger conditions for autoload. 0/3: cache miss, 1: cache hit, 2: both cache miss and hit.*/
            uint32_t dcache_autoload_size          :    2;  /*The bits are used to configure the numbers of the cache block for the issuing autoload operation.*/
            uint32_t dcache_autoload_buffer_clear  :    1;  /*The bit is used to clear autoload buffer in dcache.*/
            uint32_t reserved10                    :    22;
        };
        uint32_t val;
    } dcache_autoload_ctrl;
    uint32_t dcache_autoload_sct0_addr;
    union {
        struct {
            uint32_t dcache_autoload_sct0_size     :    27;  /*The bits are used to configure the length of the first section for autoload operation. It should be combined with dcache_autoload_sct0_ena.*/
            uint32_t reserved27                    :    5;
        };
        uint32_t val;
    } dcache_autoload_sct0_size;
    uint32_t dcache_autoload_sct1_addr;
    union {
        struct {
            uint32_t dcache_autoload_sct1_size     :    27;  /*The bits are used to configure the length of the second section for autoload operation. It should be combined with dcache_autoload_sct1_ena.*/
            uint32_t reserved27                    :    5;
        };
        uint32_t val;
    } dcache_autoload_sct1_size;
    union {
        struct {
            uint32_t icache_enable                 :    1;  /*The bit is used to activate the data cache. 0: disable, 1: enable*/
            uint32_t icache_way_mode               :    1;  /*The bit is used to configure cache way mode.0: 4-way, 1: 8-way*/
            uint32_t icache_size_mode              :    1;  /*The bit is used to configure cache memory size.0: 16KB, 1: 32KB*/
            uint32_t icache_blocksize_mode         :    1;  /*The bit is used to configure cache block size.0: 16 bytes, 1: 32 bytes*/
            uint32_t reserved4                     :    28;
        };
        uint32_t val;
    } icache_ctrl;
    union {
        struct {
            uint32_t icache_shut_core0_bus         :    1;  /*The bit is used to disable core0 ibus, 0: enable, 1: disable*/
            uint32_t icache_shut_core1_bus         :    1;  /*The bit is used to disable core1 ibus, 0: enable, 1: disable*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } icache_ctrl1;
    union {
        struct {
            uint32_t icache_tag_mem_force_on       :    1;  /*The bit is used to close clock gating of  icache tag memory. 1: close gating, 0: open clock gating.*/
            uint32_t icache_tag_mem_force_pd       :    1;  /*The bit is used to power  icache tag memory down, 0: follow rtc_lslp, 1: power down*/
            uint32_t icache_tag_mem_force_pu       :    1;  /*The bit is used to power  icache tag memory up, 0: follow rtc_lslp, 1: power up*/
            uint32_t reserved3                     :    29;
        };
        uint32_t val;
    } icache_tag_power_ctrl;
    union {
        struct {
            uint32_t icache_prelock_sct0_en        :    1;  /*The bit is used to enable the first section of prelock function.*/
            uint32_t icache_prelock_sct1_en        :    1;  /*The bit is used to enable the second section of prelock function.*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } icache_prelock_ctrl;
    uint32_t icache_prelock_sct0_addr;
    uint32_t icache_prelock_sct1_addr;
    union {
        struct {
            uint32_t icache_prelock_sct1_size      :    16;  /*The bits are used to configure the second length of data locking, which is combined with ICACHE_PRELOCK_SCT1_ADDR_REG*/
            uint32_t icache_prelock_sct0_size      :    16;  /*The bits are used to configure the first length of data locking, which is combined with ICACHE_PRELOCK_SCT0_ADDR_REG*/
        };
        uint32_t val;
    } icache_prelock_sct_size;
    union {
        struct {
            uint32_t icache_lock_ena               :    1;  /*The bit is used to enable lock operation. It will be cleared by hardware after lock operation done.*/
            uint32_t icache_unlock_ena             :    1;  /*The bit is used to enable unlock operation. It will be cleared by hardware after unlock operation done.*/
            uint32_t icache_lock_done              :    1;  /*The bit is used to indicate unlock/lock operation is finished.*/
            uint32_t reserved3                     :    29;
        };
        uint32_t val;
    } icache_lock_ctrl;
    uint32_t icache_lock_addr;
    union {
        struct {
            uint32_t icache_lock_size              :    16;  /*The bits are used to configure the length for lock operations. The bits are the counts of cache block. It should be combined with ICACHE_LOCK_ADDR_REG.*/
            uint32_t reserved16                    :    16;
        };
        uint32_t val;
    } icache_lock_size;
    union {
        struct {
            uint32_t icache_invalidate_ena         :    1;  /*The bit is used to enable invalidate operation. It will be cleared by hardware after invalidate operation done.*/
            uint32_t icache_sync_done              :    1;  /*The bit is used to indicate invalidate operation is finished.*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } icache_sync_ctrl;
    uint32_t icache_sync_addr;
    union {
        struct {
            uint32_t icache_sync_size              :    23;  /*The bits are used to configure the length for sync operations. The bits are the counts of cache block. It should be combined with ICACHE_SYNC_ADDR_REG.*/
            uint32_t reserved23                    :    9;
        };
        uint32_t val;
    } icache_sync_size;
    union {
        struct {
            uint32_t icache_preload_ena            :    1;  /*The bit is used to enable preload operation. It will be cleared by hardware after preload operation done.*/
            uint32_t icache_preload_done           :    1;  /*The bit is used to indicate preload operation is finished.*/
            uint32_t icache_preload_order          :    1;  /*The bit is used to configure the direction of preload operation. 1: descending, 0: ascending.*/
            uint32_t reserved3                     :    29;
        };
        uint32_t val;
    } icache_preload_ctrl;
    uint32_t icache_preload_addr;
    union {
        struct {
            uint32_t icache_preload_size           :    16;  /*The bits are used to configure the length for preload operation. The bits are the counts of cache block. It should be combined with ICACHE_PRELOAD_ADDR_REG..*/
            uint32_t reserved16                    :    16;
        };
        uint32_t val;
    } icache_preload_size;
    union {
        struct {
            uint32_t icache_autoload_sct0_ena      :    1;  /*The bits are used to enable the first section for autoload operation.*/
            uint32_t icache_autoload_sct1_ena      :    1;  /*The bits are used to enable the second section for autoload operation.*/
            uint32_t icache_autoload_ena           :    1;  /*The bit is used to enable and disable autoload operation. It is combined with icache_autoload_done. 1: enable, 0: disable. */
            uint32_t icache_autoload_done          :    1;  /*The bit is used to indicate autoload operation is finished.*/
            uint32_t icache_autoload_order         :    1;  /*The bits are used to configure the direction of autoload. 1: descending, 0: ascending.*/
            uint32_t icache_autoload_rqst          :    2;  /*The bits are used to configure trigger conditions for autoload. 0/3: cache miss, 1: cache hit, 2: both cache miss and hit.*/
            uint32_t icache_autoload_size          :    2;  /*The bits are used to configure the numbers of the cache block for the issuing autoload operation.*/
            uint32_t icache_autoload_buffer_clear  :    1;  /*The bit is used to clear autoload buffer in icache.*/
            uint32_t reserved10                    :    22;
        };
        uint32_t val;
    } icache_autoload_ctrl;
    uint32_t icache_autoload_sct0_addr;
    union {
        struct {
            uint32_t icache_autoload_sct0_size     :    27;  /*The bits are used to configure the length of the first section for autoload operation. It should be combined with icache_autoload_sct0_ena.*/
            uint32_t reserved27                    :    5;
        };
        uint32_t val;
    } icache_autoload_sct0_size;
    uint32_t icache_autoload_sct1_addr;
    union {
        struct {
            uint32_t icache_autoload_sct1_size     :    27;  /*The bits are used to configure the length of the second section for autoload operation. It should be combined with icache_autoload_sct1_ena.*/
            uint32_t reserved27                    :    5;
        };
        uint32_t val;
    } icache_autoload_sct1_size;
    uint32_t ibus_to_flash_start_vaddr;
    uint32_t ibus_to_flash_end_vaddr;
    uint32_t dbus_to_flash_start_vaddr;
    uint32_t dbus_to_flash_end_vaddr;
    union {
        struct {
            uint32_t dcache_acs_cnt_clr            :    1;  /*The bit is used to clear dcache counter.*/
            uint32_t icache_acs_cnt_clr            :    1;  /*The bit is used to clear icache counter.*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } cache_acs_cnt_clr;
    uint32_t ibus_acs_miss_cnt;
    uint32_t ibus_acs_cnt;
    uint32_t dbus_acs_flash_miss_cnt;
    uint32_t dbus_acs_spiram_miss_cnt;
    uint32_t dbus_acs_cnt;
    union {
        struct {
            uint32_t icache_sync_op_fault          :    1;  /*The bit is used to enable interrupt by sync configurations fault.*/
            uint32_t icache_preload_op_fault       :    1;  /*The bit is used to enable interrupt by preload configurations fault.*/
            uint32_t dcache_sync_op_fault          :    1;  /*The bit is used to enable interrupt by sync configurations fault.*/
            uint32_t dcache_preload_op_fault       :    1;  /*The bit is used to enable interrupt by preload configurations fault.*/
            uint32_t dcache_write_flash            :    1;  /*The bit is used to enable interrupt by dcache trying to write flash.*/
            uint32_t mmu_entry_fault               :    1;  /*The bit is used to enable interrupt by mmu entry fault.*/
            uint32_t dcache_occupy_exc             :    1;  /*The bit is used to enable interrupt by dcache trying to replace a line whose blocks all have been occupied by occupy-mode.*/
            uint32_t ibus_cnt_ovf                  :    1;  /*The bit is used to enable interrupt by ibus counter overflow.*/
            uint32_t dbus_cnt_ovf                  :    1;  /*The bit is used to enable interrupt by dbus counter overflow.*/
            uint32_t reserved9                     :    23;
        };
        uint32_t val;
    } cache_ilg_int_ena;
    union {
        struct {
            uint32_t icache_sync_op_fault          :    1;  /*The bit is used to clear interrupt by sync configurations fault.*/
            uint32_t icache_preload_op_fault       :    1;  /*The bit is used to clear interrupt by preload configurations fault.*/
            uint32_t dcache_sync_op_fault          :    1;  /*The bit is used to clear interrupt by sync configurations fault.*/
            uint32_t dcache_preload_op_fault       :    1;  /*The bit is used to clear interrupt by preload configurations fault.*/
            uint32_t dcache_write_flash            :    1;  /*The bit is used to clear interrupt by dcache trying to write flash.*/
            uint32_t mmu_entry_fault               :    1;  /*The bit is used to clear interrupt by mmu entry fault.*/
            uint32_t dcache_occupy_exc             :    1;  /*The bit is used to clear interrupt by dcache trying to replace a line whose blocks all have been occupied by occupy-mode.*/
            uint32_t ibus_cnt_ovf                  :    1;  /*The bit is used to clear interrupt by ibus counter overflow.*/
            uint32_t dbus_cnt_ovf                  :    1;  /*The bit is used to clear interrupt by dbus counter overflow.*/
            uint32_t reserved9                     :    23;
        };
        uint32_t val;
    } cache_ilg_int_clr;
    union {
        struct {
            uint32_t icache_sync_op_fault_st       :    1;  /*The bit is used to indicate interrupt by sync configurations fault.*/
            uint32_t icache_preload_op_fault_st    :    1;  /*The bit is used to indicate interrupt by preload configurations fault.*/
            uint32_t dcache_sync_op_fault_st       :    1;  /*The bit is used to indicate interrupt by sync configurations fault.*/
            uint32_t dcache_preload_op_fault_st    :    1;  /*The bit is used to indicate interrupt by preload configurations fault.*/
            uint32_t dcache_write_flash_st         :    1;  /*The bit is used to indicate interrupt by dcache trying to write flash.*/
            uint32_t mmu_entry_fault_st            :    1;  /*The bit is used to indicate interrupt by mmu entry fault.*/
            uint32_t dcache_occupy_exc_st          :    1;  /*The bit is used to indicate interrupt by dcache trying to replace a line whose blocks all have been occupied by occupy-mode.*/
            uint32_t ibus_acs_cnt_ovf_st           :    1;  /*The bit is used to indicate interrupt by ibus access flash/spiram counter overflow.*/
            uint32_t ibus_acs_miss_cnt_ovf_st      :    1;  /*The bit is used to indicate interrupt by ibus access flash/spiram miss counter overflow.*/
            uint32_t dbus_acs_cnt_ovf_st           :    1;  /*The bit is used to indicate interrupt by dbus access flash/spiram counter overflow.*/
            uint32_t dbus_acs_flash_miss_cnt_ovf_st:    1;  /*The bit is used to indicate interrupt by dbus access flash miss counter overflow.*/
            uint32_t dbus_acs_spiram_miss_cnt_ovf_st:    1;  /*The bit is used to indicate interrupt by dbus access spiram miss counter overflow.*/
            uint32_t reserved12                    :    20;
        };
        uint32_t val;
    } cache_ilg_int_st;
    union {
        struct {
            uint32_t core0_ibus_acs_msk_ic         :    1;  /*The bit is used to enable interrupt by cpu access icache while the corresponding ibus is disabled which include speculative access.*/
            uint32_t core0_ibus_wr_ic              :    1;  /*The bit is used to enable interrupt by ibus trying to write icache*/
            uint32_t core0_ibus_reject             :    1;  /*The bit is used to enable interrupt by authentication fail.*/
            uint32_t core0_dbus_acs_msk_dc         :    1;  /*The bit is used to enable interrupt by cpu access dcache while the corresponding dbus is disabled which include speculative access.*/
            uint32_t core0_dbus_reject             :    1;  /*The bit is used to enable interrupt by authentication fail.*/
            uint32_t reserved5                     :    27;
        };
        uint32_t val;
    } core0_acs_cache_int_ena;
    union {
        struct {
            uint32_t core0_ibus_acs_msk_ic         :    1;  /*The bit is used to clear interrupt by cpu access icache while the corresponding ibus is disabled or icache is disabled which include speculative access.*/
            uint32_t core0_ibus_wr_ic              :    1;  /*The bit is used to clear interrupt by ibus trying to write icache*/
            uint32_t core0_ibus_reject             :    1;  /*The bit is used to clear interrupt by authentication fail.*/
            uint32_t core0_dbus_acs_msk_dc         :    1;  /*The bit is used to clear interrupt by cpu access dcache while the corresponding dbus is disabled or dcache is disabled which include speculative access.*/
            uint32_t core0_dbus_reject             :    1;  /*The bit is used to clear interrupt by authentication fail.*/
            uint32_t reserved5                     :    27;
        };
        uint32_t val;
    } core0_acs_cache_int_clr;
    union {
        struct {
            uint32_t core0_ibus_acs_msk_icache_st  :    1;  /*The bit is used to indicate interrupt by cpu access  icache while the core0_ibus is disabled or icache is disabled which include speculative access.*/
            uint32_t core0_ibus_wr_icache_st       :    1;  /*The bit is used to indicate interrupt by ibus trying to write icache*/
            uint32_t core0_ibus_reject_st          :    1;  /*The bit is used to indicate interrupt by authentication fail.*/
            uint32_t core0_dbus_acs_msk_dcache_st  :    1;  /*The bit is used to indicate interrupt by cpu access dcache while the core0_dbus is disabled or dcache is disabled which include speculative access.*/
            uint32_t core0_dbus_reject_st          :    1;  /*The bit is used to indicate interrupt by authentication fail.*/
            uint32_t reserved5                     :    27;
        };
        uint32_t val;
    } core0_acs_cache_int_st;
    union {
        struct {
            uint32_t core1_ibus_acs_msk_ic         :    1;  /*The bit is used to enable interrupt by cpu access icache while the corresponding ibus is disabled which include speculative access.*/
            uint32_t core1_ibus_wr_ic              :    1;  /*The bit is used to enable interrupt by ibus trying to write icache*/
            uint32_t core1_ibus_reject             :    1;  /*The bit is used to enable interrupt by authentication fail.*/
            uint32_t core1_dbus_acs_msk_dc         :    1;  /*The bit is used to enable interrupt by cpu access dcache while the corresponding dbus is disabled which include speculative access.*/
            uint32_t core1_dbus_reject             :    1;  /*The bit is used to enable interrupt by authentication fail.*/
            uint32_t reserved5                     :    27;
        };
        uint32_t val;
    } core1_acs_cache_int_ena;
    union {
        struct {
            uint32_t core1_ibus_acs_msk_ic         :    1;  /*The bit is used to clear interrupt by cpu access icache while the corresponding ibus is disabled or icache is disabled which include speculative access.*/
            uint32_t core1_ibus_wr_ic              :    1;  /*The bit is used to clear interrupt by ibus trying to write icache*/
            uint32_t core1_ibus_reject             :    1;  /*The bit is used to clear interrupt by authentication fail.*/
            uint32_t core1_dbus_acs_msk_dc         :    1;  /*The bit is used to clear interrupt by cpu access dcache while the corresponding dbus is disabled or dcache is disabled which include speculative access.*/
            uint32_t core1_dbus_reject             :    1;  /*The bit is used to clear interrupt by authentication fail.*/
            uint32_t reserved5                     :    27;
        };
        uint32_t val;
    } core1_acs_cache_int_clr;
    union {
        struct {
            uint32_t core1_ibus_acs_msk_icache_st  :    1;  /*The bit is used to indicate interrupt by cpu access  icache while the core1_ibus is disabled or  icache is disabled which include speculative access.*/
            uint32_t core1_ibus_wr_icache_st       :    1;  /*The bit is used to indicate interrupt by ibus trying to write icache*/
            uint32_t core1_ibus_reject_st          :    1;  /*The bit is used to indicate interrupt by authentication fail.*/
            uint32_t core1_dbus_acs_msk_dcache_st  :    1;  /*The bit is used to indicate interrupt by cpu access dcache while the core1_dbus is disabled or dcache is disabled which include speculative access.*/
            uint32_t core1_dbus_reject_st          :    1;  /*The bit is used to indicate interrupt by authentication fail.*/
            uint32_t reserved5                     :    27;
        };
        uint32_t val;
    } core1_acs_cache_int_st;
    union {
        struct {
            uint32_t core0_dbus_tag_attr           :    3;  /*The bits are used to indicate the attribute of data from external memory when authentication fail. 0: invalidate, 1: execute-able, 2: read-able, 4: write-able.*/
            uint32_t core0_dbus_attr               :    3;  /*The bits are used to indicate the attribute of CPU access dbus when authentication fail. 0: invalidate, 1: execute-able, 2: read-able, 4: write-able.*/
            uint32_t core0_dbus_world              :    1;  /*The bit is used to indicate the world of CPU access dbus when authentication fail. 0: WORLD0, 1: WORLD1*/
            uint32_t reserved7                     :    25;
        };
        uint32_t val;
    } core0_dbus_reject_st;
    uint32_t core0_dbus_reject_vaddr;
    union {
        struct {
            uint32_t core0_ibus_tag_attr           :    3;  /*The bits are used to indicate the attribute of data from external memory when authentication fail. 0: invalidate, 1: execute-able, 2: read-able, 4: write-able.*/
            uint32_t core0_ibus_attr               :    3;  /*The bits are used to indicate the attribute of CPU access ibus when authentication fail. 0: invalidate, 1: execute-able, 2: read-able*/
            uint32_t core0_ibus_world              :    1;  /*The bit is used to indicate the world of CPU access ibus when authentication fail. 0: WORLD0, 1: WORLD1*/
            uint32_t reserved7                     :    25;
        };
        uint32_t val;
    } core0_ibus_reject_st;
    uint32_t core0_ibus_reject_vaddr;
    union {
        struct {
            uint32_t core1_dbus_tag_attr           :    3;  /*The bits are used to indicate the attribute of data from external memory when authentication fail. 0: invalidate, 1: execute-able, 2: read-able, 4: write-able.*/
            uint32_t core1_dbus_attr               :    3;  /*The bits are used to indicate the attribute of CPU access dbus when authentication fail. 0: invalidate, 1: execute-able, 2: read-able, 4: write-able.*/
            uint32_t core1_dbus_world              :    1;  /*The bit is used to indicate the world of CPU access dbus when authentication fail. 0: WORLD0, 1: WORLD1*/
            uint32_t reserved7                     :    25;
        };
        uint32_t val;
    } core1_dbus_reject_st;
    uint32_t core1_dbus_reject_vaddr;
    union {
        struct {
            uint32_t core1_ibus_tag_attr           :    3;  /*The bits are used to indicate the attribute of data from external memory when authentication fail. 0: invalidate, 1: execute-able, 2: read-able, 4: write-able.*/
            uint32_t core1_ibus_attr               :    3;  /*The bits are used to indicate the attribute of CPU access ibus when authentication fail. 0: invalidate, 1: execute-able, 2: read-able*/
            uint32_t core1_ibus_world              :    1;  /*The bit is used to indicate the world of CPU access ibus when authentication fail. 0: WORLD0, 1: WORLD1*/
            uint32_t reserved7                     :    25;
        };
        uint32_t val;
    } core1_ibus_reject_st;
    uint32_t core1_ibus_reject_vaddr;
    union {
        struct {
            uint32_t cache_mmu_fault_content       :    16;  /*The bits are used to indicate the content of mmu entry which cause mmu fault..*/
            uint32_t cache_mmu_fault_code          :    4;  /*The right-most 3 bits are used to indicate the operations which cause mmu fault occurrence. 0: default, 1: cpu miss, 2: preload miss, 3: writeback, 4: cpu miss evict recovery address, 5: load miss evict recovery address, 6: external dma tx, 7: external dma rx. The most significant bit is used to indicate this operation occurs in which one icache. */
            uint32_t reserved20                    :    12;
        };
        uint32_t val;
    } cache_mmu_fault_content;
    uint32_t cache_mmu_fault_vaddr;
    union {
        struct {
            uint32_t cache_flash_wrap_around       :    1;  /*The bit is used to enable wrap around mode when read data from flash.*/
            uint32_t cache_sram_rd_wrap_around     :    1;  /*The bit is used to enable wrap around mode when read data from spiram.*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } cache_wrap_around_ctrl;
    union {
        struct {
            uint32_t cache_mmu_mem_force_on        :    1;  /*The bit is used to enable clock gating to save power when access mmu memory, 0: enable, 1: disable*/
            uint32_t cache_mmu_mem_force_pd        :    1;  /*The bit is used to power mmu memory down, 0: follow_rtc_lslp_pd, 1: power down*/
            uint32_t cache_mmu_mem_force_pu        :    1;  /*The bit is used to power mmu memory down, 0: follow_rtc_lslp_pd, 1: power up*/
            uint32_t reserved3                     :    29;
        };
        uint32_t val;
    } cache_mmu_power_ctrl;
    union {
        struct {
            uint32_t icache_state                  :    12;  /*The bit is used to indicate whether  icache main fsm is in idle state or not. 1: in idle state,  0: not in idle state*/
            uint32_t dcache_state                  :    12;  /*The bit is used to indicate whether dcache main fsm is in idle state or not. 1: in idle state,  0: not in idle state*/
            uint32_t reserved24                    :    8;
        };
        uint32_t val;
    } cache_state;
    union {
        struct {
            uint32_t record_disable_db_encrypt     :    1;  /*Reserved.*/
            uint32_t record_disable_g0cb_decrypt   :    1;  /*Reserved.*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } cache_encrypt_decrypt_record_disable;
    union {
        struct {
            uint32_t clk_force_on_manual_crypt     :    1;  /*The bit is used to close clock gating of manual crypt clock. 1: close gating, 0: open clock gating.*/
            uint32_t clk_force_on_auto_crypt       :    1;  /*The bit is used to close clock gating of automatic crypt clock. 1: close gating, 0: open clock gating.*/
            uint32_t clk_force_on_crypt            :    1;  /*The bit is used to close clock gating of external memory encrypt and decrypt clock. 1: close gating, 0: open clock gating.*/
            uint32_t reserved3                     :    29;
        };
        uint32_t val;
    } cache_encrypt_decrypt_clk_force_on;
    union {
        struct {
            uint32_t alloc_wb_hold_arbiter         :    1;  /*Reserved.*/
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } cache_bridge_arbiter_ctrl;
    union {
        struct {
            uint32_t icache_preload_ist            :    1;  /*The bit is used to indicate the interrupt by  icache pre-load done.*/
            uint32_t icache_preload_iena           :    1;  /*The bit is used to enable the interrupt by  icache pre-load done.*/
            uint32_t icache_preload_iclr           :    1;  /*The bit is used to clear the interrupt by  icache pre-load done.*/
            uint32_t dcache_preload_ist            :    1;  /*The bit is used to indicate the interrupt by dcache pre-load done.*/
            uint32_t dcache_preload_iena           :    1;  /*The bit is used to enable the interrupt by dcache pre-load done.*/
            uint32_t dcache_preload_iclr           :    1;  /*The bit is used to clear the interrupt by dcache pre-load done.*/
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } cache_preload_int_ctrl;
    union {
        struct {
            uint32_t icache_sync_ist               :    1;  /*The bit is used to indicate the interrupt by  icache sync done.*/
            uint32_t icache_sync_iena              :    1;  /*The bit is used to enable the interrupt by  icache sync done.*/
            uint32_t icache_sync_iclr              :    1;  /*The bit is used to clear the interrupt by  icache sync done.*/
            uint32_t dcache_sync_ist               :    1;  /*The bit is used to indicate the interrupt by dcache sync done.*/
            uint32_t dcache_sync_iena              :    1;  /*The bit is used to enable the interrupt by dcache sync done.*/
            uint32_t dcache_sync_iclr              :    1;  /*The bit is used to clear the interrupt by dcache sync done.*/
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } cache_sync_int_ctrl;
    union {
        struct {
            uint32_t cache_mmu_owner               :    24;  /*The bits are used to specify the owner of MMU.bit0: icache, bit1: dcache, bit2: dma, bit3: reserved.*/
            uint32_t reserved24                    :    8;
        };
        uint32_t val;
    } cache_mmu_owner;
    union {
        struct {
            uint32_t cache_ignore_preload_mmu_entry_fault:    1;  /*The bit is used to disable checking mmu entry fault by preload operation.*/
            uint32_t cache_ignore_sync_mmu_entry_fault:    1;  /*The bit is used to disable checking mmu entry fault by sync operation.*/
            uint32_t cache_trace_ena               :    1;  /*The bit is used to enable cache trace function.*/
            uint32_t reserved3                     :    29;
        };
        uint32_t val;
    } cache_conf_misc;
    union {
        struct {
            uint32_t dcache_freeze_ena             :    1;  /*The bit is used to enable dcache freeze mode*/
            uint32_t dcache_freeze_mode            :    1;  /*The bit is used to configure freeze mode, 0:  assert busy if CPU miss 1: assert hit if CPU miss*/
            uint32_t dcache_freeze_done            :    1;  /*The bit is used to indicate dcache freeze success*/
            uint32_t reserved3                     :    29;
        };
        uint32_t val;
    } dcache_freeze;
    union {
        struct {
            uint32_t icache_freeze_ena             :    1;  /*The bit is used to enable icache freeze mode*/
            uint32_t icache_freeze_mode            :    1;  /*The bit is used to configure freeze mode, 0:  assert busy if CPU miss 1: assert hit if CPU miss*/
            uint32_t icache_freeze_done            :    1;  /*The bit is used to indicate icache freeze success*/
            uint32_t reserved3                     :    29;
        };
        uint32_t val;
    } icache_freeze;
    union {
        struct {
            uint32_t icache_atomic_operate_ena     :    1;  /*The bit is used to activate icache atomic operation protection. In this case, sync/lock operation can not interrupt miss-work. This feature does not work during invalidateAll operation.*/
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } icache_atomic_operate_ena;
    union {
        struct {
            uint32_t dcache_atomic_operate_ena     :    1;  /*The bit is used to activate dcache atomic operation protection. In this case, sync/lock/occupy operation can not interrupt miss-work. This feature does not work during invalidateAll operation.*/
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } dcache_atomic_operate_ena;
    union {
        struct {
            uint32_t cache_request_bypass          :    1;  /*The bit is used to disable request recording which could cause performance issue*/
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } cache_request;
    union {
        struct {
            uint32_t clk_en                        :    1;  /*Reserved.*/
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } clock_gate;
    uint32_t reserved_168;
    uint32_t reserved_16c;
    uint32_t reserved_170;
    uint32_t reserved_174;
    uint32_t reserved_178;
    uint32_t reserved_17c;
    union {
        struct {
            uint32_t icache_tag_object             :    1;  /*Set this bit to set icache tag memory as object. This bit should be onehot with the others fields inside this register.*/
            uint32_t dcache_tag_object             :    1;  /*Set this bit to set dcache tag memory as object. This bit should be onehot with the others fields inside this register.*/
            uint32_t reserved2                     :    30;  /*Reserved*/
        };
        uint32_t val;
    } cache_tag_object_ctrl;
    union {
        struct {
            uint32_t cache_tag_way_object          :    3;  /*Set this bits to select which way of the tag-object will be accessed. 0: way0, 1: way1, 2: way2, 3: way3, .., 7: way7.*/
            uint32_t reserved3                     :    29;  /*Reserved*/
        };
        uint32_t val;
    } cache_tag_way_object;
    uint32_t cache_vaddr;
    uint32_t cache_tag_content;
    uint32_t reserved_190;
    uint32_t reserved_194;
    uint32_t reserved_198;
    uint32_t reserved_19c;
    uint32_t reserved_1a0;
    uint32_t reserved_1a4;
    uint32_t reserved_1a8;
    uint32_t reserved_1ac;
    uint32_t reserved_1b0;
    uint32_t reserved_1b4;
    uint32_t reserved_1b8;
    uint32_t reserved_1bc;
    uint32_t reserved_1c0;
    uint32_t reserved_1c4;
    uint32_t reserved_1c8;
    uint32_t reserved_1cc;
    uint32_t reserved_1d0;
    uint32_t reserved_1d4;
    uint32_t reserved_1d8;
    uint32_t reserved_1dc;
    uint32_t reserved_1e0;
    uint32_t reserved_1e4;
    uint32_t reserved_1e8;
    uint32_t reserved_1ec;
    uint32_t reserved_1f0;
    uint32_t reserved_1f4;
    uint32_t reserved_1f8;
    uint32_t reserved_1fc;
    uint32_t reserved_200;
    uint32_t reserved_204;
    uint32_t reserved_208;
    uint32_t reserved_20c;
    uint32_t reserved_210;
    uint32_t reserved_214;
    uint32_t reserved_218;
    uint32_t reserved_21c;
    uint32_t reserved_220;
    uint32_t reserved_224;
    uint32_t reserved_228;
    uint32_t reserved_22c;
    uint32_t reserved_230;
    uint32_t reserved_234;
    uint32_t reserved_238;
    uint32_t reserved_23c;
    uint32_t reserved_240;
    uint32_t reserved_244;
    uint32_t reserved_248;
    uint32_t reserved_24c;
    uint32_t reserved_250;
    uint32_t reserved_254;
    uint32_t reserved_258;
    uint32_t reserved_25c;
    uint32_t reserved_260;
    uint32_t reserved_264;
    uint32_t reserved_268;
    uint32_t reserved_26c;
    uint32_t reserved_270;
    uint32_t reserved_274;
    uint32_t reserved_278;
    uint32_t reserved_27c;
    uint32_t reserved_280;
    uint32_t reserved_284;
    uint32_t reserved_288;
    uint32_t reserved_28c;
    uint32_t reserved_290;
    uint32_t reserved_294;
    uint32_t reserved_298;
    uint32_t reserved_29c;
    uint32_t reserved_2a0;
    uint32_t reserved_2a4;
    uint32_t reserved_2a8;
    uint32_t reserved_2ac;
    uint32_t reserved_2b0;
    uint32_t reserved_2b4;
    uint32_t reserved_2b8;
    uint32_t reserved_2bc;
    uint32_t reserved_2c0;
    uint32_t reserved_2c4;
    uint32_t reserved_2c8;
    uint32_t reserved_2cc;
    uint32_t reserved_2d0;
    uint32_t reserved_2d4;
    uint32_t reserved_2d8;
    uint32_t reserved_2dc;
    uint32_t reserved_2e0;
    uint32_t reserved_2e4;
    uint32_t reserved_2e8;
    uint32_t reserved_2ec;
    uint32_t reserved_2f0;
    uint32_t reserved_2f4;
    uint32_t reserved_2f8;
    uint32_t reserved_2fc;
    uint32_t reserved_300;
    uint32_t reserved_304;
    uint32_t reserved_308;
    uint32_t reserved_30c;
    uint32_t reserved_310;
    uint32_t reserved_314;
    uint32_t reserved_318;
    uint32_t reserved_31c;
    uint32_t reserved_320;
    uint32_t reserved_324;
    uint32_t reserved_328;
    uint32_t reserved_32c;
    uint32_t reserved_330;
    uint32_t reserved_334;
    uint32_t reserved_338;
    uint32_t reserved_33c;
    uint32_t reserved_340;
    uint32_t reserved_344;
    uint32_t reserved_348;
    uint32_t reserved_34c;
    uint32_t reserved_350;
    uint32_t reserved_354;
    uint32_t reserved_358;
    uint32_t reserved_35c;
    uint32_t reserved_360;
    uint32_t reserved_364;
    uint32_t reserved_368;
    uint32_t reserved_36c;
    uint32_t reserved_370;
    uint32_t reserved_374;
    uint32_t reserved_378;
    uint32_t reserved_37c;
    uint32_t reserved_380;
    uint32_t reserved_384;
    uint32_t reserved_388;
    uint32_t reserved_38c;
    uint32_t reserved_390;
    uint32_t reserved_394;
    uint32_t reserved_398;
    uint32_t reserved_39c;
    uint32_t reserved_3a0;
    uint32_t reserved_3a4;
    uint32_t reserved_3a8;
    uint32_t reserved_3ac;
    uint32_t reserved_3b0;
    uint32_t reserved_3b4;
    uint32_t reserved_3b8;
    uint32_t reserved_3bc;
    uint32_t reserved_3c0;
    uint32_t reserved_3c4;
    uint32_t reserved_3c8;
    uint32_t reserved_3cc;
    uint32_t reserved_3d0;
    uint32_t reserved_3d4;
    uint32_t reserved_3d8;
    uint32_t reserved_3dc;
    uint32_t reserved_3e0;
    uint32_t reserved_3e4;
    uint32_t reserved_3e8;
    uint32_t reserved_3ec;
    uint32_t reserved_3f0;
    uint32_t reserved_3f4;
    uint32_t reserved_3f8;
    union {
        struct {
            uint32_t extmem_reg_date               :    28;  /*version information.*/
            uint32_t reserved28                    :    4;
        };
        uint32_t val;
    } date;
} extmem_dev_t;
extern extmem_dev_t EXTMEM;
#ifdef __cplusplus
}
#endif



#endif /*_SOC_EXTMEM_STRUCT_H_ */
