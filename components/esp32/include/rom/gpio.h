// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ROM_GPIO_H_
#define _ROM_GPIO_H_

#include <stdint.h>
#include <stdbool.h>

#include "esp_attr.h"
#include "soc/gpio_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_REG_READ(reg)              READ_PERI_REG(reg)
#define GPIO_REG_WRITE(reg, val)        WRITE_PERI_REG(reg, val)
#define GPIO_PIN_COUNT                  40
#define GPIO_ID_PIN0                    0
#define GPIO_ID_PIN(n)                  (GPIO_ID_PIN0+(n))
#define GPIO_PIN_ADDR(i)                (GPIO_PIN0 + i*4)

#define GPIO_ID_IS_PIN_REGISTER(reg_id) \
    ((reg_id >= GPIO_ID_PIN0) && (reg_id <= GPIO_ID_PIN(GPIO_PIN_COUNT-1)))

#define GPIO_REGID_TO_PINIDX(reg_id) ((reg_id) - GPIO_ID_PIN0)

typedef enum{
      GPIO_PIN_INTR_DISABLE = 0,
      GPIO_PIN_INTR_POSEDGE = 1,
      GPIO_PIN_INTR_NEGEDGE = 2,
      GPIO_PIN_INTR_ANYEGDE = 3,
      GPIO_PIN_INTR_LOLEVEL = 4,
      GPIO_PIN_INTR_HILEVEL = 5
}GPIO_INT_TYPE;

#define GREEN_LED_ON()  GPIO_OUTPUT_SET(GPIO_ID_PIN(1) , 0)
#define GREEN_LED_OFF() GPIO_OUTPUT_SET(GPIO_ID_PIN(1) , 1)

#define GPIO_OUTPUT_SET(gpio_no, bit_value) \
        ((gpio_no < 32) ? gpio_output_set(bit_value<<gpio_no, (bit_value ? 0 : 1)<<gpio_no, 1<<gpio_no,0) : \
                         gpio_output_set_high(bit_value<<(gpio_no - 32), (bit_value ? 0 : 1)<<(gpio_no - 32), 1<<(gpio_no -32),0))
#define GPIO_DIS_OUTPUT(gpio_no)    ((gpio_no < 32) ? gpio_output_set(0,0,0, 1<<gpio_no) : gpio_output_set_high(0,0,0, 1<<gpio_no))
#define GPIO_INPUT_GET(gpio_no)     ((gpio_no < 32)? ((gpio_input_get()>>gpio_no)&BIT0) : ((gpio_input_get_high()>>(gpio_no - 32))&BIT0))

/* GPIO interrupt handler, registered through gpio_intr_handler_register */
typedef void (* gpio_intr_handler_fn_t)(uint32_t intr_mask, bool high, void *arg);


/*
 * Initialize GPIO.  This includes reading the GPIO Configuration DataSet
 * to initialize "output enables" and pin configurations for each gpio pin.
 * Must be called once during startup.
 */
void gpio_init(void) ROMFN_ATTR;

/*
 * Change GPIO pin output by setting, clearing, or disabling pins.
 * In general, it is expected that a bit will be set in at most one
 * of these masks.  If a bit is clear in all masks, the output state
 * remains unchanged.
 *
 * There is no particular ordering guaranteed; so if the order of
 * writes is significant, calling code should divide a single call
 * into multiple calls.
 */
void gpio_output_set(uint32_t set_mask,
        uint32_t clear_mask,
        uint32_t enable_mask,
        uint32_t disable_mask) ROMFN_ATTR;
void gpio_output_set_high(uint32_t set_mask,
        uint32_t clear_mask,
        uint32_t enable_mask,
        uint32_t disable_mask) ROMFN_ATTR;
/*
 * Sample the value of GPIO input pins and returns a bitmask.
 */
uint32_t gpio_input_get(void) ROMFN_ATTR;
uint32_t gpio_input_get_high(void) ROMFN_ATTR;

/*
 * Set the specified GPIO register to the specified value.
 * This is a very general and powerful interface that is not
 * expected to be used during normal operation.  It is intended
 * mainly for debug, or for unusual requirements.
 */
void gpio_register_set(uint32_t reg_id, uint32_t value) ROMFN_ATTR;

/* Get the current value of the specified GPIO register. */
uint32_t gpio_register_get(uint32_t reg_id) ROMFN_ATTR;

/*
 * Register an application-specific interrupt handler for GPIO pin
 * interrupts.  Once the interrupt handler is called, it will not 
 * be called again until after a call to gpio_intr_ack.  Any GPIO
 * interrupts that occur during the interim are masked.
 *
 * The application-specific handler is called with a mask of
 * pending GPIO interrupts.  After processing pin interrupts, the
 * application-specific handler may wish to use gpio_intr_pending
 * to check for any additional pending interrupts before it returns.
 */
void gpio_intr_handler_register(gpio_intr_handler_fn_t fn, void *arg) ROMFN_ATTR;

/* Determine which GPIO interrupts are pending. */
uint32_t gpio_intr_pending(void) ROMFN_ATTR;
uint32_t gpio_intr_pending_high(void) ROMFN_ATTR;

/*
 * Acknowledge GPIO interrupts.
 * Intended to be called from the gpio_intr_handler_fn.
 */
void gpio_intr_ack(uint32_t ack_mask) ROMFN_ATTR;
void gpio_intr_ack_high(uint32_t ack_mask) ROMFN_ATTR;

void gpio_pin_wakeup_enable(uint32_t i, GPIO_INT_TYPE intr_state) ROMFN_ATTR;

void gpio_pin_wakeup_disable() ROMFN_ATTR;

//extern void gpio_module_install(struct gpio_api *api);

void gpio_matrix_in(uint32_t gpio, uint32_t signal_idx, bool inv) ROMFN_ATTR;

void gpio_matrix_out(uint32_t gpio, uint32_t signal_idx, bool out_inv, bool oen_inv) ROMFN_ATTR;

#ifdef __cplusplus
}
#endif

#endif /* _ROM_GPIO_H_ */
