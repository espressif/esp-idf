/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "usbh_core.h"
#include "usbh_hid.h"

static char *TAG = "HID";

typedef struct {
    bool is_active;
    esp_timer_handle_t timer;
    uint8_t *buffer;
} hid_int_in_t;

/**
 * @brief Key event
 */
typedef struct {
    enum key_state {
        KEY_STATE_PRESSED = 0x00,
        KEY_STATE_RELEASED = 0x01
    } state;
    uint8_t modifier;
    uint8_t key_code;
} key_event_t;

/**
 * @brief hid msg
 */
typedef struct {
    uint8_t protocol;
    uint16_t len;
    uint8_t data[64];
} hid_msg_t;

#define QUEUE_LEN 10
static QueueHandle_t s_msg_queue = NULL;
static TaskHandle_t s_msg_task_handle = NULL;

/* Main char symbol for ENTER key */
#define KEYBOARD_ENTER_MAIN_CHAR    '\r'
/* When set to 1 pressing ENTER will be extending with LineFeed during serial debug output */
#define KEYBOARD_ENTER_LF_EXTEND    1
/* When set to 1, numbers entered from the numeric keypad while ALT is pressed will be escaped */
#define KEYBOARD_ENTER_ALT_ESCAPE   1

#if KEYBOARD_ENTER_ALT_ESCAPE
static bool is_ansi = false;
static unsigned int alt_code = 0;
#endif

/**
 * @brief Scancode to ascii table
 */
const uint8_t keycode2ascii [57][2] = {
    {0, 0}, /* HID_KEY_NO_PRESS        */
    {0, 0}, /* HID_KEY_ROLLOVER        */
    {0, 0}, /* HID_KEY_POST_FAIL       */
    {0, 0}, /* HID_KBD_USAGE_ERRUNDEF */
    {'a', 'A'}, /* HID_KEY_A               */
    {'b', 'B'}, /* HID_KEY_B               */
    {'c', 'C'}, /* HID_KEY_C               */
    {'d', 'D'}, /* HID_KEY_D               */
    {'e', 'E'}, /* HID_KEY_E               */
    {'f', 'F'}, /* HID_KEY_F               */
    {'g', 'G'}, /* HID_KEY_G               */
    {'h', 'H'}, /* HID_KEY_H               */
    {'i', 'I'}, /* HID_KEY_I               */
    {'j', 'J'}, /* HID_KEY_J               */
    {'k', 'K'}, /* HID_KEY_K               */
    {'l', 'L'}, /* HID_KEY_L               */
    {'m', 'M'}, /* HID_KEY_M               */
    {'n', 'N'}, /* HID_KEY_N               */
    {'o', 'O'}, /* HID_KEY_O               */
    {'p', 'P'}, /* HID_KEY_P               */
    {'q', 'Q'}, /* HID_KEY_Q               */
    {'r', 'R'}, /* HID_KEY_R               */
    {'s', 'S'}, /* HID_KEY_S               */
    {'t', 'T'}, /* HID_KEY_T               */
    {'u', 'U'}, /* HID_KEY_U               */
    {'v', 'V'}, /* HID_KEY_V               */
    {'w', 'W'}, /* HID_KEY_W               */
    {'x', 'X'}, /* HID_KEY_X               */
    {'y', 'Y'}, /* HID_KEY_Y               */
    {'z', 'Z'}, /* HID_KEY_Z               */
    {'1', '!'}, /* HID_KEY_1               */
    {'2', '@'}, /* HID_KEY_2               */
    {'3', '#'}, /* HID_KEY_3               */
    {'4', '$'}, /* HID_KEY_4               */
    {'5', '%'}, /* HID_KEY_5               */
    {'6', '^'}, /* HID_KEY_6               */
    {'7', '&'}, /* HID_KEY_7               */
    {'8', '*'}, /* HID_KEY_8               */
    {'9', '('}, /* HID_KEY_9               */
    {'0', ')'}, /* HID_KEY_0               */
    {KEYBOARD_ENTER_MAIN_CHAR, KEYBOARD_ENTER_MAIN_CHAR}, /* HID_KEY_ENTER           */
    {0, 0}, /* HID_KEY_ESC             */
    {'\b', 0}, /* HID_KEY_DEL             */
    {0, 0}, /* HID_KEY_TAB             */
    {' ', ' '}, /* HID_KEY_SPACE           */
    {'-', '_'}, /* HID_KEY_MINUS           */
    {'=', '+'}, /* HID_KEY_EQUAL           */
    {'[', '{'}, /* HID_KEY_OPEN_BRACKET    */
    {']', '}'}, /* HID_KEY_CLOSE_BRACKET   */
    {'\\', '|'}, /* HID_KEY_BACK_SLASH      */
    {'\\', '|'}, /* HID_KEY_SHARP           */  // HOTFIX: for NonUS Keyboards repeat HID_KEY_BACK_SLASH
    {';', ':'}, /* HID_KEY_COLON           */
    {'\'', '"'}, /* HID_KEY_QUOTE           */
    {'`', '~'}, /* HID_KEY_TILDE           */
    {',', '<'}, /* HID_KEY_LESS            */
    {'.', '>'}, /* HID_KEY_GREATER         */
    {'/', '?'} /* HID_KBD_USAGE_QUESTION           */
};

