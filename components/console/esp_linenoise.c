/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* esp_linenoise.c is derived from linenoise.c. The original copyright is 
 * reproduced below.
 * ------------------------------------------------------------------------
 *
 * linenoise.c -- guerrilla line editing library against the idea that a
 * line editing lib needs to be 20,000 lines of C code.
 *
 * You can find the latest source code at:
 *
 *   http://github.com/antirez/linenoise
 *
 * Does a number of crazy assumptions that happen to be true in 99.9999% of
 * the 2010 UNIX computers around.
 *
 * ------------------------------------------------------------------------
 *
 * Copyright (c) 2010-2016, Salvatore Sanfilippo <antirez at gmail dot com>
 * Copyright (c) 2010-2013, Pieter Noordhuis <pcnoordhuis at gmail dot com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ------------------------------------------------------------------------
 *
 * References:
 * - http://invisible-island.net/xterm/ctlseqs/ctlseqs.html
 * - http://www.3waylabs.com/nw/WWW/products/wizcon/vt220.html
 *
 * Todo list:
 * - Filter bogus Ctrl+<char> combinations.
 * - Win32 support
 *
 * Bloat:
 * - History search like Ctrl+r in readline?
 *
 * List of escape sequences used by this program, we do everything just
 * with three sequences. In order to be so cheap we may have some
 * flickering effect with some slow terminal, but the lesser sequences
 * the more compatible.
 *
 * EL (Erase Line)
 *    Sequence: ESC [ n K
 *    Effect: if n is 0 or missing, clear from cursor to end of line
 *    Effect: if n is 1, clear from beginning of line to cursor
 *    Effect: if n is 2, clear entire line
 *
 * CUF (CUrsor Forward)
 *    Sequence: ESC [ n C
 *    Effect: moves cursor forward n chars
 *
 * CUB (CUrsor Backward)
 *    Sequence: ESC [ n D
 *    Effect: moves cursor backward n chars
 *
 * The following is used to get the terminal width if getting
 * the width with the TIOCGWINSZ ioctl fails
 *
 * DSR (Device Status Report)
 *    Sequence: ESC [ 6 n
 *    Effect: reports the current cusor position as ESC [ n ; m R
 *            where n is the row and m is the column
 *
 * When multi line mode is enabled, we also use an additional escape
 * sequence. However multi line editing is disabled by default.
 *
 * CUU (Cursor Up)
 *    Sequence: ESC [ n A
 *    Effect: moves cursor up of n chars.
 *
 * CUD (Cursor Down)
 *    Sequence: ESC [ n B
 *    Effect: moves cursor down of n chars.
 *
 * When esp_linenoise_clear_screen() is called, two additional escape sequences
 * are used in order to clear the screen and position the cursor at home
 * position.
 *
 * CUP (Cursor position)
 *    Sequence: ESC [ H
 *    Effect: moves the cursor to upper left corner
 *
 * ED (Erase display)
 *    Sequence: ESC [ 2 J
 *    Effect: clear the whole screen
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "esp_log.h"

#include "esp_linenoise.h"

static const char* TAG = "linenoise";

#define LINENOISE_DEFAULT_HISTORY_MAX_LEN 100
#define LINENOISE_DEFAULT_MAX_LINE 4096
#define LINENOISE_MINIMAL_MAX_LINE 64
#define LINENOISE_COMMAND_MAX_LEN 32
#define LINENOISE_PASTE_KEY_DELAY 30 /* Delay, in milliseconds, between two characters 
                                        being pasted from clipboard */

typedef struct {
    esp_linenoise_completion_callback_t *completionCallback;
    esp_linenoise_hints_callback_t *hintsCallback;
    esp_linenoise_free_hints_callback_t *freeHintsCallback;
} linenoiseCallbacks_t;

typedef struct {
    size_t max_cmdline_length;
    int mlmode; /* Multi line mode. Default is single line. */
    int dumbmode; /* Dumb mode where line editing is disabled. Off by default */
    int history_max_len;
    int history_len;
    char **history;
    bool allow_empty;
    FILE* stdin_fp;
    FILE* stdout_fp;
} linenoiseOptions_t;

/* The linenoiseState structure represents the state during line editing.
 * We pass this state to functions implementing specific editing
 * functionalities. */
typedef struct {
    char *buf;          /* Edited line buffer. */
    size_t buflen;      /* Edited line buffer size. */
    const char *prompt; /* Prompt to display. */
    size_t plen;        /* Prompt length. */
    size_t pos;         /* Current cursor position. */
    size_t oldpos;      /* Previous refresh cursor position. */
    size_t len;         /* Current edited line length. */
    size_t cols;        /* Number of columns in terminal. */
    size_t maxrows;     /* Maximum num of rows used so far (multiline mode) */
    int history_index;  /* The history index we are currently editing. */
} linenoiseState_t;

struct esp_linenoise_t {
    char magic[10]; // "linenoise"
    linenoiseCallbacks_t callbacks;
    linenoiseOptions_t options;
    linenoiseState_t state;
};

#define LINENOISE_DEFAULT { \
    .magic = {'l','i','n','e','n','o','i','s','e','\0'}, \
    .callbacks = {0}, \
    .options = { \
        .max_cmdline_length = LINENOISE_DEFAULT_MAX_LINE, \
        .mlmode = 0, \
        .dumbmode = 0, \
        .history_max_len = LINENOISE_DEFAULT_HISTORY_MAX_LEN, \
        .history_len = 0, \
        .history = NULL, \
        .allow_empty = true, \
        .stdin_fp = 0, \
        .stdout_fp = 0, \
    },  \
    .state = {0} \
}

struct esp_linenoise_t stdio = LINENOISE_DEFAULT;

enum KEY_ACTION{
	KEY_NULL = 0,	    /* NULL */
	CTRL_A = 1,         /* Ctrl+a */
	CTRL_B = 2,         /* Ctrl-b */
	CTRL_C = 3,         /* Ctrl-c */
	CTRL_D = 4,         /* Ctrl-d */
	CTRL_E = 5,         /* Ctrl-e */
	CTRL_F = 6,         /* Ctrl-f */
	CTRL_H = 8,         /* Ctrl-h */
	TAB = 9,            /* Tab */
	CTRL_K = 11,        /* Ctrl+k */
	CTRL_L = 12,        /* Ctrl+l */
	ENTER = 10,         /* Enter */
	CTRL_N = 14,        /* Ctrl-n */
	CTRL_P = 16,        /* Ctrl-p */
	CTRL_T = 20,        /* Ctrl-t */
	CTRL_U = 21,        /* Ctrl+u */
	CTRL_W = 23,        /* Ctrl+w */
	ESC = 27,           /* Escape */
	BACKSPACE =  127    /* Backspace */
};

linenoiseOptions_t* getOptions(esp_linenoise_handle_t handle) {
    if (handle == 0) {
        stdio.options.stdin_fp = stdin;
        stdio.options.stdout_fp = stdout;
        return &stdio.options;
    }
    return &handle->options;
}

linenoiseState_t* getState(esp_linenoise_handle_t handle) {
    if (handle == 0) {return &stdio.state;}
    return &handle->state;
}

linenoiseCallbacks_t* getCallbacks(esp_linenoise_handle_t handle) {
    if (handle == 0) {return &stdio.callbacks;}
    return &handle->callbacks;
}

// forward define
static void refreshLine(esp_linenoise_handle_t handle, linenoiseState_t *l);

/* Debugging macro. */
#if 0
FILE *lndebug_fp = NULL;
#define lndebug(...) \
    do { \
        if (lndebug_fp == NULL) { \
            lndebug_fp = fopen("/tmp/lndebug.txt","a"); \
            fprintf(lndebug_fp, \
            "[%d %d %d] p: %d, rows: %d, rpos: %d, max: %d, oldmax: %d\n", \
            (int)l->len,(int)l->pos,(int)l->oldpos,plen,rows,rpos, \
            (int)l->maxrows,old_rows); \
        } \
        fprintf(lndebug_fp, ", " __VA_ARGS__); \
        fflush(lndebug_fp); \
    } while (0)
#else
#define lndebug(fmt, ...)
#endif

#define CHECK_HANDLE(handle) if(handle != NULL && strncmp(handle->magic, "linenoise", 10) != 0) { \
    ESP_LOGE(TAG, "%s - invalid handle: %p", __func__, handle); \
    return ESP_ERR_INVALID_ARG; \
} 

/* ======================= Low level terminal handling ====================== */


static void flushWrite(esp_linenoise_handle_t handle)
{
    linenoiseOptions_t* opts = getOptions(handle);

    if (__fbufsize(opts->stdout_fp) > 0) {
        fflush(opts->stdout_fp);
    }
    fsync(fileno(opts->stdout_fp));
}

/* Use the ESC [6n escape sequence to query the horizontal cursor position
 * and return it. On read error -1 is returned, On cursor error, -2 is returned.
 * On success the position of the cursor. */
static int getCursorPosition(esp_linenoise_handle_t handle)
{
    linenoiseOptions_t* opts = getOptions(handle);

    char buf[LINENOISE_COMMAND_MAX_LEN] = { 0 };

    const int out_fd = fileno(opts->stdout_fp);
    const int in_fd = fileno(opts->stdin_fp);

    /* The following ANSI escape sequence is used to get from the TTY the
     * cursor position. */
    const char get_cursor_cmd[] = "\x1b[6n";

    /* Send the command to the TTY on the other end of the UART.
     * Let's use unistd's write function. Thus, data sent through it are raw
     * reducing the overhead compared to using fputs, fprintf, etc... */
    write(out_fd, get_cursor_cmd, sizeof(get_cursor_cmd));

    /* For USB CDC, it is required to flush the output. */
    flushWrite(handle);

    /* The other end will send its response which format is ESC [ rows ; cols R
     * We don't know exactly how many bytes we have to read, thus, perform a
     * read for each byte.
     * Stop right before the last character of the buffer, to be able to NULL
     * terminate it. */
    int i = 0;
    while (i < sizeof(buf) - 1) {

        int nread = read(in_fd, buf + i, 1);
        if (nread == -1 && errno == EWOULDBLOCK) {
            return -1;
        }
        
        /* Keep using unistd's functions. Here, using `read` instead of `fgets`
         * or `fgets` guarantees us that we we can read a byte regardless on
         * whether the sender sent end of line character(s) (CR, CRLF, LF). */
        if (nread != 1 || buf[i] == 'R') {
            /* If we couldn't read a byte from opts->stdin_fp or if 'R' was received,
             * the transmission is finished. */
            break;
        }

        /* For some reasons, it is possible that we receive new line character
         * after querying the cursor position on some UART. Let's ignore them,
         * this will not affect the rest of the program. */
        if (buf[i] != '\n') {
            i++;
        }
    }

    /* NULL-terminate the buffer, this is required by `sscanf`. */
    buf[i] = '\0';

    /* Parse the received data to get the position of the cursor. */
    int cols = 0;
    int rows = 0;
    if (buf[0] != ESC || buf[1] != '[' || sscanf(buf+2, "%d;%d", &rows, &cols) != 2) {
        // cursor error
        return -2;
    }

    return cols;
}

/* Try to get the number of columns in the current terminal, or assume 80
 * if it fails. */
static int getColumns(esp_linenoise_handle_t handle)
{
    linenoiseOptions_t* opts = getOptions(handle);

    char seq[LINENOISE_COMMAND_MAX_LEN] = { 0 };

    /* The following ANSI escape sequence is used to tell the TTY to move
     * the cursor to the most-right position. */
    const char move_cursor_right[] = "\x1b[999C";
    const size_t cmd_len = sizeof(move_cursor_right);

    /* This one is used to set the cursor position. */
    const char set_cursor_pos[] = "\x1b[%dD";

    /* Get the initial position so we can restore it later. */
    int start = getCursorPosition(handle);
    if (start == -1 && errno == EWOULDBLOCK) {
        return -1;
    }
    if (start == -2) {
        goto failed;
    }

    /* Send the command to go to right margin. Use `write` function instead of
     * `fwrite` for the same reasons explained in `getCursorPosition()` */
    const int out_fd = fileno(opts->stdout_fp);
    if (write(out_fd, move_cursor_right, cmd_len) != cmd_len) {
        goto failed;
    }

    flushWrite(handle);

    /* After sending this command, we can get the new position of the cursor,
     * we'd get the size, in columns, of the opened TTY. */
    int cols = getCursorPosition(handle);
    if (cols == -1 && errno == EWOULDBLOCK) {
        return -1;
    }
    if (cols == -2) {
        goto failed;
    }

    /* Restore the position of the cursor back. */
    if (cols > start) {

        /* Generate the move cursor command. */
        int written = snprintf(seq, LINENOISE_COMMAND_MAX_LEN, set_cursor_pos, cols-start);

        /* If `written` is equal or bigger than LINENOISE_COMMAND_MAX_LEN, it
         * means that the output has been truncated because the size provided
         * is too small. */
        assert (written < LINENOISE_COMMAND_MAX_LEN);

        /* Send the command with `write`, which is not buffered. */
        if (write(out_fd, seq, written) == -1) {
            /* Can't recover... */
        }

        flushWrite(handle);
    }
    return cols;

failed:
    return 80;
}

/* Beep, used for completion when there is nothing to complete or when all
 * the choices were already shown. */
static void beep(esp_linenoise_handle_t handle)
{
    linenoiseOptions_t* opts = getOptions(handle);
    fprintf(opts->stdout_fp, "\x7");
    flushWrite(handle);
}

/* ============================== Completion ================================ */

/* Free a list of completion option populated by linenoiseAddCompletion(). */
static void freeCompletions(esp_linenoise_completions_t *lc)
{
    for (size_t i = 0; i < lc->len; i++) {
        free(lc->cvec[i]);
    }

    if (lc->cvec != NULL) {
        free(lc->cvec);
    }
}

/* This is an helper function for linenoiseEdit() and is called when the
 * user types the <tab> key in order to complete the string currently in the
 * input.
 *
 * The state of the editing is encapsulated into the pointed linenoiseState
 * structure as described in the structure definition. */
static int completeLine(esp_linenoise_handle_t handle, linenoiseState_t *ls)
{
    linenoiseOptions_t* opts = getOptions(handle);
    linenoiseCallbacks_t* callbacks = getCallbacks(handle);

    esp_linenoise_completions_t lc = {.len = 0, .cvec = NULL};
    callbacks->completionCallback(ls->buf, &lc);

    char c = 0;

    if (lc.len == 0) {

        beep(handle);

    } else {

        size_t stop = 0;
        size_t i = 0;

        while(!stop) {

            /* Show completion or original buffer */
            if (i < lc.len) {

                linenoiseState_t saved = *ls;

                ls->len = ls->pos = strlen(lc.cvec[i]);
                ls->buf = lc.cvec[i];

                refreshLine(handle,ls);

                ls->len = saved.len;
                ls->pos = saved.pos;
                ls->buf = saved.buf;

            } else {
                refreshLine(handle,ls);
            }

            int in_fd = fileno(opts->stdin_fp);
            int nread = read(in_fd, &c, 1);
            if (nread <= 0) {
                freeCompletions(&lc);
                return -1;
            }

            switch(c) {

                case TAB: /* tab */

                    i = (i+1) % (lc.len+1);
                    if (i == lc.len) {
                        beep(handle);
                    }
                    break;

                case ESC: /* escape */

                    /* Re-show original buffer */
                    if (i < lc.len) {
                        refreshLine(handle, ls);
                    }
                    stop = 1;
                    break;

                default:
                    /* Update buffer and return */
                    if (i < lc.len) {
                        int nwritten = snprintf(ls->buf, ls->buflen, "%s", lc.cvec[i]);
                        ls->len = ls->pos = nwritten;
                    }
                    stop = 1;
                    break;
            }
        }
    }

    freeCompletions(&lc);

    /* Return last read character */
    return c; 
}



/* =========================== Line editing ================================= */

/* We define a very simple "append buffer" structure, that is an heap
 * allocated string where we can append to. This is useful in order to
 * write all the escape sequences in a buffer and flush them to the standard
 * output in a single call, to avoid flickering effects. */
struct abuf {
    char *b;
    int len;
};

static void abInit(struct abuf *ab)
{
    ab->b = NULL;
    ab->len = 0;
}

static void abAppend(struct abuf *ab, const char *s, int len)
{
    char *new = realloc(ab->b, ab->len + len);
    if (new == NULL) {
        return;
    }

    memcpy(new+ab->len, s, len);
    ab->b = new;
    ab->len += len;
}

static void abFree(struct abuf *ab)
{
    free(ab->b);
}

/* Helper of refreshSingleLine() and refreshMultiLine() to show hints
 * to the right of the prompt. */
static void refreshShowHints(esp_linenoise_handle_t handle, struct abuf *ab, linenoiseState_t *l, int plen)
{
    linenoiseCallbacks_t* callbacks = getCallbacks(handle);

    char seq[64];

    if (callbacks->hintsCallback && plen+l->len < l->cols) {

        int color = -1;
        int bold = 0;

        char *hint = callbacks->hintsCallback(l->buf, &color, &bold);

        if (hint) {

            int hintlen = strlen(hint);
            int hintmaxlen = l->cols-(plen+l->len);
            if (hintlen > hintmaxlen) {
                hintlen = hintmaxlen;
            }

            if (bold == 1 && color == -1) {
                color = 37;
            }

            if (color != -1 || bold != 0) {
                snprintf(seq, 64, "\033[%d;%d;49m", bold, color);
                abAppend(ab, seq, strlen(seq));
            }

            abAppend(ab, hint, hintlen);

            if (color != -1 || bold != 0) {
                abAppend(ab, "\033[0m", 4);
            }

            /* Call the function to free the hint returned. */
            if (callbacks->freeHintsCallback) {
                callbacks->freeHintsCallback(hint);
            }
        }
    }
}

/* Single line low level line refresh.
 *
 * Rewrite the currently edited line accordingly to the buffer content,
 * cursor position, and number of columns of the terminal. */
static void refreshSingleLine(esp_linenoise_handle_t handle, linenoiseState_t *l)
{
    linenoiseOptions_t* opts = getOptions(handle);

    char *buf = l->buf;
    size_t plen = l->plen;
    size_t len = l->len;
    size_t pos = l->pos;

    while((plen + pos) >= l->cols) {
        buf++;
        len--;
        pos--;
    }

    while (plen + len > l->cols) {
        len--;
    }

    struct abuf ab;
    abInit(&ab);

    /* Cursor to left edge */
    char seq[64];
    snprintf(seq, 64, "\r");
    abAppend(&ab, seq, strlen(seq));

    /* Write the prompt and the current buffer content */
    abAppend(&ab, l->prompt, strlen(l->prompt));
    abAppend(&ab, buf, len);

    /* Show hits if any. */
    refreshShowHints(handle, &ab, l, plen);

    /* Erase to right */
    snprintf(seq, 64, "\x1b[0K");
    abAppend(&ab, seq, strlen(seq));

    /* Move cursor to original position. */
    snprintf(seq, 64, "\r\x1b[%dC", (int)(pos+plen));
    abAppend(&ab, seq, strlen(seq));

    const int out_fd = fileno(opts->stdout_fp);
    if (write(out_fd, ab.b, ab.len) == -1) {
        /* Can't recover from write error. */
    } 
    
    flushWrite(handle);

    abFree(&ab);
}

/* Multi line low level line refresh.
 *
 * Rewrite the currently edited line accordingly to the buffer content,
 * cursor position, and number of columns of the terminal. */
static void refreshMultiLine(esp_linenoise_handle_t handle, linenoiseState_t *l)
{
    linenoiseOptions_t* opts = getOptions(handle);

    char seq[64];

    int plen = l->plen;
    int rows = (plen + l->len + l->cols - 1) / l->cols; /* rows used by current buf. */
    int rpos = (plen + l->oldpos + l->cols) / l->cols; /* cursor relative row. */
    int old_rows = l->maxrows;

    /* Update maxrows if needed. */
    if (rows > (int)l->maxrows) {
        l->maxrows = rows;
    }

    /* First step: clear all the lines used before. To do so start by
     * going to the last row. */
    struct abuf ab;
    abInit(&ab);
    if (old_rows - rpos > 0) {
        lndebug("go down %d", old_rows-rpos);
        snprintf(seq, 64, "\x1b[%dB", old_rows-rpos);
        abAppend(&ab, seq, strlen(seq));
    }

    /* Now for every row clear it, go up. */
    for (int j = 0; j < old_rows - 1; j++) {
        lndebug("clear+up");
        snprintf(seq, 64, "\r\x1b[0K\x1b[1A");
        abAppend(&ab, seq, strlen(seq));
    }

    /* Clean the top line. */
    lndebug("clear");
    snprintf(seq, 64, "\r\x1b[0K");
    abAppend(&ab, seq, strlen(seq));

    /* Write the prompt and the current buffer content */
    abAppend(&ab, l->prompt, strlen(l->prompt));
    abAppend(&ab, l->buf, l->len);

    /* Show hits if any. */
    refreshShowHints(handle, &ab, l, plen);

    /* If we are at the very end of the screen with our prompt, we need to
     * emit a newline and move the prompt to the first column. */
    if (l->pos &&
        l->pos == l->len &&
        (l->pos + plen) % l->cols == 0)
    {
        lndebug("<newline>");
        abAppend(&ab, "\n", 1);
        snprintf(seq, 64, "\r");
        abAppend(&ab, seq, strlen(seq));
        rows++;
        if (rows > (int)l->maxrows) {
            l->maxrows = rows;
        }
    }

    /* Move cursor to right position. */
    /* rpos2 = after refresh. */
    int rpos2 = (plen+l->pos + l->cols) / l->cols; /* current cursor relative row. */
    lndebug("rpos2 %d", rpos2);

    /* Go up till we reach the expected positon. */
    if (rows - rpos2 > 0) {
        lndebug("go-up %d", rows - rpos2);
        snprintf(seq, 64, "\x1b[%dA", rows - rpos2);
        abAppend(&ab, seq, strlen(seq));
    }

    /* Set column. */
    int col = (plen + (int) l->pos) % (int) l->cols;
    lndebug("set col %d", 1 + col);
    if (col) {
        snprintf(seq, 64, "\r\x1b[%dC", col);
    } else {
        snprintf(seq, 64, "\r");
    }
    abAppend(&ab, seq, strlen(seq));

    lndebug("\n");
    l->oldpos = l->pos;

    const int out_fd = fileno(opts->stdout_fp);
    if (write(out_fd, ab.b, ab.len) == -1) {
        /* Can't recover from write error. */
    }

    flushWrite(handle);
    
    abFree(&ab);
}

/* Calls the two low level functions refreshSingleLine() or
 * refreshMultiLine() according to the selected mode. */
static void refreshLine(esp_linenoise_handle_t handle, linenoiseState_t *l)
{
    linenoiseOptions_t* opts = getOptions(handle);

    if (opts->mlmode) {
        refreshMultiLine(handle, l);
    } else {
        refreshSingleLine(handle, l);
    }
}

/* Insert the character 'c' at cursor current position.
 *
 * On error writing to the terminal -1 is returned, otherwise 0. */
static int editInsert(esp_linenoise_handle_t handle, linenoiseState_t *l, char c)
{
    linenoiseOptions_t* opts = getOptions(handle);
    linenoiseCallbacks_t* callbacks = getCallbacks(handle);

    if (l->len < l->buflen) {

        if (l->len == l->pos) {

            l->buf[l->pos] = c;
            l->pos++;
            l->len++;
            l->buf[l->len] = '\0';

            if ((!opts->mlmode && l->plen + l->len < l->cols && !callbacks->hintsCallback)) {

                /* Avoid a full update of the line in the
                 * trivial case. */
                const int out_fd = fileno(opts->stdout_fp);
                if (write(out_fd, &c,1) == -1) {
                    return -1;
                }

                flushWrite(handle);

            } else {

                refreshLine(handle,l);
            }

        } else {

            void* dest = l->buf + l->pos + 1;
            void* src  = l->buf + l->pos;
            size_t bytes = l->len - l->pos;
            memmove(dest, src, bytes);

            l->buf[l->pos] = c;
            l->len++;
            l->pos++;
            l->buf[l->len] = '\0';

            refreshLine(handle, l);
        }
    }
    return 0;
}

static int insertPastedChar(esp_linenoise_handle_t handle, linenoiseState_t *l, char c)
{
    linenoiseOptions_t* opts = getOptions(handle);

    if (l->len < l->buflen && l->len == l->pos) {

        l->buf[l->pos] = c;
        l->pos++;
        l->len++;
        l->buf[l->len] = '\0';

        const int out_fd = fileno(opts->stdout_fp);
        if (write(out_fd, &c,1) == -1) {
            return -1;
        }

        flushWrite(handle);
    }
    return 0;
}

/* Move cursor on the left. */
static void editMoveLeft(esp_linenoise_handle_t handle, linenoiseState_t *l)
{
    if (l->pos > 0) {
        l->pos--;
        refreshLine(handle,l);
    }
}

/* Move cursor on the right. */
static void editMoveRight(esp_linenoise_handle_t handle, linenoiseState_t *l)
{
    if (l->pos != l->len) {
        l->pos++;
        refreshLine(handle,l);
    }
}

/* Move cursor to the start of the line. */
static void editMoveHome(esp_linenoise_handle_t handle, linenoiseState_t *l)
{
    if (l->pos != 0) {
        l->pos = 0;
        refreshLine(handle,l);
    }
}

/* Move cursor to the end of the line. */
static void editMoveEnd(esp_linenoise_handle_t handle, linenoiseState_t *l)
{
    if (l->pos != l->len) {
        l->pos = l->len;
        refreshLine(handle,l);
    }
}

/* Substitute the currently edited line with the next or previous history
 * entry as specified by 'dir'. */
#define LINENOISE_HISTORY_NEXT 0
#define LINENOISE_HISTORY_PREV 1
static void editHistoryNext(esp_linenoise_handle_t handle, linenoiseState_t *l, int dir)
{
    linenoiseOptions_t* opts = getOptions(handle);

    if (opts->history_len > 1) {

        /* Update the current history entry before to
         * overwrite it with the next one. */
        int idx = opts->history_len - 1 - l->history_index;
        free(opts->history[idx]);
        opts->history[idx] = strdup(l->buf);

        /* Show the new entry */
        l->history_index += (dir == LINENOISE_HISTORY_PREV) ? 1 : -1;

        // hit the begining?
        if (l->history_index < 0) {
            l->history_index = 0;
            return;
        }
        
        // hit the end?
        if (l->history_index >= opts->history_len) {
            l->history_index = opts->history_len - 1;
            return;
        }

        // recalculate idx
        idx = opts->history_len - 1 - l->history_index;

        // copy from history to linenoiseState's buffer
        char *dest = l->buf;
        char *src  = opts->history[idx];
        size_t bytes = l->buflen;
        strncpy(dest, src, bytes);

        // guarentee null terminated
        l->buf[l->buflen - 1] = '\0';
        
        // set buffer len & pos
        l->len = l->pos = strlen(l->buf);

        refreshLine(handle,l);
    }
}

/* Delete the character at the right of the cursor without altering the cursor
 * position. Basically this is what happens with the "Delete" keyboard key. */
static void editDelete(esp_linenoise_handle_t handle, linenoiseState_t *l)
{
    if (l->len > 0 && l->pos < l->len) {

        // remove character at 'pos+1'
        void* dest = l->buf + l->pos;
        void* src  = l->buf + l->pos + 1;
        size_t bytes = l->len - l->pos - 1;
        memmove(dest, src, bytes);

        l->len--;
        l->buf[l->len] = '\0';

        refreshLine(handle,l);
    }
}

/* Backspace implementation. */
static void editBackspace(esp_linenoise_handle_t handle, linenoiseState_t *l)
{
    if (l->pos > 0 && l->len > 0) {

        // remove character at 'pos'
        void* dest = l->buf + l->pos - 1;
        void* src  = l->buf + l->pos;
        size_t bytes = l->len - l->pos;
        memmove(dest, src, bytes);

        l->pos--;
        l->len--;
        l->buf[l->len] = '\0';

        refreshLine(handle,l);
    }
}

/* Delete the previosu word, maintaining the cursor at the start of the
 * current word. */
static void editDeletePrevWord(esp_linenoise_handle_t handle, linenoiseState_t *l)
{
    size_t old_pos = l->pos;
    size_t diff;

    while (l->pos > 0 && l->buf[l->pos-1] == ' ') {
        l->pos--;
    }

    while (l->pos > 0 && l->buf[l->pos-1] != ' ') {
        l->pos--;
    }

    diff = old_pos - l->pos;

    // remove word
    void* dest = l->buf + l->pos;
    void* src  = l->buf + old_pos;
    size_t bytes = l->len - old_pos + 1;
    memmove(dest, src, bytes);

    l->len -= diff;

    refreshLine(handle,l);
}

static uint32_t getMillis(void)
{
    struct timeval tv = { 0 };
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/* This function is the core of the line editing capability of linenoise.
 * It expects 'fd' to be already in "raw mode" so that every key pressed
 * will be returned ASAP to read().
 *
 * The resulting string is put into 'buf' when the user type enter, or
 * when ctrl+d is typed.
 *
 * The function returns the length of the current buffer. */
static int linenoiseEdit(esp_linenoise_handle_t handle, char *buf, size_t buflen, const char *prompt)
{
    linenoiseOptions_t* opts = getOptions(handle);
    linenoiseCallbacks_t* callbacks = getCallbacks(handle);

    const int out_fd = fileno(opts->stdout_fp);
    const int in_fd = fileno(opts->stdin_fp);

    int cols = getColumns(handle);
    if (cols == -1 && errno == EWOULDBLOCK) {
        return -1;
    }

    /* Populate the linenoise state that we pass to functions implementing
     * specific editing functionalities. */
    linenoiseState_t l;
    l.buf = buf;
    l.buflen = buflen;
    l.prompt = prompt;
    l.plen = strlen(prompt);
    l.oldpos = l.pos = 0;
    l.len = 0;
    l.maxrows = 0;
    l.history_index = 0;
    l.cols = cols;

    /* Buffer starts empty. */
    l.buf[0] = '\0';
    l.buflen--; /* Make sure there is always space for the nulterm */

    /* The latest history entry is always our current buffer, that
     * initially is just an empty string. */
    esp_linenoise_history_add(handle,"");

    int pos1 = getCursorPosition(handle);
    if (pos1 == -1 && errno == EWOULDBLOCK) {
        return -1;
    }

    if (write(out_fd, prompt, l.plen) == -1) {
        return -1;
    }
    
    flushWrite(handle);
    
    int pos2 = getCursorPosition(handle);
    if (pos2 == -1 && errno == EWOULDBLOCK) {
        return -1;
    }

    if (pos1 >= 0 && pos2 >= 0) {
        l.plen = pos2 - pos1;
    }

    while(1) {

        /**
         * To determine whether the user is pasting data or typing itself, we
         * need to calculate how many milliseconds elapsed between two key
         * presses. Indeed, if there is less than LINENOISE_PASTE_KEY_DELAY
         * (typically 30-40ms), then a paste is being performed, else, the
         * user is typing.
         * NOTE: pressing a key down without releasing it will also spend
         * about 40ms (or even more)
         */
        uint32_t t1 = getMillis();
        char c = 0;
        int nread = read(in_fd, &c, 1);
        if (nread == -1 && errno == EWOULDBLOCK) {
            return -1;
        }
        if (nread <= 0) {
            return l.len;
        }

        if ( (getMillis() - t1) < LINENOISE_PASTE_KEY_DELAY && c != ENTER) {

            /* Pasting data, insert characters without formatting.
             * This can only be performed when the cursor is at the end of the
             * line. */
            if (insertPastedChar(handle,&l,c)) {
                return -1;
            }

            continue;
        }

        /* Only autocomplete when the callback is set. It returns < 0 when
         * there was an error reading from fd. Otherwise it will return the
         * character that should be handled next. */
        if (c == 9 && callbacks->completionCallback != NULL) {

            int c2 = completeLine(handle, &l);

            /* Return on errors */
            if (c2 < 0) {
                return l.len;
            }

            /* Read next character when 0 */
            if (c2 == 0) {
                continue;
            }

            c = c2;
        }

        switch(c) {
        case ENTER:    /* enter */

            opts->history_len--;
            free(opts->history[opts->history_len]);

            if (opts->mlmode) {
                editMoveEnd(handle, &l);
            }

            if (callbacks->hintsCallback) {

                /* Force a refresh without hints to leave the previous
                 * line as the user typed it after a newline. */
                esp_linenoise_hints_callback_t *hc = callbacks->hintsCallback;
                callbacks->hintsCallback = NULL;

                refreshLine(handle, &l);
                callbacks->hintsCallback = hc;
            }

            return (int)l.len;

        case CTRL_C:     /* ctrl-c */

            errno = EAGAIN;
            return -1;

        case BACKSPACE:   /* backspace */
        case 8:           /* ctrl-h */

            editBackspace(handle, &l);
            break;

        case CTRL_D:     /* ctrl-d, remove char at right of cursor, or if the
                            line is empty, act as end-of-file. */

            if (l.len > 0) {
                editDelete(handle, &l);
            } else {
                opts->history_len--;
                free(opts->history[opts->history_len]);
                return -1;
            }
            break;

        case CTRL_T:    /* ctrl-t, swaps current character with previous. */

            if (l.pos > 0 && l.pos < l.len) {
                int aux = buf[l.pos-1];
                buf[l.pos-1] = buf[l.pos];
                buf[l.pos] = aux;
                if (l.pos != l.len-1) {
                    l.pos++;
                }
                refreshLine(handle, &l);
            }
            break;

        case CTRL_B:    /* ctrl-b */

            editMoveLeft(handle, &l);
            break;

        case CTRL_F:    /* ctrl-f */

            editMoveRight(handle, &l);
            break;

        case CTRL_P:    /* ctrl-p */

            editHistoryNext(handle, &l, LINENOISE_HISTORY_PREV);
            break;

        case CTRL_N:    /* ctrl-n */

            editHistoryNext(handle, &l, LINENOISE_HISTORY_NEXT);
            break;

        case ESC:{      /* escape sequence */

            /* Read the next two bytes representing the escape sequence. */
            char seq[3];
            int n = read(in_fd, seq, 2);
            if (n == -1 && errno == EWOULDBLOCK) {
                return -1;
            }
            if (n < 2) {
                break;
            }

            /* ESC [ sequences. */
            if (seq[0] == '[') {

                if (seq[1] >= '0' && seq[1] <= '9') {

                    /* Extended escape, read additional byte. */
                    int n = read(in_fd, seq+2, 1);
                    if (n == -1 && errno == EWOULDBLOCK) {
                        return -1;
                    }
                    if (n == -1) {
                        break;
                    }

                    if (seq[2] == '~') {
                        switch(seq[1]) {
                        case '3': /* Delete key. */
                            editDelete(handle, &l);
                            break;
                        }
                    }

                } else {
                    
                    switch(seq[1]) {
                    case 'A': /* Up */

                        editHistoryNext(handle, &l, LINENOISE_HISTORY_PREV);
                        break;

                    case 'B': /* Down */

                        editHistoryNext(handle, &l, LINENOISE_HISTORY_NEXT);
                        break;

                    case 'C': /* Right */
                    
                        editMoveRight(handle, &l);
                        break;

                    case 'D': /* Left */

                        editMoveLeft(handle, &l);
                        break;

                    case 'H': /* Home */

                        editMoveHome(handle, &l);
                        break;

                    case 'F': /* End*/

                        editMoveEnd(handle, &l);
                        break;
                    }
                }
            }

            /* ESC O sequences. */
            else if (seq[0] == 'O') {

                switch(seq[1]) {
                case 'H': /* Home */

                    editMoveHome(handle, &l);
                    break;

                case 'F': /* End*/

                    editMoveEnd(handle, &l);
                    break;
                }
            }
            break;
        }
        default:

            if (editInsert(handle, &l, c)) {
                return -1;
            }
            break;

        case CTRL_U: /* Ctrl+u, delete the whole line. */

            buf[0] = '\0';
            l.pos = l.len = 0;
            refreshLine(handle, &l);
            break;

        case CTRL_K: /* Ctrl+k, delete from current to end of line. */

            buf[l.pos] = '\0';
            l.len = l.pos;
            refreshLine(handle, &l);
            break;

        case CTRL_A: /* Ctrl+a, go to the start of the line */

            editMoveHome(handle, &l);
            break;

        case CTRL_E: /* ctrl+e, go to the end of the line */

            editMoveEnd(handle, &l);
            break;

        case CTRL_L: /* ctrl+l, clear screen */

            esp_linenoise_clear_screen(handle);
            refreshLine(handle, &l);
            break;

        case CTRL_W: /* ctrl+w, delete previous word */

            editDeletePrevWord(handle, &l);
            break;

        }
        flushWrite(handle);
    }
    return l.len;
}

static int linenoiseRaw(esp_linenoise_handle_t handle, char *buf, size_t buflen, const char *prompt)
{
    int count;

    linenoiseOptions_t* opts = getOptions(handle);

    if (buflen == 0) {
        errno = EINVAL;
        return -1;
    }

    count = linenoiseEdit(handle, buf, buflen, prompt);
    if (count != -1) {
        fputc('\n', opts->stdout_fp);
        flushWrite(handle);
    }

    return count;
}

static int linenoiseDumb(esp_linenoise_handle_t handle, char* buf, size_t buflen, const char* prompt)
{
    linenoiseOptions_t* opts = getOptions(handle);

    /* dumb terminal, fall back to fgets */
    fputs(prompt, opts->stdout_fp);
    size_t count = 0;
    while (count < buflen) {

        int c = fgetc(opts->stdin_fp);
        if (c == -1 && errno == EWOULDBLOCK) {
            return -1;
        }

        if (c == '\n') {

            break;

        } else if (c >= 0x1c && c <= 0x1f) {

            continue; /* consume arrow keys */

        } else if (c == BACKSPACE || c == 0x8) {

            if (count > 0) {
                buf[count - 1] = 0;
                count--;
            }

            /* Windows CMD: erase symbol under cursor */
            fputs("\x08 ", opts->stdout_fp); 

        } else {

            buf[count] = c;
            ++count;
        }
        fputc(c, opts->stdout_fp); /* echo */
    }
    fputc('\n', opts->stdout_fp);
    flushWrite(handle);
    return count;
}

static void sanitize(char* src) {
    char* dst = src;
    for (int c = *src; c != 0; src++, c = *src) {
        if (isprint(c)) {
            *dst = c;
            ++dst;
        }
    }
    *dst = 0;
}


////////////////////////////////////////////////////////////////////////////////
//
//
// Public Api
//
//

esp_linenoise_handle_t esp_linenoise_create()
{   
    struct esp_linenoise_t def = LINENOISE_DEFAULT;
    esp_linenoise_handle_t handle = (esp_linenoise_handle_t) malloc(sizeof(struct esp_linenoise_t));
    memcpy(handle, &def, sizeof(struct esp_linenoise_t));
    return handle;
}

char *esp_linenoise(esp_linenoise_handle_t handle, const char *prompt)
{
    linenoiseOptions_t* opts = getOptions(handle);

    char *buf = calloc(1, opts->max_cmdline_length);
    if (buf == NULL) {
        return NULL;
    }

    int count = 0;
    if (!opts->dumbmode) {
        count = linenoiseRaw(handle, buf, opts->max_cmdline_length, prompt);
    } else {
        count = linenoiseDumb(handle, buf, opts->max_cmdline_length, prompt);
    }

    if (count > 0) {

        sanitize(buf);
        count = strlen(buf);

    } else if (count == 0 && opts->allow_empty) {

        /* will return an empty (0-length) string */

    } else {

        free(buf);
        return NULL;
    }
    return buf;
}

esp_err_t esp_linenoise_probe(esp_linenoise_handle_t handle)
{
    CHECK_HANDLE(handle);

    linenoiseOptions_t* opts = getOptions(handle);

    /* Switch to non-blocking mode */
    int stdin_fileno = fileno(opts->stdin_fp);
    int flags = fcntl(stdin_fileno, F_GETFL);
    flags |= O_NONBLOCK;
    int res = fcntl(stdin_fileno, F_SETFL, flags);
    if (res != 0) {
        ESP_LOGE(TAG, "could not switch to non-blocking mode");
        return ESP_FAIL;
    }

    /* Device status request */
    fprintf(opts->stdout_fp, "\x1b[5n");
    flushWrite(handle);

    /* Try to read response */
    int timeout_ms = 200;
    const int retry_ms = 10;
    size_t read_bytes = 0;
    while (timeout_ms > 0 && read_bytes < 4) { 

        // expcted response is ESC[0n or ESC[3n

        usleep(retry_ms * 1000);
        timeout_ms -= retry_ms;

        char c;
        int nread = read(stdin_fileno, &c, 1);
        if (nread == -1 && errno == EWOULDBLOCK) {
            /* Note! Due to the fcntl call above, this is read in non-blocking mode!
             * So, nread == -1 && errno == EWOULDBLOCK is expected! */
            continue;
        }
        if (read_bytes == 0 && c != '\x1b') {
            /* invalid response */
            break;
        }

        read_bytes += nread;
    }

    // Restore old mode
    flags &= ~O_NONBLOCK;
    res = fcntl(stdin_fileno, F_SETFL, flags);
    if (res != 0) {
        ESP_LOGE(TAG, "could not restore original blocking/non-blocking mode");
        return ESP_FAIL;
    }

    /* Did the terminal send the expected number of bytes */
    if (read_bytes < 4) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_linenoise_set_stdin(esp_linenoise_handle_t handle, FILE* fp)
{
    CHECK_HANDLE(handle);
    linenoiseOptions_t* opts = getOptions(handle);
    opts->stdin_fp = fp;
    return ESP_OK;
}

esp_err_t esp_linenoise_set_stdout(esp_linenoise_handle_t handle, FILE* fp)
{
    CHECK_HANDLE(handle);
    linenoiseOptions_t* opts = getOptions(handle);
    opts->stdout_fp = fp;
    return ESP_OK;
}

esp_err_t esp_linenoise_set_multiline(esp_linenoise_handle_t handle, int mlmode)
{
    CHECK_HANDLE(handle);
    linenoiseOptions_t* opts = getOptions(handle);
    opts->mlmode = mlmode;
    return ESP_OK;
}

esp_err_t esp_linenoise_set_dumb_mode(esp_linenoise_handle_t handle, int dumbmode)
{
    CHECK_HANDLE(handle);
    linenoiseOptions_t* opts = getOptions(handle);
    opts->dumbmode = dumbmode;
    return ESP_OK;
}

esp_err_t esp_linenoise_is_dumb_mode(esp_linenoise_handle_t handle, bool *is_dumb_mode)
{
    CHECK_HANDLE(handle);

    if (is_dumb_mode == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    linenoiseOptions_t* opts = getOptions(handle);
    *is_dumb_mode = opts->dumbmode;
    return ESP_OK;
}

esp_err_t esp_linenoise_set_allow_empty(esp_linenoise_handle_t handle, bool allow_empty)
{
    CHECK_HANDLE(handle);
    linenoiseOptions_t* opts = getOptions(handle);
    opts->allow_empty = allow_empty;
    return ESP_OK;
}

esp_err_t esp_linenoise_set_max_line_length(esp_linenoise_handle_t handle, size_t length)
{
    CHECK_HANDLE(handle);

    if (length < LINENOISE_MINIMAL_MAX_LINE) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    linenoiseOptions_t* opts = getOptions(handle);
    opts->max_cmdline_length = length;

    return ESP_OK;
}

esp_err_t esp_linenoise_clear_screen(esp_linenoise_handle_t handle)
{
    CHECK_HANDLE(handle);
    linenoiseOptions_t* opts = getOptions(handle);
    fprintf(opts->stdout_fp, "\x1b[H\x1b[2J");
    flushWrite(handle);
    return ESP_OK;
}

/* ================================ Callbacks ================================= */

esp_err_t esp_linenoise_set_completions_callback(esp_linenoise_handle_t handle, 
                                                 esp_linenoise_completion_callback_t *fn)
{
    CHECK_HANDLE(handle);
    linenoiseCallbacks_t* callbacks = getCallbacks(handle);
    callbacks->completionCallback = fn;
    return ESP_OK;
}

esp_err_t esp_linenoise_set_hints_callback(esp_linenoise_handle_t handle, 
                                           esp_linenoise_hints_callback_t *fn)
{
    CHECK_HANDLE(handle);
    linenoiseCallbacks_t* callbacks = getCallbacks(handle);
    callbacks->hintsCallback = fn;
    return ESP_OK;
}

esp_err_t esp_linenoise_set_free_hints_callback(esp_linenoise_handle_t handle, 
                                                esp_linenoise_free_hints_callback_t *fn)
{
    CHECK_HANDLE(handle);
    linenoiseCallbacks_t* callbacks = getCallbacks(handle);
    callbacks->freeHintsCallback = fn;
    return ESP_OK;
}

esp_err_t esp_linenoise_add_completion(esp_linenoise_completions_t *lc, const char *str)
{
    size_t len = strlen(str);

    char *copy = malloc(len+1);
    if (copy == NULL) {
        return ESP_ERR_NO_MEM;
    }

    memcpy(copy, str, len+1);

    char **cvec = realloc(lc->cvec, sizeof(char*) * (lc->len + 1));
    if (cvec == NULL) {
        free(copy);
        return ESP_ERR_NO_MEM;
    }

    lc->cvec = cvec;
    lc->cvec[lc->len++] = copy;

    return ESP_OK;
}

/* ================================ History ================================= */

esp_err_t esp_linenoise_history_free(esp_linenoise_handle_t handle)
{
    CHECK_HANDLE(handle);

    linenoiseOptions_t* opts = getOptions(handle);

    if (opts->history) {
        for (int j = 0; j < opts->history_len; j++) {
            free(opts->history[j]);
        }
        free(opts->history);
    }

    opts->history = NULL;

    return ESP_OK;
}

esp_err_t esp_linenoise_history_add(esp_linenoise_handle_t handle, const char *line)
{
    CHECK_HANDLE(handle);

    // check args
    if (line == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    linenoiseOptions_t* opts = getOptions(handle);

    // check that history is enabled
    if (opts->history_max_len == 0) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    /* Initialization on first call. */
    if (opts->history == NULL) {

        opts->history = malloc(sizeof(char*) * opts->history_max_len);
        if (opts->history == NULL) {
            return ESP_ERR_NO_MEM;
        }

        memset(opts->history, 0, (sizeof(char*) * opts->history_max_len));
    }

    /* Don't add duplicated lines. */
    if (opts->history_len && !strcmp(opts->history[opts->history_len - 1], line)) {
        return ESP_OK;
    }

    /* Add an heap allocated copy of the line in the history.
     * If we reached the max length, remove the older line. */
    char *linecopy = strdup(line);
    if (!linecopy) {
        return ESP_ERR_NO_MEM;
    }

    /* If we are out of space, pop the least recent entry */
    if (opts->history_len == opts->history_max_len) {

        free(opts->history[0]);

        void* dest = opts->history;
        void* src  = opts->history + 1;
        size_t bytes = sizeof(char*) * (opts->history_max_len - 1);
        memmove(dest, src, bytes);

        opts->history_len--;
    }

    opts->history[opts->history_len] = linecopy;
    opts->history_len++;

    return ESP_OK;
}

esp_err_t esp_linenoise_history_set_max_length(esp_linenoise_handle_t handle, int length)
{
    CHECK_HANDLE(handle);

    // check args
    if (length < 1) {
        return ESP_ERR_INVALID_ARG;
    }

    linenoiseOptions_t* opts = getOptions(handle);

    if (opts->history) {

        int tocopy = opts->history_len;

        char **new = malloc(sizeof(char*) * length);
        if (new == NULL) {
            return ESP_ERR_NO_MEM;
        }

        /* If we can't copy everything, free the elements we'll not use. */
        if (length < tocopy) {
            for (int j = 0; j < tocopy - length; j++) {
                free(opts->history[j]);
            }
            tocopy = length;
        }

        memset(new, 0, sizeof(char*) * length);
        memcpy(new, opts->history + (opts->history_len - tocopy), sizeof(char*) * tocopy);

        free(opts->history);
        opts->history = new;
    }

    opts->history_max_len = length;

    if (opts->history_len > opts->history_max_len) {
        opts->history_len = opts->history_max_len;
    }

    return ESP_OK;
}

esp_err_t esp_linenoise_history_save(esp_linenoise_handle_t handle, const char *filename)
{
    CHECK_HANDLE(handle);

    // check args
    if (filename == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t rv = ESP_OK;
    FILE *fp = NULL;

    {
        linenoiseOptions_t* opts = getOptions(handle);

        fp = fopen(filename, "w");
        if (fp == NULL) {
            rv = ESP_FAIL;
            goto end;
        }

        for (int j = 0; j < opts->history_len; j++) {

            int rv = fprintf(fp, "%s\n", opts->history[j]);
            if (rv < 0) {
                ESP_LOGE(TAG, "fprintf failed. rv: %i", rv);
                rv = ESP_FAIL;
                goto end;
            }
        }
    }
end:
    if (fp) {
        if (fclose(fp) != 0) {
            ESP_LOGE(TAG, "fclose failed");
            return ESP_FAIL;
        }
    }

    return rv;
}

esp_err_t esp_linenoise_history_load(esp_linenoise_handle_t handle, const char *filename)
{
    CHECK_HANDLE(handle);

    // check args
    if (filename == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t rv = ESP_OK;
    FILE *fp = NULL;
    char *buf = NULL;

    {
        linenoiseOptions_t* opts = getOptions(handle);

        fp = fopen(filename, "r");
        if (fp == NULL) {
            rv = ESP_FAIL;
            goto end;
        }

        buf = calloc(1, opts->max_cmdline_length);
        if (buf == NULL) {
            rv = ESP_ERR_NO_MEM;
            goto end;
        }

        while (fgets(buf, opts->max_cmdline_length, fp) != NULL) {

            char *p = strchr(buf,'\r');
            if (!p) {
                p = strchr(buf,'\n');
            }
            if (p) {
                *p = '\0';
            }

            // add to history
            rv = esp_linenoise_history_add(handle, buf);
            if (rv != ESP_OK) {
                goto end;
            }
        }
    }

end:
    free(buf);

    if (fp) {
        if (fclose(fp) != 0) {
            ESP_LOGE(TAG, "fclose failed");
            return ESP_FAIL;
        }
    }

    return rv;
}

/* ================================ Free ================================= */

esp_err_t  esp_linenoise_free(void *ptr)
{
    free(ptr);
    return ESP_OK;
}

esp_err_t esp_linenoise_destroy(esp_linenoise_handle_t handle)
{
    CHECK_HANDLE(handle);
    memset(handle, 0, sizeof(struct esp_linenoise_t));
    free(handle);
    return ESP_OK;
}
