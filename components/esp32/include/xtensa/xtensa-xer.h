/* xer-constants.h -- various constants describing external registers accessed 
      via wer and rer.

      TODO: find a better prefix. Also conditionalize certain constants based
      on number of cores and interrupts actually present.  
*/

/*
 * Copyright (c) 1999-2008 Tensilica Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <xtensa/config/core.h>

#define NUM_INTERRUPTS 27
#define NUM_CORES       4

/* Routing of NMI (BInterrupt2)	and interrupts 0..n-1 (BInterrupt3+) 
   RER reads
   WER writes
 */

#define XER_MIROUT           0x0000
#define XER_MIROUT_LAST      (XER_MIROUT + NUM_INTERRUPTS)


/* IPI to core M (all 16 causes). 
   
   RER reads
   WER clears
 */
#define XER_MIPICAUSE        0x0100
#define XER_MIPICAUSE_FIELD_A_FIRST 0x0
#define XER_MIPICAUSE_FIELD_A_LAST  0x0
#define XER_MIPICAUSE_FIELD_B_FIRST 0x1
#define XER_MIPICAUSE_FIELD_B_LAST  0x3
#define XER_MIPICAUSE_FIELD_C_FIRST 0x4
#define XER_MIPICAUSE_FIELD_C_LAST  0x7
#define XER_MIPICAUSE_FIELD_D_FIRST 0x8
#define XER_MIPICAUSE_FIELD_D_LAST  0xF


/* IPI from cause bit 0..15  

   RER invalid
   WER sets
*/
#define XER_MIPISET          0x0140
#define XER_MIPISET_LAST     0x014F


/* Global enable

   RER read
   WER clear
*/
#define XER_MIENG            0x0180


/* Global enable

   RER invalid
   WER set
*/
#define XER_MIENG_SET        0x0184

/* Global assert

   RER read
   WER clear
*/
#define XER_MIASG            0x0188


/* Global enable

   RER invalid
   WER set
*/
#define XER_MIASG_SET        0x018C


/* IPI partition register

   RER read
   WER write
*/
#define XER_PART            0x0190
#define XER_IPI0            0x0
#define XER_IPI1            0x1
#define XER_IPI2            0x2
#define XER_IPI3            0x3

#define XER_PART_ROUTE_IPI(NUM, FIELD) ((NUM) << ((FIELD) << 2))

#define XER_PART_ROUTE_IPI_CAUSE(TO_A, TO_B, TO_C, TO_D)		\
  (XER_PART_ROUTE_IPI(TO_A, XER_IPI0) |					\
   XER_PART_ROUTE_IPI(TO_B, XER_IPI1) |					\
   XER_PART_ROUTE_IPI(TO_C, XER_IPI2) |					\
   XER_PART_ROUTE_IPI(TO_D, XER_IPI3))

#define XER_IPI_WAKE_EXT_INTERRUPT XCHAL_EXTINT0_NUM
#define XER_IPI_WAKE_CAUSE  XER_MIPICAUSE_FIELD_C_FIRST
#define XER_IPI_WAKE_ADDRESS    (XER_MIPISET + XER_IPI_WAKE_CAUSE)
#define XER_DEFAULT_IPI_ROUTING XER_PART_ROUTE_IPI_CAUSE(XER_IPI1, XER_IPI0, XER_IPI2, XER_IPI3)


/* System configuration ID

   RER read
   WER invalid
*/
#define XER_SYSCFGID        0x01A0


/* RunStall to slave processors

   RER read
   WER write
*/
#define XER_MPSCORE        0x0200


/* Cache coherency ON

   RER read
   WER write
*/
#define XER_CCON           0x0220