void ld_include_hid(void)
{
}

/**
 * @brief HID Keyboard print char symbol
 *
 * @param[in] key_char  Keyboard char to stdout
 */
static inline void hid_keyboard_print_char(unsigned int key_char)
{
    if (!!key_char) {
        putchar(key_char);
#if (KEYBOARD_ENTER_LF_EXTEND)
        if (KEYBOARD_ENTER_MAIN_CHAR == key_char) {
            putchar('\n');
        }
#endif // KEYBOARD_ENTER_LF_EXTEND
        fflush(stdout);
    }
}

/**
 * @brief Makes new line depending on report output protocol type
 *
 * @param[in] proto Current protocol to output
 */
static void hid_print_new_device_report_header(int proto)
{
    static int prev_proto_output = -1;

    if (prev_proto_output != proto) {
        prev_proto_output = proto;
        printf("\r\n");
        if (proto == HID_PROTOCOL_MOUSE) {
            printf("Mouse\r\n");
        } else if (proto == HID_PROTOCOL_KEYBOARD) {
            printf("Keyboard\r\n");
        } else {
            printf("Generic\r\n");
        }
        fflush(stdout);
    }
}

/**
 * @brief HID Keyboard modifier verification for capitalization application (right or left shift)
 *
 * @param[in] modifier
 * @return true  Modifier was pressed (left or right shift)
 * @return false Modifier was not pressed (left or right shift)
 *
 */
static inline bool hid_keyboard_is_modifier_shift(uint8_t modifier)
{
    if (((modifier & HID_MODIFIER_LSHIFT) == HID_MODIFIER_LSHIFT) ||
            ((modifier & HID_MODIFIER_RSHIFT) == HID_MODIFIER_RSHIFT)) {
        return true;
    }
    return false;
}

#if KEYBOARD_ENTER_ALT_ESCAPE
/**
 * @brief HID Keyboard modifier verification for capitalization application (right or left alt)
 *
 * @param[in] modifier
 * @return true  Modifier was pressed (left or right alt)
 * @return false Modifier was not pressed (left or right alt)
 *
 */
static inline bool hid_keyboard_is_modifier_alt(uint8_t modifier)
{
    if (((modifier & HID_MODIFIER_LALT) == HID_MODIFIER_LALT) ||
            ((modifier & HID_MODIFIER_RALT) == HID_MODIFIER_RALT)) {
        return true;
    }
    return false;
}

/**
 * @brief HID Keyboard alt code process(Called when ALT is pressed)
 *
 * @param[in] key_code Entered key value
 * @return true  Key values that qualify for ALT escape processing
 * @return false Key values that do not comply with ALT escape processing
 *
 */
static inline bool hid_keyboard_alt_code_processing(uint8_t key_code)
{
    if ((key_code < HID_KBD_USAGE_KPD1) || (key_code > HID_KBD_USAGE_KPD0)) {
        return false;
    }
    if (key_code == HID_KBD_USAGE_KPD0) {
        if (alt_code == 0) {
            is_ansi = true;
            return true;
        }
        /* Note: Since the keyboard code 0 of the numeric keypad is not keyboard code 1 minus 1, the
         * conversion is performed here to facilitate subsequent calculations of the input numbers.
        */
        key_code = HID_KBD_USAGE_KPD1 - 1;
    }
    alt_code = alt_code * 10 + (key_code - (HID_KBD_USAGE_KPD1 - 1));
    return true;
}

