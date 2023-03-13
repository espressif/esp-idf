/**
 * @file lv_txt.h
 *
 */

#ifndef LV_TXT_H
#define LV_TXT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include <stdbool.h>
#include <stdarg.h>
#include "lv_area.h"
#include "../font/lv_font.h"
#include "lv_printf.h"
#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_TXT_COLOR_CMD
#define LV_TXT_COLOR_CMD "#"
#endif

#define LV_TXT_ENC_UTF8 1
#define LV_TXT_ENC_ASCII 2

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Options for text rendering.
 */
enum {
    LV_TEXT_FLAG_NONE    = 0x00,
    LV_TEXT_FLAG_RECOLOR = 0x01, /**< Enable parsing of recolor command*/
    LV_TEXT_FLAG_EXPAND  = 0x02, /**< Ignore max-width to avoid automatic word wrapping*/
    LV_TEXT_FLAG_FIT     = 0x04, /**< Max-width is already equal to the longest line. (Used to skip some calculation)*/
};
typedef uint8_t lv_text_flag_t;

/**
 * State machine for text renderer.*/
enum {
    LV_TEXT_CMD_STATE_WAIT, /**< Waiting for command*/
    LV_TEXT_CMD_STATE_PAR,  /**< Processing the parameter*/
    LV_TEXT_CMD_STATE_IN,   /**< Processing the command*/
};
typedef uint8_t lv_text_cmd_state_t;

/** Label align policy*/
enum {
    LV_TEXT_ALIGN_AUTO, /**< Align text auto*/
    LV_TEXT_ALIGN_LEFT, /**< Align text to left*/
    LV_TEXT_ALIGN_CENTER, /**< Align text to center*/
    LV_TEXT_ALIGN_RIGHT, /**< Align text to right*/
};
typedef uint8_t lv_text_align_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Get size of a text
 * @param size_res pointer to a 'point_t' variable to store the result
 * @param text pointer to a text
 * @param font pointer to font of the text
 * @param letter_space letter space of the text
 * @param line_space line space of the text
 * @param flags settings for the text from ::lv_text_flag_t
 * @param max_width max width of the text (break the lines to fit this size). Set COORD_MAX to avoid
 * line breaks
 */
void lv_txt_get_size(lv_point_t * size_res, const char * text, const lv_font_t * font, lv_coord_t letter_space,
                     lv_coord_t line_space, lv_coord_t max_width, lv_text_flag_t flag);

/**
 * Get the next line of text. Check line length and break chars too.
 * @param txt a '\0' terminated string
 * @param font pointer to a font
 * @param letter_space letter space
 * @param max_width max width of the text (break the lines to fit this size). Set COORD_MAX to avoid
 * line breaks
 * @param used_width When used_width != NULL, save the width of this line if
 * flag == LV_TEXT_FLAG_NONE, otherwise save -1.
 * @param flags settings for the text from 'txt_flag_type' enum
 * @return the index of the first char of the new line (in byte index not letter index. With UTF-8
 * they are different)
 */
uint32_t _lv_txt_get_next_line(const char * txt, const lv_font_t * font, lv_coord_t letter_space,
                               lv_coord_t max_width, lv_coord_t * used_width, lv_text_flag_t flag);

/**
 * Give the length of a text with a given font
 * @param txt a '\0' terminate string
 * @param length length of 'txt' in byte count and not characters (Á is 1 character but 2 bytes in
 * UTF-8)
 * @param font pointer to a font
 * @param letter_space letter space
 * @param flags settings for the text from 'txt_flag_t' enum
 * @return length of a char_num long text
 */
lv_coord_t lv_txt_get_width(const char * txt, uint32_t length, const lv_font_t * font, lv_coord_t letter_space,
                            lv_text_flag_t flag);

/**
 * Check next character in a string and decide if the character is part of the command or not
 * @param state pointer to a txt_cmd_state_t variable which stores the current state of command
 * processing
 * @param c the current character
 * @return true: the character is part of a command and should not be written,
 *         false: the character should be written
 */
bool _lv_txt_is_cmd(lv_text_cmd_state_t * state, uint32_t c);

/**
 * Insert a string into an other
 * @param txt_buf the original text (must be big enough for the result text and NULL terminated)
 * @param pos position to insert (0: before the original text, 1: after the first char etc.)
 * @param ins_txt text to insert, must be '\0' terminated
 */
void _lv_txt_ins(char * txt_buf, uint32_t pos, const char * ins_txt);

/**
 * Delete a part of a string
 * @param txt string to modify, must be '\0' terminated and should point to a heap or stack frame, not read-only memory.
 * @param pos position where to start the deleting (0: before the first char, 1: after the first
 * char etc.)
 * @param len number of characters to delete
 */
