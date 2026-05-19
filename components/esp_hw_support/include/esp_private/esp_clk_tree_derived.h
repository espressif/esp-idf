/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Mutable runtime state for a derived PLL clock
 *
 * Owned and statically allocated by the per-target file that registers the
 * derived clock; the engine in `esp_clk_tree_common.c` only reads/writes
 * through `esp_clk_tree_derived_clk_desc_t::state`.
 */
typedef struct {
    int              ref_cnt;          /*!< Number of acquirers; 0 means gated off. */
    soc_module_clk_t cur_upstream;     /*!< Currently programmed upstream clk (`SOC_MOD_CLK_INVALID` until first config). */
    uint32_t         cur_divider;      /*!< Currently programmed divider (0 until first config). */
} esp_clk_tree_derived_clk_state_t;

/**
 * @brief Single candidate upstream PLL for a derived clock
 *
 * Lists one allowed upstream and the mux selector value the engine must pass
 * to `set_src` to route from that upstream. For derived clocks with no
 * upstream mux (e.g. ESP32-P4 PLL_F50M is fixed to MPLL), `mux_sel` is
 * unused and may be 0.
 */
typedef struct {
    soc_module_clk_t upstream;   /*!< Allowed upstream module clock (CPLL, MPLL, ...) */
    uint8_t          mux_sel;    /*!< Mux selector value passed to `set_src` to route from `upstream` */
} esp_clk_tree_derived_upstream_t;

/**
 * @brief Descriptor for a derived PLL clock (e.g. PLL_F50M, PLL_F25M, PLL_F20M)
 *
 * Each target that wants a derived clock to participate in the shared
 * acquire/release/freq_set engine in `esp_clk_tree_common.c` provides a
 * descriptor and overrides the weak `esp_clk_tree_get_derived_clk_desc()`.
 *
 * Upstream selection is pure data: the engine iterates `upstreams[]` in
 * order, querying each candidate's currently programmed frequency, and
 * picks the first one that divides cleanly to the requested target
 * frequency. Iteration order encodes per-target preference.
 *
 * @warning `set_src` / `set_divider` / `set_gate` are invoked from a critical section
 *       and must be short and must not call functions that take other locks or block.
 *       It means you must use `RCC_ATOMIC` lock free form of HAL functions (e.g.`_clk_gate_ll_*`)!
 */
typedef struct {
    soc_module_clk_t clk_id;                              /*!< Module clock id this descriptor belongs to */
    void (*set_src)(uint8_t mux_sel);                     /*!< Optional: program the upstream PLL mux. NULL when the clock has no mux (e.g. ESP32-P4 PLL_F50M is fixed to MPLL). */
    void (*set_divider)(uint32_t divider);                /*!< Required: program the divider register (divider value, not the raw "div_num - 1" form). */
    void (*set_gate)(bool enable);                        /*!< Required: enable/disable the clock gate. */
    const esp_clk_tree_derived_upstream_t *upstreams;     /*!< Required: candidate upstream PLLs in preference order (used by both auto-pick and explicit-upstream paths). */
    size_t upstream_count;                                /*!< Required: number of entries in `upstreams[]`. */
    esp_clk_tree_derived_clk_state_t *state;              /*!< Required: pointer to a statically-allocated mutable state slot owned by the target. Engine reads/writes ref_cnt / cur_upstream / cur_divider through this pointer. */
} esp_clk_tree_derived_clk_desc_t;

/**
 * @brief Look up the derived-clock descriptor for a given module clock id
 *
 * Default implementation in `esp_clk_tree_common.c` returns NULL for every
 * clock. Each target that supports derived-clock locking overrides this
 * function in its `port/<target>/esp_clk_tree.c`.
 *
 * @param clk_src Module clock id
 * @return Pointer to a static descriptor, or NULL if the clock is not a
 *         lockable derived clock on this target.
 */
const esp_clk_tree_derived_clk_desc_t *esp_clk_tree_get_derived_clk_desc(soc_module_clk_t clk_src);

/**
 * @brief Acquire a derived clock (refcount-based)
 *
 * Increments the descriptor's reference counter and enables the gate on the
 * first acquire. Public callers should use `esp_clk_tree_enable_src(clk, true)`
 * which dispatches here for any clock with a registered descriptor.
 */
esp_err_t esp_clk_tree_derived_clk_acquire(soc_module_clk_t clk_src);

/**
 * @brief Release a derived clock previously acquired
 *
 * Decrements the descriptor's reference counter and disables the gate on the
 * last release. Public callers should use `esp_clk_tree_enable_src(clk, false)`.
 */
esp_err_t esp_clk_tree_derived_clk_release(soc_module_clk_t clk_src);

/**
 * @brief Configure the divider of a derived clock for a target frequency
 *
 * Uses the upstream previously chosen via `esp_clk_tree_src_select_upstream()`
 * (`state->cur_upstream`) when set; otherwise iterates the descriptor's
 * `upstreams[]` and auto-picks the first whose currently programmed
 * frequency divides cleanly to `expt_freq_hz`. Programs both the upstream
 * mux (if not already programmed) and the divider atomically. Public
 * callers should use `esp_clk_tree_src_set_freq_hz()` which dispatches here.
 *
 * Share-lock semantics: when ref_cnt > 1 the request must match the
 * currently applied (upstream, divider) pair, otherwise returns
 * `ESP_ERR_INVALID_STATE` and reports the active frequency via
 * `real_freq_hz`.
 */
esp_err_t esp_clk_tree_derived_clk_freq_set(soc_module_clk_t clk_src,
                                            uint32_t expt_freq_hz,
                                            uint32_t *real_freq_hz);

#ifdef __cplusplus
}
#endif