/**
 * @brief HID Keyboard alt code process complete(Called when ALT is not pressed)
 */
static inline void hid_keyboard_alt_code_process_complete(void)
{
    if (alt_code > 0) {
        alt_code = alt_code & 0xff;
        if (is_ansi || alt_code == 0) {
            char utf8_buffer[8] = { 0 };
            if (alt_code == 0) {
                alt_code = 0x100;
            }
            //ANSI is processed as UTF8
            if (alt_code <= 0x7F) {
                utf8_buffer[0] = (char)alt_code;
            } else {
                utf8_buffer[0] = 0xC0 | ((alt_code >> 6) & 0x1F);
                utf8_buffer[1] = 0x80 | (alt_code & 0x3F);
            }
            printf("%s", utf8_buffer);
            fflush(stdout);
        } else {
            hid_keyboard_print_char(alt_code);
        }
        alt_code = 0;
    }
    is_ansi = false;
}
#endif

/**
 * @brief HID Keyboard get char symbol from key code
 *
 * @param[in] modifier  Keyboard modifier data
 * @param[in] key_code  Keyboard key code
 * @param[in] key_char  Pointer to key char data
 *
 * @return true  Key scancode converted successfully
 * @return false Key scancode unknown
 */
static inline bool hid_keyboard_get_char(uint8_t modifier,
                                         uint8_t key_code,
                                         unsigned char *key_char)
{
    uint8_t mod = (hid_keyboard_is_modifier_shift(modifier)) ? 1 : 0;

#if KEYBOARD_ENTER_ALT_ESCAPE
    if (hid_keyboard_is_modifier_alt(modifier)) {
        if (hid_keyboard_alt_code_processing(key_code)) {
            return false;
        }
    }
#endif

    if ((key_code >= HID_KBD_USAGE_A) && (key_code <= HID_KBD_USAGE_QUESTION)) {
        *key_char = keycode2ascii[key_code][mod];
    } else {
        // All other key pressed
        return false;
    }

    return true;
}

/**
 * @brief Key Event. Key event with the key code, state and modifier.
 *
 * @param[in] key_event Pointer to Key Event structure
 *
 */
static void key_event_callback(key_event_t *key_event)
{
    unsigned char key_char;

    hid_print_new_device_report_header(HID_PROTOCOL_KEYBOARD);

    if (KEY_STATE_PRESSED == key_event->state) {
        if (hid_keyboard_get_char(key_event->modifier,
                                  key_event->key_code, &key_char)) {

            hid_keyboard_print_char(key_char);

        }
    }
}

/**
 * @brief Key buffer scan code search.
 *
 * @param[in] src       Pointer to source buffer where to search
 * @param[in] key       Key scancode to search
 * @param[in] length    Size of the source buffer
 */
static inline bool key_found(const uint8_t *const src,
                             uint8_t key,
                             unsigned int length)
{
    for (unsigned int i = 0; i < length; i++) {
        if (src[i] == key) {
            return true;
        }
    }
    return false;
}

static void usbh_hid_keyboard_report_callback(void *arg, int nbytes)
{
    struct usb_hid_kbd_report *kb_report = (struct usb_hid_kbd_report *)arg;
    if (nbytes < sizeof(struct usb_hid_kbd_report)) {
        return;
    }
    static uint8_t prev_keys[sizeof(kb_report->key)] = { 0 };
    key_event_t key_event;

#if KEYBOARD_ENTER_ALT_ESCAPE
    if (!hid_keyboard_is_modifier_alt(kb_report->modifier)) {
        hid_keyboard_alt_code_process_complete();
    }
#endif

    for (int i = 0; i < sizeof(kb_report->key); i++) {

        // key has been released verification
        if (prev_keys[i] > HID_KBD_USAGE_ERRUNDEF &&
                !key_found(kb_report->key, prev_keys[i], sizeof(kb_report->key))) {
            key_event.key_code = prev_keys[i];
            key_event.modifier = 0;
            key_event.state = KEY_STATE_RELEASED;
            key_event_callback(&key_event);
        }

        // key has been pressed verification
        if (kb_report->key[i] > HID_KBD_USAGE_ERRUNDEF &&
                !key_found(prev_keys, kb_report->key[i], sizeof(kb_report->key))) {
            key_event.key_code = kb_report->key[i];
            key_event.modifier = kb_report->modifier;
            key_event.state = KEY_STATE_PRESSED;
            key_event_callback(&key_event);
        }
    }

    memcpy(prev_keys, &kb_report->key, sizeof(kb_report->key));
}

