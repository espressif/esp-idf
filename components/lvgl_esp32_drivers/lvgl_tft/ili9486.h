/**
 * @file ili9486.h
 *
 */

#ifndef ILI9486_H
#define ILI9486_H

#ifdef __cplusplus
extern "C" {
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
#define ILI9486_DC      CONFIG_LV_DISP_PIN_DC
#define ILI9486_RST     CONFIG_LV_DISP_PIN_RST
#define ILI9486_USE_RST CONFIG_LV_DISP_USE_RST


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void ili9486_init(void);
void ili9486_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ILI9486_H*/
