ULP coprocessor instruction set
===============================

This document provides details about the instructions used by ESP32 ULP coprocessor assembler.

ULP coprocessor has 4 16-bit general purpose registers, labeled R0, R1, R2, R3. It also has an 8-bit counter register (stage_cnt) which can be used to implement loops. Stage count regiter is accessed using special instructions.

ULP coprocessor can access 8k bytes of RTC_SLOW_MEM memory region. Memory is addressed in 32-bit word units. It can also access peripheral registers in RTC_CNTL, RTC_IO, and SENS peripherals.

All instructions are 32-bit. Jump instructions, ALU instructions, peripheral register and memory access instructions are executed in 1 cycle. Instructions which work with peripherals (TSENS, ADC, I2C) take variable number of cycles, depending on peripheral operation.

The instruction syntax is case insensitive. Upper and lower case letters can be used and intermixed arbitrarily. This is true both for register names and instruction names.

Note about addressing
---------------------
ESP32 ULP coprocessor's JUMP, ST, LD instructions which take register as an argument (jump address, store/load base address) expect the argument to be expressed in 32-bit words.

Consider the following example program::

  entry:
          NOP
          NOP
          NOP
          NOP
  loop:
          MOVE R1, loop
          JUMP R1

When this program is assembled and linked, address of label ``loop`` will be equal to 16 (expressed in bytes). However `JUMP` instruction expects the address stored in register to be expressed in 32-bit words. To account for this common use case, assembler will convert the address of label `loop` from bytes to words, when generating ``MOVE`` instruction, so the code generated code will be equivalent to::

  0000    NOP
  0004    NOP
  0008    NOP
  000c    NOP
  0010    MOVE R1, 4
  0014    JUMP R1

The other case is when the argument of ``MOVE`` instruction is not a label but a constant. In this case assembler will use the value as is, without any conversion::

          .set        val, 0x10
          MOVE        R1, val

In this case, value loaded into R1 will be ``0x10``.

Similar considerations apply to ``LD`` and ``ST`` instructions. Consider the following code::

          .global array
  array:  .long 0
          .long 0
          .long 0
          .long 0

          MOVE R1, array
          MOVE R2, 0x1234
          ST R2, R1, 0      // write value of R2 into the first array element,
                            // i.e. array[0]

          ST R2, R1, 4      // write value of R2 into the second array element
                            // (4 byte offset), i.e. array[1]

          ADD R1, R1, 2     // this increments address by 2 words (8 bytes)
          ST R2, R1, 0      // write value of R2 into the third array element,
                            // i.e. array[2]

**NOP** - no operation
----------------------

**Syntax:**
  **NOP**
**Operands:**
  None
**Description:**
  No operation is performed. Only the PC is incremented.

**Example**::

  1:    NOP


**ADD** - Add to register
-------------------------

**Syntax:**
    **ADD**      *Rdst, Rsrc1, Rsrc2*

    **ADD**      *Rdst, Rsrc1, imm*


**Operands:**
  - *Rdst* - Register R[0..3]
  - *Rsrc1* - Register R[0..3]
  - *Rsrc2* - Register R[0..3]
  - *Imm* - 16-bit signed value


**Description:**
  The instruction adds source register to another source register or to a 16-bit signed value and stores result to the destination register.

**Examples**::

  1:    ADD R1, R2, R3        //R1 = R2 + R3

  2:    Add R1, R2, 0x1234    //R1 = R2 + 0x1234

  3:    .set value1, 0x03     //constant value1=0x03
        Add R1, R2, value1    //R1 = R2 + value1


  4:    .global label         //declaration of variable label
        Add R1, R2, label     //R1 = R2 + label
          ...
        label: nop            //definition of variable label


**SUB** - Subtract from register
--------------------------------

**Syntax:**
  **SUB** *Rdst, Rsrc1, Rsrc2*

  **SUB** *Rdst, Rsrc1, imm*

**Operands:**
  - *Rdst*  - Register R[0..3]
  - *Rsrc1* - Register R[0..3]
  - *Rsrc2* - Register R[0..3]
  - *Imm*   - 16-bit signed value

