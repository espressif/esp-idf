/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Utility functions to test that APB access is still safe
   while the other CPU performs some set of DPORT accesses

   (see ECO 3.10 and the standalone esp32 test_dport.c for more).
*/

/* start_apb_access_loop() starts a task reading from APB in a loop on the non-Unity-test CPU.

   Call this before doing something which involes DPORT reads.

   Does nothing in unicore mode.
*/
void start_apb_access_loop(void);

/* verify_apb_access_loop() kills the task started by start_apb_access_loop()
   and verifies that none of the APB reads were corrupted by unsafe DPORT reads.
*/
void verify_apb_access_loop(void);
