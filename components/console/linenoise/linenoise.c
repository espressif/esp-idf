/* linenoise.c -- guerrilla line editing library against the idea that a
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
 * When linenoise2ClearScreen() is called, two additional escape sequences
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
#include "linenoise.h"

#define LINENOISE_DEFAULT_HISTORY_MAX_LEN 100
#define LINENOISE_DEFAULT_MAX_LINE 4096
#define LINENOISE_MINIMAL_MAX_LINE 64
#define LINENOISE_COMMAND_MAX_LEN 32
#define LINENOISE_PASTE_KEY_DELAY 30 /* Delay, in milliseconds, between two characters 
                                        being pasted from clipboard */

typedef struct {
    linenoiseCompletionCallback *completionCallback;
    linenoiseHintsCallback *hintsCallback;
    linenoiseFreeHintsCallback *freeHintsCallback;
} linenoiseCallbacks_t;

typedef struct {
    size_t max_cmdline_length;
    int mlmode; /* Multi line mode. Default is single line. */
    int dumbmode; /* Dumb mode where line editing is disabled. Off by default */
    int history_max_len;
    int history_len;
    char **history;
    bool allow_empty;
    FILE* fpIn;
    FILE* fpOut;
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

struct linenoise_t{
    linenoiseCallbacks_t callbacks;
    linenoiseOptions_t options;
    linenoiseState_t state;
};

#define LINENOISE_DEFAULT { \
    .callbacks = {0}, \
    .options = { \
        .max_cmdline_length = LINENOISE_DEFAULT_MAX_LINE, \
        .mlmode = 0, \
        .dumbmode = 0, \
        .history_max_len = LINENOISE_DEFAULT_HISTORY_MAX_LEN, \
        .history_len = 0, \
        .history = NULL, \
        .allow_empty = true, \
        .fpIn = 0, \
        .fpOut = 0, \
    },  \
    .state = {0} \
}

struct linenoise_t stdio = LINENOISE_DEFAULT;

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

linenoiseOptions_t* getOptions(linenoiseHandle_t handle) {
    if (handle == 0) {
        stdio.options.fpIn = stdin;
        stdio.options.fpOut = stdout;
        return &stdio.options;
    }
    return &handle->options;
}

linenoiseState_t* getState(linenoiseHandle_t handle) {
    if (handle == 0) {return &stdio.state;}
    return &handle->state;
}

linenoiseCallbacks_t* getCallbacks(linenoiseHandle_t handle) {
    if (handle == 0) {return &stdio.callbacks;}
    return &handle->callbacks;
}

////////////////////////////////////////////////////////////////////
// Linenoise 1
//

void linenoiseSetCompletionCallback(linenoiseCompletionCallback *cb)     {linenoise2SetCompletionCallback(0, cb);}
void linenoiseSetHintsCallback(linenoiseHintsCallback *cb)               {linenoise2SetHintsCallback(0, cb);}
void linenoiseSetFreeHintsCallback(linenoiseFreeHintsCallback *cb)       {linenoise2SetFreeHintsCallback(0, cb);}
void linenoiseAddCompletion(linenoiseCompletions *cmpl, const char * str){linenoise2AddCompletion(cmpl, str);}

int   linenoiseProbe(void)                      {return linenoise2Probe(0);}
char* linenoise(const char *prompt)             {return linenoise2(0, prompt);}
void  linenoiseFree(void *ptr)                  {return linenoise2Free(ptr);}
int   linenoiseHistoryAdd(const char *line)     {return linenoise2HistoryAdd(0, line);}
int   linenoiseHistorySetMaxLen(int len)        {return linenoise2HistorySetMaxLen(0, len);}
int   linenoiseHistorySave(const char *filename){return linenoise2HistorySave(0, filename);}
int   linenoiseHistoryLoad(const char *filename){return linenoise2HistoryLoad(0, filename);}
void  linenoiseHistoryFree(void)                {return linenoise2HistoryFree(0);}
void  linenoiseClearScreen(void)                {return linenoise2ClearScreen(0);}
void  linenoiseSetMultiLine(int ml)             {return linenoise2SetMultiLine(0, ml);}
void  linenoiseSetDumbMode(int set)             {return linenoise2SetDumbMode(0, set);}
bool  linenoiseIsDumbMode(void)                 {return linenoise2IsDumbMode(0);}
void  linenoisePrintKeyCodes(void)              {return linenoise2PrintKeyCodes(0);}
void  linenoiseAllowEmpty(bool allow)           {return linenoise2AllowEmpty(0, allow);}
int   linenoiseSetMaxLineLen(size_t len)        {return linenoise2SetMaxLineLen(0, len);}


////////////////////////////////////////////////////////////////////
// Linenoise 2
//

linenoiseHandle_t linenoise2Create()
{   
    struct linenoise_t def = LINENOISE_DEFAULT;
    linenoiseHandle_t handle = (linenoiseHandle_t) malloc(sizeof(struct linenoise_t));
    memcpy(handle, &def, sizeof(struct linenoise_t));
    return handle;
}

void  linenoise2Delete(linenoiseHandle_t handle)
{
    free(handle);
}