**Description:**
  The instruction subtracts the source register from another source register or subtracts 16-bit signed value from a source register, and stores result to the destination register.

**Examples:**::

  1:         SUB R1, R2, R3             //R1 = R2 - R3
  
  2:         sub R1, R2, 0x1234         //R1 = R2 - 0x1234
  
  3:         .set value1, 0x03          //constant value1=0x03
             SUB R1, R2, value1         //R1 = R2 - value1
  4:         .global label              //declaration of variable label
             SUB R1, R2, label          //R1 = R2 - label
               ....
    label:   nop                        //definition of variable label


**AND** - Logical AND of two operands
-------------------------------------

**Syntax:**
    **AND** *Rdst, Rsrc1, Rsrc2*

    **AND** *Rdst, Rsrc1, imm*

**Operands:**
  - *Rdst* - Register R[0..3]
  - *Rsrc1* - Register R[0..3]
  - *Rsrc2* - Register R[0..3]
  - *Imm* - 16-bit signed value

**Description:**
  The instruction does logical AND of a source register and another source register or 16-bit signed value and stores result to the destination register.

**Example**::

  1:        AND R1, R2, R3          //R1 = R2 & R3

  2:        AND R1, R2, 0x1234      //R1 = R2 & 0x1234

  3:        .set value1, 0x03       //constant value1=0x03
            AND R1, R2, value1      //R1 = R2 & value1

  4:        .global label           //declaration of variable label
            AND R1, R2, label       //R1 = R2 & label
                ...
    label:  nop                     //definition of variable label


**OR** - Logical OR of two operands
-----------------------------------

**Syntax**
  **OR** *Rdst, Rsrc1, Rsrc2*
 
  **OR** *Rdst, Rsrc1, imm*


**Operands**
  - *Rdst* - Register R[0..3]
  - *Rsrc1* - Register R[0..3]
  - *Rsrc2* - Register R[0..3]
  - *Imm* - 16-bit signed value
 
**Description**
  The instruction does logical OR of a source register and another source register or 16-bit signed value and stores result to the destination register.

**Examples**::
 
  1:       OR R1, R2, R3           //R1 = R2 \| R3

  2:       OR R1, R2, 0x1234       //R1 = R2 \| 0x1234

  3:       .set value1, 0x03       //constant value1=0x03
           OR R1, R2, value1       //R1 = R2 \| value1

  4:       .global label           //declaration of variable label
           OR R1, R2, label        //R1 = R2 \|label
           ...
    label: nop                     //definition of variable label



**LSH** - Logical Shift Left
----------------------------

**Syntax**
  **LSH** *Rdst, Rsrc1, Rsrc2*

  **LSH** *Rdst, Rsrc1, imm*

**Operands**
  - *Rdst* - Register R[0..3]
  - *Rsrc1* - Register R[0..3]
  - *Rsrc2* - Register R[0..3]
  - *Imm* - 16-bit signed value
 
**Description**
   The instruction does logical shift to left of source register to number of bits from another source register or 16-bit signed value and store result to the destination register.

**Examples**::

  1:       LSH R1, R2, R3            //R1 = R2 << R3

  2:       LSH R1, R2, 0x03          //R1 = R2 << 0x03

  3:       .set value1, 0x03         //constant value1=0x03
           LSH R1, R2, value1        //R1 = R2 << value1

  4:       .global label             //declaration of variable label
           LSH R1, R2, label         //R1 = R2 << label
           ...
    label:  nop                       //definition of variable label


**RSH** - Logical Shift Right
-----------------------------

**Syntax**
   **RSH** *Rdst, Rsrc1, Rsrc2*

   **RSH** *Rdst, Rsrc1, imm*

**Operands**
  *Rdst* - Register R[0..3]
  *Rsrc1* - Register R[0..3]
  *Rsrc2* - Register R[0..3]
  *Imm* - 16-bit signed value

**Description**
  The instruction does logical shift to right of source register to number of bits from another source register or 16-bit signed value and store result to the destination register.

