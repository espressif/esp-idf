/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

/**
 * @brief Symbols defined by the linker.
 * Retrieve the addresses of both .eh_frame_hdr and .eh_frame sections.
 */
extern char __eh_frame_hdr;
extern char __eh_frame_hdr_end;
extern char __eh_frame;
extern char __eh_frame_end;

/**
 * @brief Pointers to both .eh_frame_hdr and .eh_frame sections.
 */
#define EH_FRAME_HDR_ADDR       ((void*) (&__eh_frame_hdr))
#define EH_FRAME_HDR_END_ADDR   ((void*) (&__eh_frame_hdr_end))
#define EH_FRAME_ADDR           ((void*) (&__eh_frame))
#define EH_FRAME_END_ADDR       ((void*) (&__eh_frame_end))


void app_main(void)
{
    /* As soon as this test compiles, it can be considered passed. The linker should
     * test that the eh_frame and eh_frame_hdr sections are not empty but let's make
     * sure again that they are not empty. */
    assert((EH_FRAME_END_ADDR > EH_FRAME_ADDR) && ".eh_frame section must not be empty");
    assert((EH_FRAME_HDR_END_ADDR > EH_FRAME_HDR_ADDR) && ".eh_frame_hdr section must not be empty");

    /* Use the symbols just to make sure they won't be optimized away */
    printf(".eh_frame start: %p, end: %p\n", EH_FRAME_ADDR, EH_FRAME_END_ADDR);
    printf(".eh_frame_hdr start: %p, end: %p\n", EH_FRAME_HDR_ADDR, EH_FRAME_HDR_END_ADDR);
}
