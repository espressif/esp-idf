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
 * When linenoiseClearScreen() is called, two additional escape sequences
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
#include <stdint.h>
#include <stdio.h>
#include "sdkconfig.h"
#if !CONFIG_IDF_TARGET_LINUX
// On Linux, we don't need __fbufsize (see comments below), and
// __fbufsize not available on MacOS (which is also considered "Linux" target)
#include <stdio_ext.h> // for __fbufsize
#endif
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>
#include "linenoise.h"

#define LINENOISE_DEFAULT_HISTORY_MAX_LEN 100
#define LINENOISE_DEFAULT_MAX_LINE 4096
#define LINENOISE_MINIMAL_MAX_LINE 64
#define LINENOISE_COMMAND_MAX_LEN 32
#define LINENOISE_PASTE_KEY_DELAY 30 /* Delay, in milliseconds, between two characters being pasted from clipboard */

static linenoiseCompletionCallback *completionCallback = NULL;
static linenoiseHintsCallback *hintsCallback = NULL;
static linenoiseFreeHintsCallback *freeHintsCallback = NULL;

static size_t max_cmdline_length = LINENOISE_DEFAULT_MAX_LINE;
static int mlmode = 0;  /* Multi line mode. Default is single line. */
static int dumbmode = 0; /* Dumb mode where line editing is disabled. Off by default */
static int history_max_len = LINENOISE_DEFAULT_HISTORY_MAX_LEN;
static int history_len = 0;
static char **history = NULL;
static bool allow_empty = true;

/* The linenoiseState structure represents the state during line editing.
 * We pass this state to functions implementing specific editing
 * functionalities. */
struct linenoiseState {
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
};

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
	UNIT_SEP = 31,      /* ctrl-_ */
	BACKSPACE =  127    /* Backspace */
};

int linenoiseHistoryAdd(const char *line);
static void refreshLine(struct linenoiseState *l);

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
void linenoiseSetMultiLine(int ml) {
    mlmode = ml;
}

/* Set if terminal does not recognize escape sequences */
void linenoiseSetDumbMode(int set) {
    dumbmode = set;
}

/* Returns whether the current mode is dumbmode or not. */
bool linenoiseIsDumbMode(void) {
    return dumbmode;
}

static void flushWrite(void) {
// On Linux, we set stdout to unbuffered mode to facilitate interaction with tools.
// Performance on Linux is not considered as critical as on chip targets. Additionally,
// MacOS does not have __fbufsize.
#if !CONFIG_IDF_TARGET_LINUX
    if (__fbufsize(stdout) > 0) {
        fflush(stdout);
    }
#endif
    fsync(fileno(stdout));
}

/* Use the ESC [6n escape sequence to query the horizontal cursor position
 * and return it. On error -1 is returned, on success the position of the
 * cursor. */