**Examples**::

  1:        RSH R1, R2, R3              //R1 = R2 >> R3

  2:        RSH R1, R2, 0x03            //R1 = R2 >> 0x03

  3:        .set value1, 0x03           //constant value1=0x03
            RSH R1, R2, value1          //R1 = R2 >> value1

  4:        .global label               //declaration of variable label
            RSH R1, R2, label           //R1 = R2 >> label
    label:  nop                         //definition of variable label



**MOVE** – Move to register
---------------------------

**Syntax**
  **MOVE**     *Rdst, Rsrc*

  **MOVE**     *Rdst, imm*

**Operands**
  - *Rdst* – Register R[0..3]
  - *Rsrc* – Register R[0..3]
  - *Imm*  – 16-bit signed value

**Description**
   The instruction move to destination register value from source register or 16-bit signed value.

   Note that when a label is used as an immediate, the address of the label will be converted from bytes to words. This is because LD, ST, and JUMP instructions expect the address register value to be expressed in words rather than bytes. To avoid using an extra instruction


**Examples**::

  1:        MOVE       R1, R2            //R1 = R2 >> R3

  2:        MOVE       R1, 0x03          //R1 = R2 >> 0x03

  3:        .set       value1, 0x03      //constant value1=0x03
            MOVE       R1, value1        //R1 = value1

  4:        .global     label            //declaration of label
            MOVE        R1, label        //R1 = address_of(label) / 4
            ...
    label:  nop                          //definition of label


**ST** – Store data to the memory
---------------------------------

**Syntax**
  **ST**     *Rsrc, Rdst, offset*

**Operands**
  - *Rsrc* – Register R[0..3], holds the 16-bit value to store
  - *Rdst* – Register R[0..3], address of the destination, in 32-bit words
  - *Offset* – 10-bit signed value, offset in bytes

