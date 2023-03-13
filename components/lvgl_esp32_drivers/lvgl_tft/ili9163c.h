/**
 * @file lv_templ.h
 *
 */

#ifndef ILI9163C_H
#define ILI9163C_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#include "../lvgl_helpers.h"

/*********************
 *      DEFINES
 *********************/
#define ILI9163C_DC            CONFIG_LV_DISP_PIN_DC
#define ILI9163C_RST           CONFIG_LV_DISP_PIN_RST
#define ILI9163C_INVERT_COLORS CONFIG_LV_INVERT_COLORS

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void ili9163c_init(void);
void ili9163c_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
void ili9163c_sleep_in(void);
void ili9163c_sleep_out(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*ILI9163C_H*/