static int getCursorPosition(void) {
    char buf[LINENOISE_COMMAND_MAX_LEN] = { 0 };
    int cols = 0;
    int rows = 0;
    int i = 0;
    const int out_fd = fileno(stdout);
    const int in_fd = fileno(stdin);
    /* The following ANSI escape sequence is used to get from the TTY the
     * cursor position. */
    const char get_cursor_cmd[] = "\x1b[6n";

    /* Send the command to the TTY on the other end of the UART.
     * Let's use unistd's write function. Thus, data sent through it are raw
     * reducing the overhead compared to using fputs, fprintf, etc... */
    int num_written = write(out_fd, get_cursor_cmd, sizeof(get_cursor_cmd));
    if (num_written != sizeof(get_cursor_cmd)) {
        return -1;
    }

    /* For USB CDC, it is required to flush the output. */
    flushWrite();

    /* The other end will send its response which format is ESC [ rows ; cols R
     * We don't know exactly how many bytes we have to read, thus, perform a
     * read for each byte.
     * Stop right before the last character of the buffer, to be able to NULL
     * terminate it. */
    while (i < sizeof(buf)-1) {
        /* Keep using unistd's functions. Here, using `read` instead of `fgets`
         * or `fgets` guarantees us that we we can read a byte regardless on
         * whether the sender sent end of line character(s) (CR, CRLF, LF). */
        if (read(in_fd, buf + i, 1) != 1 || buf[i] == 'R') {
            /* If we couldn't read a byte from STDIN or if 'R' was received,
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
        return -1;
    }
    return cols;
}

/* Try to get the number of columns in the current terminal, or assume 80
 * if it fails. */
static int getColumns(void) {
    int start = 0;
    int cols = 0;
    int written = 0;
    char seq[LINENOISE_COMMAND_MAX_LEN] = { 0 };
    const int fd = fileno(stdout);

    /* The following ANSI escape sequence is used to tell the TTY to move
     * the cursor to the most-right position. */
    const char move_cursor_right[] = "\x1b[999C";
    const size_t cmd_len = sizeof(move_cursor_right);

    /* This one is used to set the cursor position. */
    const char set_cursor_pos[] = "\x1b[%dD";

    /* Get the initial position so we can restore it later. */
    start = getCursorPosition();
    if (start == -1) {
        goto failed;
    }

    /* Send the command to go to right margin. Use `write` function instead of
     * `fwrite` for the same reasons explained in `getCursorPosition()` */
    if (write(fd, move_cursor_right, cmd_len) != cmd_len) {
        goto failed;
    }
    flushWrite();

    /* After sending this command, we can get the new position of the cursor,
     * we'd get the size, in columns, of the opened TTY. */
    cols = getCursorPosition();
    if (cols == -1) {
        goto failed;
    }

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
        flushWrite();
    }
    return cols;

failed:
    return 80;
}

/* Clear the screen. Used to handle ctrl+l */
void linenoiseClearScreen(void) {
    fprintf(stdout,"\x1b[H\x1b[2J");
    flushWrite();
}

/* Beep, used for completion when there is nothing to complete or when all
 * the choices were already shown. */
static void linenoiseBeep(void) {
    fprintf(stdout, "\x7");
    flushWrite();
}

/* ============================== Completion ================================ */

/* Free a list of completion option populated by linenoiseAddCompletion(). */
static void freeCompletions(linenoiseCompletions *lc) {
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
static int completeLine(struct linenoiseState *ls) {
    linenoiseCompletions lc = { 0, NULL };
    int nread, nwritten;
    char c = 0;
    int in_fd = fileno(stdin);

    completionCallback(ls->buf,&lc);
    if (lc.len == 0) {
        linenoiseBeep();
    } else {
        size_t stop = 0, i = 0;

        while(!stop) {
            /* Show completion or original buffer */
            if (i < lc.len) {
                struct linenoiseState saved = *ls;

                ls->len = ls->pos = strlen(lc.cvec[i]);
                ls->buf = lc.cvec[i];
                refreshLine(ls);
                ls->len = saved.len;
                ls->pos = saved.pos;
                ls->buf = saved.buf;
            } else {
                refreshLine(ls);
            }

            nread = read(in_fd, &c, 1);
            if (nread <= 0) {
                freeCompletions(&lc);
                return -1;
            }

            switch(c) {
                case TAB: /* tab */
                    i = (i+1) % (lc.len+1);
                    if (i == lc.len) linenoiseBeep();
                    break;
                case ESC: /* escape */
                    /* Re-show original buffer */
                    if (i < lc.len) refreshLine(ls);
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
void linenoiseSetCompletionCallback(linenoiseCompletionCallback *fn) {
    completionCallback = fn;
}

/* Register a hits function to be called to show hits to the user at the
 * right of the prompt. */
void linenoiseSetHintsCallback(linenoiseHintsCallback *fn) {
    hintsCallback = fn;
}

/* Register a function to free the hints returned by the hints callback
 * registered with linenoiseSetHintsCallback(). */
void linenoiseSetFreeHintsCallback(linenoiseFreeHintsCallback *fn) {
    freeHintsCallback = fn;
}

/* This function is used by the callback function registered by the user
 * in order to add completion options given the input string when the
 * user typed <tab>. See the example.c source code for a very easy to
 * understand example. */
void linenoiseAddCompletion(linenoiseCompletions *lc, const char *str) {
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
void refreshShowHints(struct abuf *ab, struct linenoiseState *l, int plen) {
    char seq[64];
    if (hintsCallback && plen+l->len < l->cols) {
        int color = -1, bold = 0;
        char *hint = hintsCallback(l->buf,&color,&bold);
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
            if (freeHintsCallback) freeHintsCallback(hint);
        }
    }
}

/* Single line low level line refresh.
 *
 * Rewrite the currently edited line accordingly to the buffer content,
 * cursor position, and number of columns of the terminal. */
static void refreshSingleLine(struct linenoiseState *l) {
    char seq[64];
    size_t plen = l->plen;
    int fd = fileno(stdout);
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
    refreshShowHints(&ab,l,plen);
    /* Erase to right */
    snprintf(seq,64,"\x1b[0K");
    abAppend(&ab,seq,strlen(seq));
    /* Move cursor to original position. */
    snprintf(seq,64,"\r\x1b[%dC", (int)(pos+plen));
    abAppend(&ab,seq,strlen(seq));
    if (write(fd, ab.b, ab.len) == -1) {} /* Can't recover from write error. */
    flushWrite();
    abFree(&ab);
}

/* Multi line low level line refresh.
 *
 * Rewrite the currently edited line accordingly to the buffer content,
 * cursor position, and number of columns of the terminal. */
static void refreshMultiLine(struct linenoiseState *l) {
    char seq[64];
    int plen = l->plen;
    int rows = (plen+l->len+l->cols-1)/l->cols; /* rows used by current buf. */
    int rpos = (plen+l->oldpos+l->cols)/l->cols; /* cursor relative row. */
    int rpos2; /* rpos after refresh. */
    int col; /* column position, zero-based. */
    int old_rows = l->maxrows;
    int j;
    int fd = fileno(stdout);
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
    refreshShowHints(&ab,l,plen);

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

    /* Go up till we reach the expected position. */
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
    flushWrite();
    abFree(&ab);
}

/* Calls the two low level functions refreshSingleLine() or
 * refreshMultiLine() according to the selected mode. */
static void refreshLine(struct linenoiseState *l) {
    if (mlmode)
        refreshMultiLine(l);
    else
        refreshSingleLine(l);
}

/* Insert the character 'c' at cursor current position.
 *
 * On error writing to the terminal -1 is returned, otherwise 0. */
int linenoiseEditInsert(struct linenoiseState *l, char c) {
    int fd = fileno(stdout);
    if (l->len < l->buflen) {
        if (l->len == l->pos) {
            l->buf[l->pos] = c;
            l->pos++;
            l->len++;
            l->buf[l->len] = '\0';
            if ((!mlmode && l->plen+l->len < l->cols && !hintsCallback)) {
                /* Avoid a full update of the line in the
                 * trivial case. */
                if (write(fd, &c,1) == -1) {
                    return -1;
                }
                flushWrite();
            } else {
                refreshLine(l);
            }
        } else {
            memmove(l->buf+l->pos+1,l->buf+l->pos,l->len-l->pos);
            l->buf[l->pos] = c;
            l->len++;
            l->pos++;
            l->buf[l->len] = '\0';
            refreshLine(l);
        }
    }
    return 0;
}

int linenoiseInsertPastedChar(struct linenoiseState *l, char c) {
    int fd = fileno(stdout);
    if (l->len < l->buflen && l->len == l->pos) {
        l->buf[l->pos] = c;
        l->pos++;
        l->len++;
        l->buf[l->len] = '\0';
        if (write(fd, &c,1) == -1) {
            return -1;
        }
        flushWrite();
    }
    return 0;
}

/* Move cursor on the left. */
void linenoiseEditMoveLeft(struct linenoiseState *l) {
    if (l->pos > 0) {
        l->pos--;
        refreshLine(l);
    }
}

/* Move cursor on the right. */
void linenoiseEditMoveRight(struct linenoiseState *l) {
    if (l->pos != l->len) {
        l->pos++;
        refreshLine(l);
    }
}

/* Move cursor to the start of the line. */
void linenoiseEditMoveHome(struct linenoiseState *l) {
    if (l->pos != 0) {
        l->pos = 0;
        refreshLine(l);
    }
}

/* Move cursor to the end of the line. */
void linenoiseEditMoveEnd(struct linenoiseState *l) {
    if (l->pos != l->len) {
        l->pos = l->len;
        refreshLine(l);
    }
}

/* Substitute the currently edited line with the next or previous history
 * entry as specified by 'dir'. */
#define LINENOISE_HISTORY_NEXT 0
#define LINENOISE_HISTORY_PREV 1
void linenoiseEditHistoryNext(struct linenoiseState *l, int dir) {
    if (history_len > 1) {
        /* Update the current history entry before to
         * overwrite it with the next one. */
        free(history[history_len - 1 - l->history_index]);
        history[history_len - 1 - l->history_index] = strdup(l->buf);
        /* Show the new entry */
        l->history_index += (dir == LINENOISE_HISTORY_PREV) ? 1 : -1;
        if (l->history_index < 0) {
            l->history_index = 0;
            return;
        } else if (l->history_index >= history_len) {
            l->history_index = history_len-1;
            return;
        }
        strncpy(l->buf,history[history_len - 1 - l->history_index],l->buflen);
        l->buf[l->buflen-1] = '\0';
        l->len = l->pos = strlen(l->buf);
        refreshLine(l);
    }
}

/* Delete the character at the right of the cursor without altering the cursor
 * position. Basically this is what happens with the "Delete" keyboard key. */
void linenoiseEditDelete(struct linenoiseState *l) {
    if (l->len > 0 && l->pos < l->len) {
        memmove(l->buf+l->pos,l->buf+l->pos+1,l->len-l->pos-1);
        l->len--;
        l->buf[l->len] = '\0';
        refreshLine(l);
    }
}

/* Backspace implementation. */
void linenoiseEditBackspace(struct linenoiseState *l) {
    if (l->pos > 0 && l->len > 0) {
        memmove(l->buf+l->pos-1,l->buf+l->pos,l->len-l->pos);
        l->pos--;
        l->len--;
        l->buf[l->len] = '\0';
        refreshLine(l);
    }
}

/* Delete the previous word, maintaining the cursor at the start of the
 * current word. */
void linenoiseEditDeletePrevWord(struct linenoiseState *l) {
    size_t old_pos = l->pos;
    size_t diff;

    while (l->pos > 0 && l->buf[l->pos-1] == ' ')
        l->pos--;
    while (l->pos > 0 && l->buf[l->pos-1] != ' ')
        l->pos--;
    diff = old_pos - l->pos;
    memmove(l->buf+l->pos,l->buf+old_pos,l->len-old_pos+1);
    l->len -= diff;
    refreshLine(l);
}

uint32_t getMillis(void) {
    struct timeval tv = { 0 };
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static inline size_t prompt_len_ignore_escape_seq(const char *prompt) {
    size_t plen = 0;
    bool in_escape_sequence = false;

    for (; *prompt != '\0'; ++prompt) {
        if (*prompt == '\033') {
            in_escape_sequence = true;
        } else if (in_escape_sequence && *prompt == 'm') {
            in_escape_sequence = false;
        } else if (!in_escape_sequence) {
            ++plen;
        }
    }

    return plen;
}

/* This function is the core of the line editing capability of linenoise.
 * It expects 'fd' to be already in "raw mode" so that every key pressed
 * will be returned ASAP to read().
 *
 * The resulting string is put into 'buf' when the user type enter, or
 * when ctrl+d is typed.
 *
 * The function returns the length of the current buffer. */
static int linenoiseEdit(char *buf, size_t buflen, const char *prompt)
{
    uint32_t t1 = 0;
    struct linenoiseState l;
    int out_fd = fileno(stdout);
    int in_fd = fileno(stdin);

    /* Populate the linenoise state that we pass to functions implementing
     * specific editing functionalities. */
    l.buf = buf;
    l.buflen = buflen;
    l.prompt = prompt;
    l.plen = strlen(prompt);
    l.oldpos = l.pos = 0;
    l.len = 0;
    l.cols = getColumns();
    l.maxrows = 0;
    l.history_index = 0;

    /* Buffer starts empty. */
    l.buf[0] = '\0';
    l.buflen--; /* Make sure there is always space for the nulterm */

    /* The latest history entry is always our current buffer, that
     * initially is just an empty string. */
    linenoiseHistoryAdd("");

    if (write(out_fd, prompt,l.plen) == -1) {
        return -1;
    }
    flushWrite();

    /* If the prompt has been registered with ANSI escape sequences
     * for terminal colors then we remove them from the prompt length
     * calculation. */
    l.plen = prompt_len_ignore_escape_seq(prompt);

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
        if (nread <= 0) return l.len;

        if ( (getMillis() - t1) < LINENOISE_PASTE_KEY_DELAY && c != ENTER) {
            /* Pasting data, insert characters without formatting.
             * This can only be performed when the cursor is at the end of the
             * line. */
            if (linenoiseInsertPastedChar(&l,c)) {
                return -1;
            }
            continue;
        }

        /* Only autocomplete when the callback is set. It returns < 0 when
         * there was an error reading from fd. Otherwise it will return the
         * character that should be handled next. */
        if (c == 9 && completionCallback != NULL) {
            int c2 = completeLine(&l);
            /* Return on errors */
            if (c2 < 0) return l.len;
            /* Read next character when 0 */
            if (c2 == 0) continue;
            c = c2;
        }

        switch(c) {
        case ENTER:    /* enter */
            history_len--;
            free(history[history_len]);
            if (mlmode) linenoiseEditMoveEnd(&l);
            if (hintsCallback) {
                /* Force a refresh without hints to leave the previous
                 * line as the user typed it after a newline. */
                linenoiseHintsCallback *hc = hintsCallback;
                hintsCallback = NULL;
                refreshLine(&l);
                hintsCallback = hc;
            }
            return (int)l.len;
        case CTRL_C:     /* ctrl-c */
            errno = EAGAIN;
            return -1;
        case BACKSPACE:   /* backspace */
        case 8:     /* ctrl-h */
            linenoiseEditBackspace(&l);
            break;
        case CTRL_D:     /* ctrl-d, remove char at right of cursor, or if the
                            line is empty, act as end-of-file. */
            if (l.len > 0) {
                linenoiseEditDelete(&l);
            } else {
                history_len--;
                free(history[history_len]);
                return -1;
            }
            break;
        case CTRL_T:    /* ctrl-t, swaps current character with previous. */
            if (l.pos > 0 && l.pos < l.len) {
                int aux = buf[l.pos-1];
                buf[l.pos-1] = buf[l.pos];
                buf[l.pos] = aux;
                if (l.pos != l.len-1) l.pos++;
                refreshLine(&l);
            }
            break;
        case CTRL_B:     /* ctrl-b */
            linenoiseEditMoveLeft(&l);
            break;
        case CTRL_F:     /* ctrl-f */
            linenoiseEditMoveRight(&l);
            break;
        case CTRL_P:    /* ctrl-p */
            linenoiseEditHistoryNext(&l, LINENOISE_HISTORY_PREV);
            break;
        case CTRL_N:    /* ctrl-n */
            linenoiseEditHistoryNext(&l, LINENOISE_HISTORY_NEXT);
            break;
        case ESC:    /* escape sequence */
            /* Read the next two bytes representing the escape sequence. */
            if (read(in_fd, seq, 2) < 2) {
                break;
            }

            /* ESC [ sequences. */
            if (seq[0] == '[') {
                if (seq[1] >= '0' && seq[1] <= '9') {
                    /* Extended escape, read additional byte. */
                    if (read(in_fd, seq+2, 1) == -1) {
                        break;
                    }
                    if (seq[2] == '~') {
                        switch(seq[1]) {
                        case '3': /* Delete key. */
                            linenoiseEditDelete(&l);
                            break;
                        }
                    }
                } else {
                    switch(seq[1]) {
                    case 'A': /* Up */
                        linenoiseEditHistoryNext(&l, LINENOISE_HISTORY_PREV);
                        break;
                    case 'B': /* Down */
                        linenoiseEditHistoryNext(&l, LINENOISE_HISTORY_NEXT);
                        break;
                    case 'C': /* Right */
                        linenoiseEditMoveRight(&l);
                        break;
                    case 'D': /* Left */
                        linenoiseEditMoveLeft(&l);
                        break;
                    case 'H': /* Home */
                        linenoiseEditMoveHome(&l);
                        break;
                    case 'F': /* End*/
                        linenoiseEditMoveEnd(&l);
                        break;
                    }
                }
            }

            /* ESC O sequences. */
            else if (seq[0] == 'O') {
                switch(seq[1]) {
                case 'H': /* Home */
                    linenoiseEditMoveHome(&l);
                    break;
                case 'F': /* End*/
                    linenoiseEditMoveEnd(&l);
                    break;
                }
            }
            break;
        default:
            if (linenoiseEditInsert(&l,c)) return -1;
            break;
        case CTRL_U: /* Ctrl+u, delete the whole line. */
            buf[0] = '\0';
            l.pos = l.len = 0;
            refreshLine(&l);
            break;
        case CTRL_K: /* Ctrl+k, delete from current to end of line. */
            buf[l.pos] = '\0';
            l.len = l.pos;
            refreshLine(&l);
            break;
        case CTRL_A: /* Ctrl+a, go to the start of the line */
            linenoiseEditMoveHome(&l);
            break;
        case CTRL_E: /* ctrl+e, go to the end of the line */
            linenoiseEditMoveEnd(&l);
            break;
        case CTRL_L: /* ctrl+l, clear screen */
            linenoiseClearScreen();
            refreshLine(&l);
            break;
        case CTRL_W: /* ctrl+w, delete previous word */
            linenoiseEditDeletePrevWord(&l);
            break;
        }
        flushWrite();
    }
    return l.len;
}

void linenoiseAllowEmpty(bool val) {
    allow_empty = val;
}

int linenoiseProbe(void) {
    /* Switch to non-blocking mode */
    int stdin_fileno = fileno(stdin);
    int flags = fcntl(stdin_fileno, F_GETFL);
    flags |= O_NONBLOCK;
    int res = fcntl(stdin_fileno, F_SETFL, flags);
    if (res != 0) {
        return -1;
    }
    /* Device status request */
    fprintf(stdout, "\x1b[5n");
    flushWrite();

    /* Try to read response */
    int timeout_ms = 500;
    const int retry_ms = 10;
    size_t read_bytes = 0;
    while (timeout_ms > 0 && read_bytes < 4) { // response is ESC[0n or ESC[3n
        usleep(retry_ms * 1000);
        timeout_ms -= retry_ms;
        char c;
        int cb = read(stdin_fileno, &c, 1);
        if (cb < 0) {
            continue;
        }
        if (read_bytes == 0 && c != ESC) {
            /* invalid response, try again until the timeout triggers */
            continue;
        }
        read_bytes += cb;
    }
    /* Restore old mode */
    flags &= ~O_NONBLOCK;
    res = fcntl(stdin_fileno, F_SETFL, flags);
    if (res != 0) {
        return -1;
    }
    if (read_bytes < 4) {
        return -2;
    }
    return 0;
}

static int linenoiseRaw(char *buf, size_t buflen, const char *prompt) {
    int count;

    if (buflen == 0) {
        errno = EINVAL;
        return -1;
    }

    count = linenoiseEdit(buf, buflen, prompt);
    fputc('\n', stdout);
    flushWrite();
    return count;
}

static int linenoiseDumb(char* buf, size_t buflen, const char* prompt) {
    /* dumb terminal, fall back to fgets */
    fputs(prompt, stdout);
    flushWrite();
    size_t count = 0;
    while (count < buflen) {
        int c = fgetc(stdin);
        if (c == '\n') {
            break;
        } else if (c == BACKSPACE || c == CTRL_H) {
            if (count > 0) {
                buf[count - 1] = 0;
                count--;

                /* Only erase symbol echoed from stdin. */
                fputs("\x08 ", stdout); /* Windows CMD: erase symbol under cursor */
                flushWrite();
            } else {
                /* Consume backspace if the command line is empty to avoid erasing the prompt */
                continue;
            }

        } else if (c <= UNIT_SEP) {
            /* Consume all character that are non printable (the backspace
             * case is handled above) */
            continue;
        } else {
            buf[count] = c;
            ++count;
        }
        fputc(c, stdout); /* echo */
        flushWrite();
    }
    fputc('\n', stdout);
    flushWrite();
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
char *linenoise(const char *prompt) {
    char *buf = calloc(1, max_cmdline_length);
    int count = 0;
    if (buf == NULL) {
        return NULL;
    }
    if (!dumbmode) {
        count = linenoiseRaw(buf, max_cmdline_length, prompt);
    } else {
        count = linenoiseDumb(buf, max_cmdline_length, prompt);
    }
    if (count > 0) {
        sanitize(buf);
        count = strlen(buf);
    } else if (count == 0 && allow_empty) {
        /* will return an empty (0-length) string */
    } else {
        free(buf);
        return NULL;
    }
    return buf;
}

/* This is just a wrapper the user may want to call in order to make sure
 * the linenoise returned buffer is freed with the same allocator it was
 * created with. Useful when the main program is using an alternative
 * allocator. */
void linenoiseFree(void *ptr) {
    free(ptr);
}

/* ================================ History ================================= */

void linenoiseHistoryFree(void) {
    if (history) {
        for (int j = 0; j < history_len; j++) {
            free(history[j]);
        }
        free(history);
    }
    history = NULL;
}

/* This is the API call to add a new entry in the linenoise history.
 * It uses a fixed array of char pointers that are shifted (memmoved)
 * when the history max length is reached in order to remove the older
 * entry and make room for the new one, so it is not exactly suitable for huge
 * histories, but will work well for a few hundred of entries.
 *
 * Using a circular buffer is smarter, but a bit more complex to handle. */
int linenoiseHistoryAdd(const char *line) {
    char *linecopy;

    if (history_max_len == 0) return 0;

    /* Initialization on first call. */
    if (history == NULL) {
        history = malloc(sizeof(char*)*history_max_len);
        if (history == NULL) return 0;
        memset(history,0,(sizeof(char*)*history_max_len));
    }

    /* Don't add duplicated lines. */
    if (history_len && !strcmp(history[history_len-1], line)) return 0;

    /* Add an heap allocated copy of the line in the history.
     * If we reached the max length, remove the older line. */
    linecopy = strdup(line);
    if (!linecopy) return 0;
    if (history_len == history_max_len) {
        free(history[0]);
        memmove(history,history+1,sizeof(char*)*(history_max_len-1));
        history_len--;
    }
    history[history_len] = linecopy;
    history_len++;
    return 1;
}

/* Set the maximum length for the history. This function can be called even
 * if there is already some history, the function will make sure to retain
 * just the latest 'len' elements if the new history length value is smaller
 * than the amount of items already inside the history. */
int linenoiseHistorySetMaxLen(int len) {
    char **new;

    if (len < 1) return 0;
    if (history) {
        int tocopy = history_len;

        new = malloc(sizeof(char*)*len);
        if (new == NULL) return 0;

        /* If we can't copy everything, free the elements we'll not use. */
        if (len < tocopy) {
            int j;

            for (j = 0; j < tocopy-len; j++) free(history[j]);
            tocopy = len;
        }
        memset(new,0,sizeof(char*)*len);
        memcpy(new,history+(history_len-tocopy), sizeof(char*)*tocopy);
        free(history);
        history = new;
    }
    history_max_len = len;
    if (history_len > history_max_len)
        history_len = history_max_len;
    return 1;
}

/* Save the history in the specified file. On success 0 is returned
 * otherwise -1 is returned. */
int linenoiseHistorySave(const char *filename) {
    FILE *fp;
    int j;

    fp = fopen(filename,"w");
    if (fp == NULL) return -1;
    for (j = 0; j < history_len; j++)
        fprintf(fp,"%s\n",history[j]);
    fclose(fp);
    return 0;
}

/* Load the history from the specified file. If the file does not exist
 * zero is returned and no operation is performed.
 *
 * If the file exists and the operation succeeded 0 is returned, otherwise
 * on error -1 is returned. */
int linenoiseHistoryLoad(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return -1;
    }

    char *buf = calloc(1, max_cmdline_length);
    if (buf == NULL) {
        fclose(fp);
        return -1;
    }

    while (fgets(buf, max_cmdline_length, fp) != NULL) {
        char *p;

        p = strchr(buf,'\r');
        if (!p) p = strchr(buf,'\n');
        if (p) *p = '\0';
        linenoiseHistoryAdd(buf);
    }

    free(buf);
    fclose(fp);

    return 0;
}

/* Set line maximum length. If len parameter is smaller than
 * LINENOISE_MINIMAL_MAX_LINE, -1 is returned
 * otherwise 0 is returned. */
int linenoiseSetMaxLineLen(size_t len) {
    if (len < LINENOISE_MINIMAL_MAX_LINE) {
        return -1;
    }
    max_cmdline_length = len;
    return 0;
}