int linenoise2HistoryAdd(linenoiseHandle_t handle, const char *line);
static void refreshLine(linenoiseHandle_t handle, linenoiseState_t *l);

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

/* ======================= Low level terminal handling ====================== */

/* Set if to use or not the multi line mode. */
void linenoise2SetMultiLine(linenoiseHandle_t handle, int ml)
{
    linenoiseOptions_t* opts = getOptions(handle);
    opts->mlmode = ml;
}

/* Set if terminal does not recognize escape sequences */
void linenoise2SetDumbMode(linenoiseHandle_t handle, int set)
{
    linenoiseOptions_t* opts = getOptions(handle);
    opts->dumbmode = set;
}

/* Returns whether the current mode is dumbmode or not. */
bool linenoise2IsDumbMode(linenoiseHandle_t handle)
{
    linenoiseOptions_t* opts = getOptions(handle);
    return opts->dumbmode;
}

static void flushWrite(linenoiseHandle_t handle)
{
    linenoiseOptions_t* opts = getOptions(handle);

    if (__fbufsize(opts->fpOut) > 0) {
        fflush(opts->fpOut);
    }
    fsync(fileno(opts->fpOut));
}

/* Use the ESC [6n escape sequence to query the horizontal cursor position
 * and return it. On read error -1 is returned, On cursor error, -2 is returned.
 * On success the position of the cursor. */