void _lv_txt_cut(char * txt, uint32_t pos, uint32_t len);

/**
 * return a new formatted text. Memory will be allocated to store the text.
 * @param fmt `printf`-like format
 * @return pointer to the allocated text string.
 */
char * _lv_txt_set_text_vfmt(const char * fmt, va_list ap) LV_FORMAT_ATTRIBUTE(1, 0);

/**
 * Decode two encoded character from a string.
 * @param txt pointer to '\0' terminated string
 * @param letter the first decoded Unicode character or 0 on invalid data code
 * @param letter_next the second decoded Unicode character or 0 on invalid data code
 * @param ofs start index in 'txt' where to start.
 *                After the call it will point to the next encoded char in 'txt'.
 *                NULL to use txt[0] as index
 */
void _lv_txt_encoded_letter_next_2(const char * txt, uint32_t * letter, uint32_t * letter_next, uint32_t * ofs);

/**
 * Test if char is break char or not (a text can broken here or not)
 * @param letter a letter
 * @return false: 'letter' is not break char
 */
static inline bool _lv_txt_is_break_char(uint32_t letter)
{
    uint8_t i;
    bool ret = false;

    /* each chinese character can be break */
    if(letter >= 0x4E00 && letter <= 0x9FA5) {
        return true;
    }

    /*Compare the letter to TXT_BREAK_CHARS*/
    for(i = 0; LV_TXT_BREAK_CHARS[i] != '\0'; i++) {
        if(letter == (uint32_t)LV_TXT_BREAK_CHARS[i]) {
            ret = true; /*If match then it is break char*/
            break;
        }
    }

    return ret;
}

/***************************************************************
 *  GLOBAL FUNCTION POINTERS FOR CHARACTER ENCODING INTERFACE
 ***************************************************************/

/**
 * Give the size of an encoded character
 * @param str pointer to a character in a string
 * @return length of the encoded character (1,2,3 ...). O in invalid
 */
extern uint8_t (*_lv_txt_encoded_size)(const char *);

/**
 * Convert a Unicode letter to encoded
 * @param letter_uni a Unicode letter
 * @return Encoded character in Little Endian to be compatible with C chars (e.g. 'Á', 'Ü')
 */
extern uint32_t (*_lv_txt_unicode_to_encoded)(uint32_t);

/**
 * Convert a wide character, e.g. 'Á' little endian to be compatible with the encoded format.
 * @param c a wide character
 * @return `c` in the encoded format
 */
extern uint32_t (*_lv_txt_encoded_conv_wc)(uint32_t c);

/**
 * Decode the next encoded character from a string.
 * @param txt pointer to '\0' terminated string
 * @param i start index in 'txt' where to start.
 *                After the call it will point to the next encoded char in 'txt'.
 *                NULL to use txt[0] as index
 * @return the decoded Unicode character or 0 on invalid data code
 */
extern uint32_t (*_lv_txt_encoded_next)(const char *, uint32_t *);

/**
 * Get the previous encoded character form a string.
 * @param txt pointer to '\0' terminated string
 * @param i_start index in 'txt' where to start. After the call it will point to the previous
 * encoded char in 'txt'.
 * @return the decoded Unicode character or 0 on invalid data
 */
extern uint32_t (*_lv_txt_encoded_prev)(const char *, uint32_t *);

/**
 * Convert a letter index (in the encoded text) to byte index.
 * E.g. in UTF-8 "AÁRT" index of 'R' is 2 but start at byte 3 because 'Á' is 2 bytes long
 * @param txt a '\0' terminated UTF-8 string
 * @param enc_id letter index
 * @return byte index of the 'enc_id'th letter
 */
extern uint32_t (*_lv_txt_encoded_get_byte_id)(const char *, uint32_t);

/**
 * Convert a byte index (in an encoded text) to character index.
 * E.g. in UTF-8 "AÁRT" index of 'R' is 2 but start at byte 3 because 'Á' is 2 bytes long
 * @param txt a '\0' terminated UTF-8 string
 * @param byte_id byte index
 * @return character index of the letter at 'byte_id'th position
 */
extern uint32_t (*_lv_txt_encoded_get_char_id)(const char *, uint32_t);

/**
 * Get the number of characters (and NOT bytes) in a string.
 * E.g. in UTF-8 "ÁBC" is 3 characters (but 4 bytes)
 * @param txt a '\0' terminated char string
 * @return number of characters
 */
extern uint32_t (*_lv_txt_get_encoded_length)(const char *);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TXT_H*/