static void usbh_hid_mouse_report_callback(void *arg, int nbytes)
{
    struct usb_hid_mouse_report *mouse_report = (struct usb_hid_mouse_report *)arg;

    // At least 3 bytes are reported, followed by optional data
    if (nbytes < 3) {
        return;
    }
    static int x_pos = 0;
    static int y_pos = 0;

    // Calculate absolute position from displacement
    x_pos += (int8_t)mouse_report->xdisp;
    y_pos += (int8_t)mouse_report->ydisp;

    hid_print_new_device_report_header(HID_PROTOCOL_MOUSE);

    printf("X: %06d\tY: %06d\t|%c|%c|\n",
           x_pos, y_pos,
           ((mouse_report->buttons & HID_MOUSE_INPUT_BUTTON_LEFT) ? 'o' : ' '),
           ((mouse_report->buttons & HID_MOUSE_INPUT_BUTTON_RIGHT) ? 'o' : ' '));
    fflush(stdout);
}

static void usbh_hid_generic_report_callback(void *arg, int nbytes)
{
    char *data = arg;
    hid_print_new_device_report_header(HID_PROTOCOL_NONE);
    for (int i = 0; i < nbytes; i++) {
        printf("%02X", data[i]);
    }
    putchar('\r');
}

//Note: This callback is in the interrupt context
static void usbh_hid_callback(void *arg, int nbytes)
{
    BaseType_t xTaskWoken = pdFALSE;
    struct usbh_hid *hid_class = (struct usbh_hid *)arg;
    hid_int_in_t *hid_intin = (hid_int_in_t *)hid_class->user_data;

    if (nbytes <= 0) {
        hid_intin->is_active = false;
        return;
    }
    uint8_t sub_class = hid_class->hport->config.intf[hid_class->intf].altsetting[0].intf_desc.bInterfaceSubClass;
    uint8_t protocol = hid_class->hport->config.intf[hid_class->intf].altsetting[0].intf_desc.bInterfaceProtocol;

    if (s_msg_queue) {
        hid_msg_t msg;
        if (nbytes <= sizeof(msg.data)) {
            msg.protocol = HID_PROTOCOL_NONE;
            if (sub_class == HID_SUBCLASS_BOOTIF) {
                if (protocol == HID_PROTOCOL_KEYBOARD) {
                    msg.protocol = HID_PROTOCOL_KEYBOARD;
                } else if (protocol == HID_PROTOCOL_MOUSE) {
                    msg.protocol = HID_PROTOCOL_MOUSE;
                }
            }
            msg.len = nbytes;
            memcpy(msg.data, hid_intin->buffer, nbytes);
            if (xQueueSendFromISR(s_msg_queue, &msg, &xTaskWoken) != pdTRUE) {
                ESP_EARLY_LOGD(TAG, "msg queue full");
            }
        } else {
            ESP_EARLY_LOGD(TAG, "nbytes(%d) > sizeof(msg.data)", nbytes);
        }

    }
    hid_intin->is_active = false;
    if (xTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

//Note: If the dispatch_method of esp_timer is ESP_TIMER_ISR, the callback is in the interrupt context.
static void intin_timer_cb(void *arg)
{
    int ret;
    struct usbh_hid *hid_class = (struct usbh_hid *)arg;
    hid_int_in_t *hid_intin = (hid_int_in_t *)hid_class->user_data;
    if (hid_intin->is_active) {
        return;
    }
    usbh_int_urb_fill(&hid_class->intin_urb, hid_class->hport, hid_class->intin, hid_intin->buffer, hid_class->intin->wMaxPacketSize, 0,
                      usbh_hid_callback, hid_class);

    hid_intin->is_active = true;
    ret = usbh_submit_urb(&hid_class->intin_urb);
    if (ret != 0) {
        if (ret == -USB_ERR_NOTCONN) {
            esp_timer_stop(hid_intin->timer);
            return;
        }
        hid_intin->is_active = false;
        ESP_EARLY_LOGE(TAG, "usbh_submit_urb failed");
    }
}

static void usbh_hid_msg_task(void *arg)
{
    hid_msg_t msg;
    while (1) {
        BaseType_t err = xQueueReceive(s_msg_queue, &msg, portMAX_DELAY);
        if (err != pdTRUE) {
            continue;
        }
        if (msg.protocol == HID_PROTOCOL_KEYBOARD) {
            usbh_hid_keyboard_report_callback(msg.data, msg.len);
        } else if (msg.protocol == HID_PROTOCOL_MOUSE) {
            usbh_hid_mouse_report_callback(msg.data, msg.len);
        } else {
            usbh_hid_generic_report_callback(msg.data, msg.len);
        }
    }
    vTaskDelete(NULL);
}

static void creat_msg_task(void)
{
    if (s_msg_queue == NULL) {
        s_msg_queue = xQueueCreate(QUEUE_LEN, sizeof(hid_msg_t));
        if (s_msg_queue == NULL) {
            ESP_LOGE(TAG, "ringbuf create failed");
            return;
        }
    }
    if (s_msg_task_handle == NULL) {
        xTaskCreate(usbh_hid_msg_task, "usbh_hid_msg_task", 4096, NULL, 5, &s_msg_task_handle);
    }
}

void usbh_hid_run(struct usbh_hid *hid_class)
{
    int ret;
    esp_err_t err;
    uint8_t sub_class = hid_class->hport->config.intf[hid_class->intf].altsetting[0].intf_desc.bInterfaceSubClass;
    uint8_t protocol = hid_class->hport->config.intf[hid_class->intf].altsetting[0].intf_desc.bInterfaceProtocol;
    ESP_LOGI(TAG, "intf %u, SubClass %u, Protocol %u", hid_class->intf, sub_class, protocol);

    if (sub_class == HID_SUBCLASS_BOOTIF) {
        ret = usbh_hid_set_protocol(hid_class, HID_PROTOCOL_BOOT);
        if (ret < 0) {
            return;
        }
    }

    creat_msg_task();

    if (hid_class->intin == NULL) {
        ESP_LOGW(TAG, "no intin ep desc");
        return;
    }

    hid_int_in_t *hid_intin = heap_caps_calloc(1, sizeof(hid_int_in_t), MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    if (hid_intin == NULL) {
        ESP_LOGW(TAG, "Malloc failed");
        return;
    }
    hid_intin->buffer = heap_caps_aligned_alloc(CONFIG_USB_ALIGN_SIZE, hid_class->intin->wMaxPacketSize, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (hid_intin->buffer == NULL) {
        ESP_LOGW(TAG, "Malloc failed");
        goto error;
    }
    hid_intin->is_active = false;
    esp_timer_create_args_t timer_cfg = {
        .callback = intin_timer_cb,
        .arg = hid_class,
#if CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD
        .dispatch_method = ESP_TIMER_ISR,
#else
        .dispatch_method = ESP_TIMER_TASK,
#endif
        .name = "intin timer",
        .skip_unhandled_events = true,
    };
    err = esp_timer_create(&timer_cfg, &hid_intin->timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "timer create failed");
        goto error;
    }

    hid_class->user_data = hid_intin;

    esp_timer_start_periodic(hid_intin->timer, USBH_GET_URB_INTERVAL(hid_class->intin->bInterval, hid_class->hport->speed));

    return;
error:
    if (hid_intin->buffer) {
        heap_caps_free(hid_intin->buffer);
    }
    heap_caps_free(hid_intin);
}

void usbh_hid_stop(struct usbh_hid *hid_class)
{
    hid_int_in_t *hid_intin = (hid_int_in_t *)hid_class->user_data;
    if (hid_intin) {
        esp_timer_stop(hid_intin->timer);
        esp_timer_delete(hid_intin->timer);
        heap_caps_free(hid_intin->buffer);
        heap_caps_free(hid_intin);
    }
    ESP_LOGW(TAG, "hid stop");
}