static int getCursorPosition(linenoiseHandle_t handle)
{
    linenoiseOptions_t* opts = getOptions(handle);

    char buf[LINENOISE_COMMAND_MAX_LEN] = { 0 };
    int cols = 0;
    int rows = 0;
    int i = 0;
    const int out_fd = fileno(opts->fpOut);
    const int in_fd = fileno(opts->fpIn);
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
    while (i < sizeof(buf)-1) {

        int nread = read(in_fd, buf + i, 1);
        if (nread == -1 && errno == EWOULDBLOCK) {return -1;} // read error
        
        /* Keep using unistd's functions. Here, using `read` instead of `fgets`
         * or `fgets` guarantees us that we we can read a byte regardless on
         * whether the sender sent end of line character(s) (CR, CRLF, LF). */
        if (nread != 1 || buf[i] == 'R') {
            /* If we couldn't read a byte from opts->fpIn or if 'R' was received,
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
    if (buf[0] != ESC || buf[1] != '[' || sscanf(buf+2,"%d;%d",&rows,&cols) != 2) {
        // cursor error
        return -2;
    }
    return cols;
}

/* Try to get the number of columns in the current terminal, or assume 80
 * if it fails. */
static int getColumns(linenoiseHandle_t handle)
{
    linenoiseOptions_t* opts = getOptions(handle);

    int start = 0;
    int cols = 0;
    int written = 0;
    char seq[LINENOISE_COMMAND_MAX_LEN] = { 0 };
    const int fd = fileno(opts->fpOut);

    /* The following ANSI escape sequence is used to tell the TTY to move
     * the cursor to the most-right position. */
    const char move_cursor_right[] = "\x1b[999C";
    const size_t cmd_len = sizeof(move_cursor_right);

    /* This one is used to set the cursor position. */
    const char set_cursor_pos[] = "\x1b[%dD";

    /* Get the initial position so we can restore it later. */
    start = getCursorPosition(handle);
    if (start == -1 && errno == EWOULDBLOCK) {return -1;}
    if (start == -2) {goto failed;}

    /* Send the command to go to right margin. Use `write` function instead of
     * `fwrite` for the same reasons explained in `getCursorPosition()` */
    if (write(fd, move_cursor_right, cmd_len) != cmd_len) {
        goto failed;
    }
    flushWrite(handle);

    /* After sending this command, we can get the new position of the cursor,
     * we'd get the size, in columns, of the opened TTY. */
    cols = getCursorPosition(handle);
    if (cols == -1 && errno == EWOULDBLOCK) {return -1;}
    if (cols == -2) {goto failed;}

    /* Restore the position of the cursor back. */
    if (cols > start) {
        /* Generate the move cursor command. */
        written = snprintf(seq, LINENOISE_COMMAND_MAX_LEN, set_cursor_pos, cols-start);

        /* If `written` is equal or bigger than LINENOISE_COMMAND_MAX_LEN, it
         * means that the output has been truncated because the size provided
         * is too small. */
        assert (written < LINENOISE_COMMAND_MAX_LEN);

        /* Send the command with `write`, which is not buffered. */
        if (write(fd, seq, written) == -1) {
            /* Can't recover... */
        }
        flushWrite(handle);
    }
    return cols;

failed:
    return 80;
}

/* Clear the screen. Used to handle ctrl+l */
void linenoise2ClearScreen(linenoiseHandle_t handle)
{
    linenoiseOptions_t* opts = getOptions(handle);
    fprintf(opts->fpOut,"\x1b[H\x1b[2J");
    flushWrite(handle);
}

/* Beep, used for completion when there is nothing to complete or when all
 * the choices were already shown. */
static void linenoise2Beep(linenoiseHandle_t handle)
{
    linenoiseOptions_t* opts = getOptions(handle);
    fprintf(opts->fpOut, "\x7");
    flushWrite(handle);
}

/* ============================== Completion ================================ */

/* Free a list of completion option populated by linenoiseAddCompletion(). */
static void freeCompletions(linenoiseCompletions *lc)
{
    size_t i;
    for (i = 0; i < lc->len; i++)
        free(lc->cvec[i]);
    if (lc->cvec != NULL)
        free(lc->cvec);
}

/* This is an helper function for linenoiseEdit() and is called when the
 * user types the <tab> key in order to complete the string currently in the
 * input.
 *
 * The state of the editing is encapsulated into the pointed linenoiseState
 * structure as described in the structure definition. */
static int completeLine(linenoiseHandle_t handle, linenoiseState_t *ls)
{
    linenoiseOptions_t* opts = getOptions(handle);
    linenoiseCallbacks_t* callbacks = getCallbacks(handle);

    linenoiseCompletions lc = { 0, NULL };
    int nread, nwritten;
    char c = 0;
    int in_fd = fileno(opts->fpIn);

    callbacks->completionCallback(ls->buf,&lc);
    if (lc.len == 0) {
        linenoise2Beep(handle);
    } else {
        size_t stop = 0, i = 0;

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

            nread = read(in_fd, &c, 1);
            if (nread <= 0) {
                freeCompletions(&lc);
                return -1;
            }

            switch(c) {
                case TAB: /* tab */
                    i = (i+1) % (lc.len+1);
                    if (i == lc.len) linenoise2Beep(handle);
                    break;
                case ESC: /* escape */
                    /* Re-show original buffer */
                    if (i < lc.len) refreshLine(handle,ls);
                    stop = 1;
                    break;
                default:
                    /* Update buffer and return */
                    if (i < lc.len) {
                        nwritten = snprintf(ls->buf,ls->buflen,"%s",lc.cvec[i]);
                        ls->len = ls->pos = nwritten;
                    }
                    stop = 1;
                    break;
            }
        }
    }

    freeCompletions(&lc);
    return c; /* Return last read character */
}

/* Register a callback function to be called for tab-completion. */
void linenoise2SetCompletionCallback(linenoiseHandle_t handle, linenoiseCompletionCallback *fn)
{
    linenoiseCallbacks_t* callbacks = getCallbacks(handle);
    callbacks->completionCallback = fn;
}

/* Register a hits function to be called to show hits to the user at the
 * right of the prompt. */
void linenoise2SetHintsCallback(linenoiseHandle_t handle, linenoiseHintsCallback *fn)
{
    linenoiseCallbacks_t* callbacks = getCallbacks(handle);
    callbacks->hintsCallback = fn;
}

/* Register a function to free the hints returned by the hints callback
 * registered with linenoiseSetHintsCallback(). */
void linenoise2SetFreeHintsCallback(linenoiseHandle_t handle, linenoiseFreeHintsCallback *fn)
{
    linenoiseCallbacks_t* callbacks = getCallbacks(handle);
    callbacks->freeHintsCallback = fn;
}

/* This function is used by the callback function registered by the user
 * in order to add completion options given the input string when the
 * user typed <tab>. See the example.c source code for a very easy to
 * understand example. */
void linenoise2AddCompletion(linenoiseCompletions *lc, const char *str) {
    size_t len = strlen(str);
    char *copy, **cvec;

    copy = malloc(len+1);
    if (copy == NULL) return;
    memcpy(copy,str,len+1);
    cvec = realloc(lc->cvec,sizeof(char*)*(lc->len+1));
    if (cvec == NULL) {
        free(copy);
        return;
    }
    lc->cvec = cvec;
    lc->cvec[lc->len++] = copy;
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

static void abInit(struct abuf *ab) {
    ab->b = NULL;
    ab->len = 0;
}

static void abAppend(struct abuf *ab, const char *s, int len) {
    char *new = realloc(ab->b,ab->len+len);

    if (new == NULL) return;
    memcpy(new+ab->len,s,len);
    ab->b = new;
    ab->len += len;
}

static void abFree(struct abuf *ab) {
    free(ab->b);
}

/* Helper of refreshSingleLine() and refreshMultiLine() to show hints
 * to the right of the prompt. */
void refreshShowHints(linenoiseHandle_t handle, struct abuf *ab, linenoiseState_t *l, int plen)
{
    linenoiseCallbacks_t* callbacks = getCallbacks(handle);

    char seq[64];
    if (callbacks->hintsCallback && plen+l->len < l->cols) {
        int color = -1, bold = 0;
        char *hint = callbacks->hintsCallback(l->buf,&color,&bold);
        if (hint) {
            int hintlen = strlen(hint);
            int hintmaxlen = l->cols-(plen+l->len);
            if (hintlen > hintmaxlen) hintlen = hintmaxlen;
            if (bold == 1 && color == -1) color = 37;
            if (color != -1 || bold != 0) {
                snprintf(seq,64,"\033[%d;%d;49m",bold,color);
                abAppend(ab,seq,strlen(seq));
            }
            abAppend(ab,hint,hintlen);
            if (color != -1 || bold != 0)
                abAppend(ab,"\033[0m",4);
            /* Call the function to free the hint returned. */
            if (callbacks->freeHintsCallback) callbacks->freeHintsCallback(hint);
        }
    }
}

/* Single line low level line refresh.
 *
 * Rewrite the currently edited line accordingly to the buffer content,
 * cursor position, and number of columns of the terminal. */
static void refreshSingleLine(linenoiseHandle_t handle, linenoiseState_t *l)
{
    linenoiseOptions_t* opts = getOptions(handle);

    char seq[64];
    size_t plen = l->plen;
    int fd = fileno(opts->fpOut);
    char *buf = l->buf;
    size_t len = l->len;
    size_t pos = l->pos;
    struct abuf ab;

    while((plen+pos) >= l->cols) {
        buf++;
        len--;
        pos--;
    }
    while (plen+len > l->cols) {
        len--;
    }

    abInit(&ab);
    /* Cursor to left edge */
    snprintf(seq,64,"\r");
    abAppend(&ab,seq,strlen(seq));
    /* Write the prompt and the current buffer content */
    abAppend(&ab,l->prompt,strlen(l->prompt));
    abAppend(&ab,buf,len);
    /* Show hits if any. */
    refreshShowHints(handle,&ab,l,plen);
    /* Erase to right */
    snprintf(seq,64,"\x1b[0K");
    abAppend(&ab,seq,strlen(seq));
    /* Move cursor to original position. */
    snprintf(seq,64,"\r\x1b[%dC", (int)(pos+plen));
    abAppend(&ab,seq,strlen(seq));
    if (write(fd, ab.b, ab.len) == -1) {} /* Can't recover from write error. */
    flushWrite(handle);
    abFree(&ab);
}

/* Multi line low level line refresh.
 *
 * Rewrite the currently edited line accordingly to the buffer content,
 * cursor position, and number of columns of the terminal. */
static void refreshMultiLine(linenoiseHandle_t handle, linenoiseState_t *l)
{
    linenoiseOptions_t* opts = getOptions(handle);

    char seq[64];
    int plen = l->plen;
    int rows = (plen+l->len+l->cols-1)/l->cols; /* rows used by current buf. */
    int rpos = (plen+l->oldpos+l->cols)/l->cols; /* cursor relative row. */
    int rpos2; /* rpos after refresh. */
    int col; /* colum position, zero-based. */
    int old_rows = l->maxrows;
    int j;
    int fd = fileno(opts->fpOut);
    struct abuf ab;

    /* Update maxrows if needed. */
    if (rows > (int)l->maxrows) l->maxrows = rows;

    /* First step: clear all the lines used before. To do so start by
     * going to the last row. */
    abInit(&ab);
    if (old_rows-rpos > 0) {
        lndebug("go down %d", old_rows-rpos);
        snprintf(seq,64,"\x1b[%dB", old_rows-rpos);
        abAppend(&ab,seq,strlen(seq));
    }

    /* Now for every row clear it, go up. */
    for (j = 0; j < old_rows-1; j++) {
        lndebug("clear+up");
        snprintf(seq,64,"\r\x1b[0K\x1b[1A");
        abAppend(&ab,seq,strlen(seq));
    }

    /* Clean the top line. */
    lndebug("clear");
    snprintf(seq,64,"\r\x1b[0K");
    abAppend(&ab,seq,strlen(seq));

    /* Write the prompt and the current buffer content */
    abAppend(&ab,l->prompt,strlen(l->prompt));
    abAppend(&ab,l->buf,l->len);

    /* Show hits if any. */
    refreshShowHints(handle,&ab,l,plen);

    /* If we are at the very end of the screen with our prompt, we need to
     * emit a newline and move the prompt to the first column. */
    if (l->pos &&
        l->pos == l->len &&
        (l->pos+plen) % l->cols == 0)
    {
        lndebug("<newline>");
        abAppend(&ab,"\n",1);
        snprintf(seq,64,"\r");
        abAppend(&ab,seq,strlen(seq));
        rows++;
        if (rows > (int)l->maxrows) l->maxrows = rows;
    }

    /* Move cursor to right position. */
    rpos2 = (plen+l->pos+l->cols)/l->cols; /* current cursor relative row. */
    lndebug("rpos2 %d", rpos2);

    /* Go up till we reach the expected positon. */
    if (rows-rpos2 > 0) {
        lndebug("go-up %d", rows-rpos2);
        snprintf(seq,64,"\x1b[%dA", rows-rpos2);
        abAppend(&ab,seq,strlen(seq));
    }

    /* Set column. */
    col = (plen+(int)l->pos) % (int)l->cols;
    lndebug("set col %d", 1+col);
    if (col)
        snprintf(seq,64,"\r\x1b[%dC", col);
    else
        snprintf(seq,64,"\r");
    abAppend(&ab,seq,strlen(seq));

    lndebug("\n");
    l->oldpos = l->pos;

    if (write(fd,ab.b,ab.len) == -1) {} /* Can't recover from write error. */
    flushWrite(handle);
    abFree(&ab);
}

/* Calls the two low level functions refreshSingleLine() or
 * refreshMultiLine() according to the selected mode. */
static void refreshLine(linenoiseHandle_t handle, linenoiseState_t *l)
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
int linenoise2EditInsert(linenoiseHandle_t handle, linenoiseState_t *l, char c)
{
    linenoiseOptions_t* opts = getOptions(handle);
    linenoiseCallbacks_t* callbacks = getCallbacks(handle);

    int fd = fileno(opts->fpOut);
    if (l->len < l->buflen) {
        if (l->len == l->pos) {
            l->buf[l->pos] = c;
            l->pos++;
            l->len++;
            l->buf[l->len] = '\0';
            if ((!opts->mlmode && l->plen+l->len < l->cols && !callbacks->hintsCallback)) {
                /* Avoid a full update of the line in the
                 * trivial case. */
                if (write(fd, &c,1) == -1) {
                    return -1;
                }
                flushWrite(handle);
            } else {
                refreshLine(handle,l);
            }
        } else {
            memmove(l->buf+l->pos+1,l->buf+l->pos,l->len-l->pos);
            l->buf[l->pos] = c;
            l->len++;
            l->pos++;
            l->buf[l->len] = '\0';
            refreshLine(handle,l);
        }
    }
    return 0;
}

int linenoise2InsertPastedChar(linenoiseHandle_t handle, linenoiseState_t *l, char c)
{
    linenoiseOptions_t* opts = getOptions(handle);

    int fd = fileno(opts->fpOut);
    if (l->len < l->buflen && l->len == l->pos) {
        l->buf[l->pos] = c;
        l->pos++;
        l->len++;
        l->buf[l->len] = '\0';
        if (write(fd, &c,1) == -1) {
            return -1;
        }
        flushWrite(handle);
    }
    return 0;
}

/* Move cursor on the left. */
void linenoise2EditMoveLeft(linenoiseHandle_t handle, linenoiseState_t *l)
{
    if (l->pos > 0) {
        l->pos--;
        refreshLine(handle,l);
    }
}

/* Move cursor on the right. */
void linenoise2EditMoveRight(linenoiseHandle_t handle, linenoiseState_t *l)
{
    if (l->pos != l->len) {
        l->pos++;
        refreshLine(handle,l);
    }
}

/* Move cursor to the start of the line. */
void linenoise2EditMoveHome(linenoiseHandle_t handle, linenoiseState_t *l)
{
    if (l->pos != 0) {
        l->pos = 0;
        refreshLine(handle,l);
    }
}

/* Move cursor to the end of the line. */
void linenoise2EditMoveEnd(linenoiseHandle_t handle, linenoiseState_t *l)
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
void linenoise2EditHistoryNext(linenoiseHandle_t handle, linenoiseState_t *l, int dir)
{
    linenoiseOptions_t* opts = getOptions(handle);

    if (opts->history_len > 1) {
        /* Update the current history entry before to
         * overwrite it with the next one. */
        free(opts->history[opts->history_len - 1 - l->history_index]);
        opts->history[opts->history_len - 1 - l->history_index] = strdup(l->buf);

        /* Show the new entry */
        l->history_index += (dir == LINENOISE_HISTORY_PREV) ? 1 : -1;
        if (l->history_index < 0) {
            l->history_index = 0;
            return;
        } else if (l->history_index >= opts->history_len) {
            l->history_index = opts->history_len-1;
            return;
        }
        strncpy(l->buf,opts->history[opts->history_len - 1 - l->history_index],l->buflen);
        l->buf[l->buflen-1] = '\0';
        l->len = l->pos = strlen(l->buf);
        refreshLine(handle,l);
    }
}

/* Delete the character at the right of the cursor without altering the cursor
 * position. Basically this is what happens with the "Delete" keyboard key. */
void linenoise2EditDelete(linenoiseHandle_t handle, linenoiseState_t *l)
{
    if (l->len > 0 && l->pos < l->len) {
        memmove(l->buf+l->pos,l->buf+l->pos+1,l->len-l->pos-1);
        l->len--;
        l->buf[l->len] = '\0';
        refreshLine(handle,l);
    }
}

/* Backspace implementation. */
void linenoiseEditBackspace(linenoiseHandle_t handle, linenoiseState_t *l)
{
    if (l->pos > 0 && l->len > 0) {
        memmove(l->buf+l->pos-1,l->buf+l->pos,l->len-l->pos);
        l->pos--;
        l->len--;
        l->buf[l->len] = '\0';
        refreshLine(handle,l);
    }
}

/* Delete the previosu word, maintaining the cursor at the start of the
 * current word. */
void linenoise2EditDeletePrevWord(linenoiseHandle_t handle, linenoiseState_t *l)
{
    size_t old_pos = l->pos;
    size_t diff;

    while (l->pos > 0 && l->buf[l->pos-1] == ' ')
        l->pos--;
    while (l->pos > 0 && l->buf[l->pos-1] != ' ')
        l->pos--;
    diff = old_pos - l->pos;
    memmove(l->buf+l->pos,l->buf+old_pos,l->len-old_pos+1);
    l->len -= diff;
    refreshLine(handle,l);
}

uint32_t getMillis(void)
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
static int linenoise2Edit(linenoiseHandle_t handle, char *buf, size_t buflen, const char *prompt)
{
    linenoiseOptions_t* opts = getOptions(handle);
    linenoiseCallbacks_t* callbacks = getCallbacks(handle);

    uint32_t t1 = 0;
    linenoiseState_t l;
    int out_fd = fileno(opts->fpOut);
    int in_fd = fileno(opts->fpIn);

    int cols = getColumns(handle);
    if (cols == -1 && errno == EWOULDBLOCK) {return -1;}

    /* Populate the linenoise state that we pass to functions implementing
     * specific editing functionalities. */
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
    linenoise2HistoryAdd(handle,"");

    int pos1 = getCursorPosition(handle);
    if (pos1 == -1 && errno == EWOULDBLOCK){return -1;}

    if (write(out_fd, prompt,l.plen) == -1) {
        return -1;
    }
    flushWrite(handle);
    
    int pos2 = getCursorPosition(handle);
    if (pos2 == -1 && errno == EWOULDBLOCK){return -1;}

    if (pos1 >= 0 && pos2 >= 0) {
        l.plen = pos2 - pos1;
    }
    while(1) {
        char c;
        int nread;
        char seq[3];

        /**
         * To determine whether the user is pasting data or typing itself, we
         * need to calculate how many milliseconds elapsed between two key
         * presses. Indeed, if there is less than LINENOISE_PASTE_KEY_DELAY
         * (typically 30-40ms), then a paste is being performed, else, the
         * user is typing.
         * NOTE: pressing a key down without releasing it will also spend
         * about 40ms (or even more)
         */
        t1 = getMillis();
        nread = read(in_fd, &c, 1);
        if (nread == -1 && errno == EWOULDBLOCK) {return -1;}
        if (nread <= 0){return l.len;}

        if ( (getMillis() - t1) < LINENOISE_PASTE_KEY_DELAY && c != ENTER) {
            /* Pasting data, insert characters without formatting.
             * This can only be performed when the cursor is at the end of the
             * line. */
            if (linenoise2InsertPastedChar(handle,&l,c)) {
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
            if (c2 < 0) return l.len;
            /* Read next character when 0 */
            if (c2 == 0) continue;
            c = c2;
        }

        switch(c) {
        case ENTER:    /* enter */
            opts->history_len--;
            free(opts->history[opts->history_len]);
            if (opts->mlmode) linenoise2EditMoveEnd(handle, &l);
            if (callbacks->hintsCallback) {
                /* Force a refresh without hints to leave the previous
                 * line as the user typed it after a newline. */
                linenoiseHintsCallback *hc = callbacks->hintsCallback;
                callbacks->hintsCallback = NULL;
                refreshLine(handle,&l);
                callbacks->hintsCallback = hc;
            }
            return (int)l.len;
        case CTRL_C:     /* ctrl-c */
            errno = EAGAIN;
            return -1;
        case BACKSPACE:   /* backspace */
        case 8:     /* ctrl-h */
            linenoiseEditBackspace(handle, &l);
            break;
        case CTRL_D:     /* ctrl-d, remove char at right of cursor, or if the
                            line is empty, act as end-of-file. */
            if (l.len > 0) {
                linenoise2EditDelete(handle, &l);
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
                if (l.pos != l.len-1) l.pos++;
                refreshLine(handle,&l);
            }
            break;
        case CTRL_B:     /* ctrl-b */
            linenoise2EditMoveLeft(handle, &l);
            break;
        case CTRL_F:     /* ctrl-f */
            linenoise2EditMoveRight(handle, &l);
            break;
        case CTRL_P:    /* ctrl-p */
            linenoise2EditHistoryNext(handle, &l, LINENOISE_HISTORY_PREV);
            break;
        case CTRL_N:    /* ctrl-n */
            linenoise2EditHistoryNext(handle, &l, LINENOISE_HISTORY_NEXT);
            break;
        case ESC:{    /* escape sequence */
            /* Read the next two bytes representing the escape sequence. */
            int n = read(in_fd, seq, 2);
            if (n == -1 && errno == EWOULDBLOCK) {return -1;}
            if (n < 2) {break;}

            /* ESC [ sequences. */
            if (seq[0] == '[') {
                if (seq[1] >= '0' && seq[1] <= '9') {
                    /* Extended escape, read additional byte. */
                    int n = read(in_fd, seq+2, 1);
                    if (n == -1 && errno == EWOULDBLOCK) {return -1;}
                    if (n == -1) {break;}
                    if (seq[2] == '~') {
                        switch(seq[1]) {
                        case '3': /* Delete key. */
                            linenoise2EditDelete(handle, &l);
                            break;
                        }
                    }
                } else {
                    switch(seq[1]) {
                    case 'A': /* Up */
                        linenoise2EditHistoryNext(handle, &l, LINENOISE_HISTORY_PREV);
                        break;
                    case 'B': /* Down */
                        linenoise2EditHistoryNext(handle, &l, LINENOISE_HISTORY_NEXT);
                        break;
                    case 'C': /* Right */
                        linenoise2EditMoveRight(handle, &l);
                        break;
                    case 'D': /* Left */
                        linenoise2EditMoveLeft(handle, &l);
                        break;
                    case 'H': /* Home */
                        linenoise2EditMoveHome(handle, &l);
                        break;
                    case 'F': /* End*/
                        linenoise2EditMoveEnd(handle, &l);
                        break;
                    }
                }
            }

            /* ESC O sequences. */
            else if (seq[0] == 'O') {
                switch(seq[1]) {
                case 'H': /* Home */
                    linenoise2EditMoveHome(handle, &l);
                    break;
                case 'F': /* End*/
                    linenoise2EditMoveEnd(handle, &l);
                    break;
                }
            }
            break;
        }
        default:
            if (linenoise2EditInsert(handle,&l,c)) return -1;
            break;
        case CTRL_U: /* Ctrl+u, delete the whole line. */
            buf[0] = '\0';
            l.pos = l.len = 0;
            refreshLine(handle,&l);
            break;
        case CTRL_K: /* Ctrl+k, delete from current to end of line. */
            buf[l.pos] = '\0';
            l.len = l.pos;
            refreshLine(handle,&l);
            break;
        case CTRL_A: /* Ctrl+a, go to the start of the line */
            linenoise2EditMoveHome(handle, &l);
            break;
        case CTRL_E: /* ctrl+e, go to the end of the line */
            linenoise2EditMoveEnd(handle, &l);
            break;
        case CTRL_L: /* ctrl+l, clear screen */
            linenoise2ClearScreen(handle);
            refreshLine(handle, &l);
            break;
        case CTRL_W: /* ctrl+w, delete previous word */
            linenoise2EditDeletePrevWord(handle, &l);
            break;
        }
        flushWrite(handle);
    }
    return l.len;
}

void linenoise2AllowEmpty(linenoiseHandle_t handle, bool val)
{
    linenoiseOptions_t* opts = getOptions(handle);
    opts->allow_empty = val;
}

int linenoise2Probe(linenoiseHandle_t handle)
{
    linenoiseOptions_t* opts = getOptions(handle);

    /* Switch to non-blocking mode */
    int fpIn_fileno = fileno(opts->fpIn);
    int flags = fcntl(fpIn_fileno, F_GETFL);
    flags |= O_NONBLOCK;
    int res = fcntl(fpIn_fileno, F_SETFL, flags);
    if (res != 0) {
        return -1;
    }
    /* Device status request */
    fprintf(opts->fpOut, "\x1b[5n");
    flushWrite(handle);

    /* Try to read response */
    int timeout_ms = 200;
    const int retry_ms = 10;
    size_t read_bytes = 0;
    while (timeout_ms > 0 && read_bytes < 4) { // response is ESC[0n or ESC[3n
        usleep(retry_ms * 1000);
        timeout_ms -= retry_ms;
        char c;
        int cb = read(fpIn_fileno, &c, 1);
        // Note! Due to the fcntl call above, this is read in non-blocking mode!
        // So, nread == -1 && errno == EWOULDBLOCK are expected here!
        if (cb < 0) {
            continue;
        }
        if (read_bytes == 0 && c != '\x1b') {
            /* invalid response */
            break;
        }
        read_bytes += cb;
    }
    /* Restore old mode */
    flags &= ~O_NONBLOCK;
    res = fcntl(fpIn_fileno, F_SETFL, flags);
    if (res != 0) {
        return -1;
    }
    if (read_bytes < 4) {
        return -2;
    }
    return 0;
}

static int linenoise2Raw(linenoiseHandle_t handle, char *buf, size_t buflen, const char *prompt)
{
    int count;

    linenoiseOptions_t* opts = getOptions(handle);

    if (buflen == 0) {
        errno = EINVAL;
        return -1;
    }

    count = linenoise2Edit(handle, buf, buflen, prompt);
    if (count != -1){
        fputc('\n', opts->fpOut);
        flushWrite(handle);
    }
    return count;
}

static int linenoise2Dumb(linenoiseHandle_t handle, char* buf, size_t buflen, const char* prompt)
{
    linenoiseOptions_t* opts = getOptions(handle);

    /* dumb terminal, fall back to fgets */
    fputs(prompt, opts->fpOut);
    size_t count = 0;
    while (count < buflen) {
        int c = fgetc(opts->fpIn);
        if (c == -1 && errno == EWOULDBLOCK) {return -1;}

        if (c == '\n') {
            break;
        } else if (c >= 0x1c && c <= 0x1f){
            continue; /* consume arrow keys */
        } else if (c == BACKSPACE || c == 0x8) {
            if (count > 0) {
                buf[count - 1] = 0;
                count --;
            }
            fputs("\x08 ", opts->fpOut); /* Windows CMD: erase symbol under cursor */
        } else {
            buf[count] = c;
            ++count;
        }
        fputc(c, opts->fpOut); /* echo */
    }
    fputc('\n', opts->fpOut);
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

/* The high level function that is the main API of the linenoise library. */
char *linenoise2(linenoiseHandle_t handle, const char *prompt)
{
    linenoiseOptions_t* opts = getOptions(handle);

    char *buf = calloc(1, opts->max_cmdline_length);
    int count = 0;
    if (buf == NULL) {
        return NULL;
    }
    if (!opts->dumbmode) {
        count = linenoise2Raw(handle, buf, opts->max_cmdline_length, prompt);
    } else {
        count = linenoise2Dumb(handle, buf, opts->max_cmdline_length, prompt);
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

void  linenoise2SetFpIn(linenoiseHandle_t handle, FILE* fpIn)
{
    linenoiseOptions_t* opts = getOptions(handle);
    opts->fpIn = fpIn;
}
void  linenoise2SetFpOut(linenoiseHandle_t handle, FILE* fpOut)
{
    linenoiseOptions_t* opts = getOptions(handle);
    opts->fpOut = fpOut;
}

/* This is just a wrapper the user may want to call in order to make sure
 * the linenoise returned buffer is freed with the same allocator it was
 * created with. Useful when the main program is using an alternative
 * allocator. */
void linenoise2Free(void* ptr) {
    free(ptr);
}

/* ================================ History ================================= */

void linenoise2HistoryFree(linenoiseHandle_t handle)
{
    linenoiseOptions_t* opts = getOptions(handle);

    if (opts->history) {
        for (int j = 0; j < opts->history_len; j++) {
            free(opts->history[j]);
        }
        free(opts->history);
    }
    opts->history = NULL;
}

/* This is the API call to add a new entry in the linenoise history.
 * It uses a fixed array of char pointers that are shifted (memmoved)
 * when the history max length is reached in order to remove the older
 * entry and make room for the new one, so it is not exactly suitable for huge
 * histories, but will work well for a few hundred of entries.
 *
 * Using a circular buffer is smarter, but a bit more complex to handle. */
int linenoise2HistoryAdd(linenoiseHandle_t handle, const char *line)
{
    char *linecopy;

    linenoiseOptions_t* opts = getOptions(handle);

    if (opts->history_max_len == 0) return 0;

    /* Initialization on first call. */
    if (opts->history == NULL) {
        opts->history = malloc(sizeof(char*) * opts->history_max_len);
        if (opts->history == NULL) return 0;
        memset(opts->history,0,(sizeof(char*) * opts->history_max_len));
    }

    /* Don't add duplicated lines. */
    if (opts->history_len && !strcmp(opts->history[opts->history_len-1], line)) return 0;

    /* Add an heap allocated copy of the line in the history.
     * If we reached the max length, remove the older line. */
    linecopy = strdup(line);
    if (!linecopy) return 0;
    if (opts->history_len == opts->history_max_len) {
        free(opts->history[0]);
        memmove(opts->history,opts->history+1,sizeof(char*) * (opts->history_max_len-1));
        opts->history_len--;
    }
    opts->history[opts->history_len] = linecopy;
    opts->history_len++;

    return 1;
}

/* Set the maximum length for the history. This function can be called even
 * if there is already some history, the function will make sure to retain
 * just the latest 'len' elements if the new history length value is smaller
 * than the amount of items already inside the history. */
int linenoise2HistorySetMaxLen(linenoiseHandle_t handle, int len)
{
    char **new;

    linenoiseOptions_t* opts = getOptions(handle);

    if (len < 1) return 0;
    if (opts->history) {
        int tocopy = opts->history_len;

        new = malloc(sizeof(char*)*len);
        if (new == NULL) return 0;

        /* If we can't copy everything, free the elements we'll not use. */
        if (len < tocopy) {
            int j;

            for (j = 0; j < tocopy-len; j++) free(opts->history[j]);
            tocopy = len;
        }
        memset(new,0,sizeof(char*)*len);
        memcpy(new,opts->history + (opts->history_len - tocopy), sizeof(char*)*tocopy);
        free(opts->history);
        opts->history = new;
    }
    opts->history_max_len = len;
    if (opts->history_len > opts->history_max_len)
        opts->history_len = opts->history_max_len;
    return 1;
}

/* Save the history in the specified file. On success 0 is returned
 * otherwise -1 is returned. */
int linenoise2HistorySave(linenoiseHandle_t handle, const char *filename)
{
    FILE *fp;
    int j;

    linenoiseOptions_t* opts = getOptions(handle);

    fp = fopen(filename,"w");
    if (fp == NULL) return -1;
    for (j = 0; j < opts->history_len; j++)
        fprintf(fp,"%s\n",opts->history[j]);
    fclose(fp);
    return 0;
}

/* Load the history from the specified file. If the file does not exist
 * zero is returned and no operation is performed.
 *
 * If the file exists and the operation succeeded 0 is returned, otherwise
 * on error -1 is returned. */
int linenoise2HistoryLoad(linenoiseHandle_t handle, const char *filename)
{
    linenoiseOptions_t* opts = getOptions(handle);

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return -1;
    }

    char *buf = calloc(1, opts->max_cmdline_length);
    if (buf == NULL) {
        fclose(fp);
        return -1;
    }

    while (fgets(buf, opts->max_cmdline_length, fp) != NULL) {
        char *p;

        p = strchr(buf,'\r');
        if (!p) p = strchr(buf,'\n');
        if (p) *p = '\0';
        linenoise2HistoryAdd(handle, buf);
    }

    free(buf);
    fclose(fp);

    return 0;
}

/* Set line maximum length. If len parameter is smaller than
 * LINENOISE_MINIMAL_MAX_LINE, -1 is returned
 * otherwise 0 is returned. */
int linenoise2SetMaxLineLen(linenoiseHandle_t handle, size_t len)
{
    linenoiseOptions_t* opts = getOptions(handle);
    if (len < LINENOISE_MINIMAL_MAX_LINE) {
        return -1;
    }
    opts->max_cmdline_length = len;
    return 0;
}
