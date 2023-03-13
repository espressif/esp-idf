/**
 * @file lv_refr.h
 *
 */

#ifndef LV_REFR_H
#define LV_REFR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/

#define LV_REFR_TASK_PRIO LV_TASK_PRIO_MID

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the screen refresh subsystem
 */
void _lv_refr_init(void);

/**
 * Redraw the invalidated areas now.
 * Normally the redrawing is periodically executed in `lv_timer_handler` but a long blocking process
 * can prevent the call of `lv_timer_handler`. In this case if the GUI is updated in the process
 * (e.g. progress bar) this function can be called when the screen should be updated.
 * @param disp pointer to display to refresh. NULL to refresh all displays.
 */
void lv_refr_now(lv_disp_t * disp);

/**
 * Redrawn on object an all its children using the passed draw context
 * @param draw  pointer to an initialized draw context
 * @param obj   the start object from the redraw should start
 */
void lv_obj_redraw(lv_draw_ctx_t * draw_ctx, lv_obj_t * obj);

/**
 * Invalidate an area on display to redraw it
 * @param area_p pointer to area which should be invalidated (NULL: delete the invalidated areas)
 * @param disp pointer to display where the area should be invalidated (NULL can be used if there is
 * only one display)
 */
void _lv_inv_area(lv_disp_t * disp, const lv_area_t * area_p);

/**
 * Get the display which is being refreshed
 * @return the display being refreshed
 */
lv_disp_t * _lv_refr_get_disp_refreshing(void);

/**
 * Set the display which is being refreshed.
 * It shouldn't be used directly by the user.
 * It can be used to trick the drawing functions about there is an active display.
 * @param the display being refreshed
 */
void _lv_refr_set_disp_refreshing(lv_disp_t * disp);

#if LV_USE_PERF_MONITOR
/**
 * Reset FPS counter
 */
void lv_refr_reset_fps_counter(void);

/**
 * Get the average FPS
 * @return the average FPS
 */
uint32_t lv_refr_get_fps_avg(void);
#endif

/**
 * Called periodically to handle the refreshing
 * @param timer pointer to the timer itself
 */
void _lv_disp_refr_timer(lv_timer_t * timer);

/**********************
 *   STATIC FUNCTIONS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_REFR_H*/