**Description**
  The instruction stores the 16-bit value of Rsrc to the lower half-word of memory with address Rdst+offset. The upper half-word is written with the current program counter (PC), expressed in words, shifted left by 5 bits::

    Mem[Rdst + offset / 4]{31:0} = {PC[10:0], 5'b0, Rsrc[15:0]}

  The application can use higher 16 bits to determine which instruction in the ULP program has written any particular word into memory.

**Examples**::

  1:        ST  R1, R2, 0x12        //MEM[R2+0x12] = R1
  
  2:        .data                   //Data section definition
    Addr1:  .word     123           // Define label Addr1 16 bit
            .set      offs, 0x00    // Define constant offs
            .text                   //Text section definition
            MOVE      R1, 1         // R1 = 1
            MOVE      R2, Addr1     // R2 = Addr1
            ST        R1, R2, offs  // MEM[R2 +  0] = R1
                                    // MEM[Addr1 + 0] will be 32'h600001


**LD** – Load data from the memory
----------------------------------

**Syntax**
   **LD**      *Rdst, Rsrc, offset*

**Operands**
   *Rdst*  – Register R[0..3], destination
   
   *Rsrc* – Register R[0..3], holds address of destination, in 32-bit words
   
   *Offset* – 10-bit signed value, offset in bytes

**Description**
   The instruction loads lower 16-bit half-word from memory with address Rsrc+offset into the destination register Rdst::

     Rdst[15:0] = Mem[Rsrc + offset / 4][15:0]

**Examples**::

  1:        LD  R1, R2, 0x12            //R1 = MEM[R2+0x12]

  2:        .data                       //Data section definition
    Addr1:  .word     123               // Define label Addr1 16 bit
            .set      offs, 0x00        // Define constant offs
            .text                       //Text section definition
            MOVE      R1, 1             // R1 = 1
            MOVE      R2, Addr1         // R2 = Addr1 / 4 (address of label is converted into words)
            LD        R1, R2, offs      // R1 = MEM[R2 +  0]
                                        // R1 will be 123




**JUMP** – Jump to an absolute address
--------------------------------------

**Syntax**
  **JUMP**        *Rdst*

  **JUMP**        *ImmAddr*

  **JUMP**        *Rdst, Condition*

  **JUMP**        *ImmAddr, Condition*


**Operands**
  - *Rdst* – Register R[0..3] containing address to jump to (expressed in 32-bit words)

  - *ImmAddr* – 13 bits address (expressed in bytes), aligned to 4 bytes

  - *Condition*:
     - EQ – jump if last ALU operation result was zero
     - OV – jump if last ALU has set overflow flag


**Description**
  The instruction makes jump to the specified address. Jump can be either unconditional or based on an ALU flag.

**Examples**::

  1:        JUMP       R1            // Jump to address in R1 (address in R1 is in 32-bit words)

  2:        JUMP       0x120, EQ     // Jump to address 0x120 (in bytes) if ALU result is zero

  3:        JUMP       label         // Jump to label
            ...
    label:  nop                      // Definition of label

  4:        .global    label         // Declaration of global label

            MOVE       R1, label     // R1 = label (value loaded into R1 is in words)
            JUMP       R1            // Jump to label
            ...
    label:  nop                      // Definition of label



**JUMPR** – Jump to a relative offset (condition based on R0)
-------------------------------------------------------------

**Syntax**
   **JUMPR**        *Step, Threshold, Condition*

**Operands**
   - *Step*          – relative shift from current position, in bytes
   - *Threshold*     – threshold value for branch condition
   - *Condition*:
      - *GE* (greater or equal) – jump if value in R0 >= threshold 

      - *LT* (less than) – jump if value in R0 < threshold

**Description**
   The instruction makes a jump to a relative address if condition is true. Condition is the result of comparison of R0 register value and the threshold value.

**Examples**::

  1:pos:    JUMPR       16, 20, GE   // Jump to address (position + 16 bytes) if value in R0 >= 20

  2:        // Down counting loop using R0 register
            MOVE        R0, 16       // load 16 into R0
    label:  SUB         R0, R0, 1    // R0--
            NOP                      // do something
            JUMPR       label, 1, GE // jump to label if R0 >= 1



**JUMPS** – Jump to a relative address (condition based on stage count)
-----------------------------------------------------------------------

**Syntax**
   **JUMPS**        *Step, Threshold, Condition*

**Operands**
   - *Step*       – relative shift from current position, in bytes
   - *Threshold*  – threshold value for branch condition
   - *Condition*:
       - *EQ* (equal) – jump if value in stage_cnt == threshold
       - *LT* (less than) –  jump if value in stage_cnt < threshold
       - *GT* (greater than) –  jump if value in stage_cnt > threshold

**Description**
    The instruction makes a jump to a relative address if condition is true. Condition is the result of comparison of count register value and threshold value.

**Examples**::

   1:pos:    JUMPS     16, 20, EQ     // Jump to (position + 16 bytes) if stage_cnt == 20

   2:        // Up counting loop using stage count register
             STAGE_RST                  // set stage_cnt to 0
     label:  STAGE_INC  1               // stage_cnt++
             NOP                        // do something
             JUMPS       label, 16, LT  // jump to label if stage_cnt < 16



**STAGE_RST** – Reset stage count register
------------------------------------------
**Syntax**
     **STAGE_RST**

**Operands**
   No operands

**Description**
   The instruction sets the stage count register to 0

**Examples**::

   1:       STAGE_RST      // Reset stage count register



**STAGE_INC** – Increment stage count register
----------------------------------------------

**Syntax**
  **STAGE_INC**      *Value*

**Operands**
   - *Value* – 8 bits value

**Description**
   The instruction increments stage count register by given value.

**Examples**::

  1:        STAGE_INC      10          // stage_cnt += 10

  2:        // Up counting loop example:
            STAGE_RST                  // set stage_cnt to 0
    label:  STAGE_INC  1               // stage_cnt++
            NOP                        // do something
            JUMPS      label, 16, LT   // jump to label if stage_cnt < 16


**STAGE_DEC** – Decrement stage count register
----------------------------------------------

**Syntax**
  **STAGE_DEC**      *Value*

**Operands**
   - *Value* – 8 bits value

**Description**
   The instruction decrements stage count register by given value.

**Examples**::

  1:        STAGE_DEC      10        // stage_cnt -= 10;

  2:        // Down counting loop exaple
            STAGE_RST                // set stage_cnt to 0
            STAGE_INC  16            // increment stage_cnt to 16
    label:  STAGE_DEC  1             // stage_cnt--;
            NOP                      // do something
            JUMPS      label, 0, GT  // jump to label if stage_cnt > 0


**HALT** – End the program
--------------------------

**Syntax**
  **HALT**

**Operands**
  No operands
**Description**
   The instruction halt the processor to the power down mode

**Examples**::

  1:       HALT      // Move chip to powerdown



**WAKE** – wakeup the chip
--------------------------

**Syntax**
   **WAKE**

**Operands**
  No operands

**Description**
  The instruction sends an interrupt from ULP to RTC controller.

  - If the SoC is in deep sleep mode, and ULP wakeup is enabled, this causes the SoC to wake up.

  - If the SoC is not in deep sleep mode, and ULP interrupt bit (RTC_CNTL_ULP_CP_INT_ENA) is set in RTC_CNTL_INT_ENA_REG register, RTC interrupt will be triggered.

**Examples**::

  1:        WAKE                      // Trigger wake up
            REG_WR 0x006, 24, 24, 0   // Stop ULP timer (clear RTC_CNTL_ULP_CP_SLP_TIMER_EN)
            HALT                      // Stop the ULP program
            // After these instructions, SoC will wake up,
            // and ULP will not run again until started by the main program.



**SLEEP** – set ULP wakeup timer period
---------------------------------------

**Syntax**
  **SLEEP**   *sleep_reg*

**Operands**
   - *sleep_reg* – 0..4, selects one of ``SENS_ULP_CP_SLEEP_CYCx_REG`` registers.

**Description**
   The instruction selects which of the ``SENS_ULP_CP_SLEEP_CYCx_REG`` (x = 0..4) register values is to be used by the ULP wakeup timer as wakeup period. By default, the value from ``SENS_ULP_CP_SLEEP_CYC0_REG`` is used.

**Examples**::

  1:        SLEEP     1         // Use period set in SENS_ULP_CP_SLEEP_CYC1_REG

  2:        .set sleep_reg, 4   // Set constant
            SLEEP  sleep_reg    // Use period set in SENS_ULP_CP_SLEEP_CYC4_REG


**WAIT** – wait some number of cycles
-------------------------------------

**Syntax**
   **WAIT**   *Cycles*

**Operands**
   - *Cycles* – number of cycles for wait

**Description**
   The instruction delays for given number of cycles.

**Examples**::

  1:        WAIT     10         // Do nothing for 10 cycles

  2:        .set  wait_cnt, 10  // Set a constant
            WAIT  wait_cnt      // wait for 10 cycles




**TSENS** – do measurement with temperature sensor
--------------------------------------------------

**Syntax**
   - **TSENS**   *Rdst, Wait_Delay*

**Operands**
  - *Rdst* – Destination Register R[0..3], result will be stored to this register
  - *Wait_Delay* – number of cycles used to perform the measurement


**Description**
   The instruction performs measurement using TSENS and stores the result into a general purpose register.

**Examples**::

  1:        TSENS     R1, 1000     // Measure temperature sensor for 1000 cycles,
                                   // and store result to R1




**ADC** – do measurement with ADC
---------------------------------

**Syntax**
  **ADC**   *Rdst, Sar_sel, Mux, Cycles*

**Operands**
  - *Rdst* –      Destination Register R[0..3], result will be stored to this register
  - *Sar_sel* – selected ADC : 0=SARADC0, 1=SARADC1
  - *Mux*  -  selected PAD, SARADC Pad[Mux+1] is enabled
  - *Cycle* – number of cycles used to perform measurement

**Description**
  The instruction makes measurements from ADC.

**Examples**::

   1:        ADC      R1, 0, 1, 100 // Measure value using ADC1 pad 2,
                                    // for 100 cycles and move result to R1


**REG_RD** – read from peripheral register
------------------------------------------

**Syntax**
   **REG_RD**   *Addr, High, Low*

**Operands**
  - *Addr* – register address, in 32-bit words
  - *High* – High part of R0
  - *Low* – Low part of R0

**Description**
   The instruction reads up to 16 bits from a peripheral register into a general purpose register: ``R0 = REG[Addr][High:Low]``.

   This instruction can access registers in RTC_CNTL, RTC_IO, and SENS peripherals. Address of the the register, as seen from the ULP,
   can be calculated from the address of the same register on the DPORT bus as follows::

    addr_ulp = (addr_dport - DR_REG_RTCCNTL_BASE) / 4

**Examples**::

  1:        REG_RD      0x120, 2, 0     // load 4 bits: R0 = {12'b0, REG[0x120][7:4]}


**REG_WR** – write to peripheral register
-----------------------------------------

**Syntax**
  **REG_WR**   *Addr, High, Low, Data*

**Operands**
  - *Addr* – register address, in 32-bit words.
  - *High* – High part of R0
  - *Low* – Low part of R0
  - *Data* – value to write, 8 bits

**Description**
   The instruction writes up to 8 bits from a general purpose register into a peripheral register. ``REG[Addr][High:Low] = data``

   This instruction can access registers in RTC_CNTL, RTC_IO, and SENS peripherals. Address of the the register, as seen from the ULP,
   can be calculated from the address of the same register on the DPORT bus as follows::

    addr_ulp = (addr_dport - DR_REG_RTCCNTL_BASE) / 4

**Examples**::

   1:        REG_WR      0x120, 7, 0, 0x10   // set 8 bits: REG[0x120][7:0] = 0x10

Convenience macros for peripheral registers access
--------------------------------------------------

ULP source files are passed through C preprocessor before the assembler. This allows certain macros to be used to facilitate access to peripheral registers.

Some existing macros are defined in ``soc/soc_ulp.h`` header file. These macros allow access to the fields of peripheral registers by their names.
Peripheral registers names which can be used with these macros are the ones defined in ``soc/rtc_cntl_reg.h``, ``soc/rtc_io_reg.h``, and ``soc/sens_reg.h``.

READ_RTC_REG(rtc_reg, low_bit, bit_width)
  Read up to 16 bits from rtc_reg[low_bit + bit_width - 1 : low_bit] into R0. For example::

    #include "soc/soc_ulp.h"
    #include "soc/rtc_cntl_reg.h"

    /* Read 16 lower bits of RTC_CNTL_TIME0_REG into R0 */
    READ_RTC_REG(RTC_CNTL_TIME0_REG, 0, 16)

READ_RTC_FIELD(rtc_reg, field)
  Read from a field in rtc_reg into R0, up to 16 bits. For example::

    #include "soc/soc_ulp.h"
    #include "soc/sens_reg.h"

    /* Read 8-bit SENS_TSENS_OUT field of SENS_SAR_SLAVE_ADDR3_REG into R0 */
    READ_RTC_REG(SENS_SAR_SLAVE_ADDR3_REG, SENS_TSENS_OUT)

WRITE_RTC_REG(rtc_reg, low_bit, bit_width, value)
  Write immediate value into rtc_reg[low_bit + bit_width - 1 : low_bit], bit_width <= 8. For example::

    #include "soc/soc_ulp.h"
    #include "soc/rtc_io_reg.h"

    /* Set BIT(2) of RTC_GPIO_OUT_DATA_W1TS field in RTC_GPIO_OUT_W1TS_REG */
    WRITE_RTC_REG(RTC_GPIO_OUT_W1TS_REG, RTC_GPIO_OUT_DATA_W1TS_S + 2, 1, 1)


WRITE_RTC_FIELD(rtc_reg, field, value)
  Write immediate value into a field in rtc_reg, up to 8 bits. For example::

    #include "soc/soc_ulp.h"
    #include "soc/rtc_cntl_reg.h"

    /* Set RTC_CNTL_ULP_CP_SLP_TIMER_EN field of RTC_CNTL_STATE0_REG to 0 */
    READ_RTC_REG(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN, 0)






