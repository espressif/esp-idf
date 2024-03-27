{IDF_TARGET_NAME} ULP 协处理器指令
=================================================

:link_to_translation:`en:[English]`

本文档详细介绍了 {IDF_TARGET_NAME} ULP FSM 协处理器汇编程序使用的指令。

ULP FSM 协处理器有 4 个 16 位通用寄存器，分别标记为 R0、R1、R2、R3，还有一个 8 位计数器寄存器 (``stage_cnt``) 用来实现循环。可以用特殊指令来访问阶段计数寄存器。

ULP 协处理器可以访问 8 K 字节大小的 RTC_SLOW_MEM 内存区域。内存以 32 位字单位寻址。它还可以访问 ``RTC_CNTL``、``RTC_IO`` 和 ``SENS`` 外设中的外设寄存器。

所有指令都是 32 位。跳转指令、ALU 指令、外设寄存器和内存访问指令在 1 个周期内执行。与外设（TSENS、ADC 和 I2C）相关的指令所需的周期不同，具体取决于外设操作。

指令语法不区分大小写。无论是寄存器名称还是指令名称，都可以任意混合使用大小写字母。

.. _ulp-fsm-addressing:

寻址注意事项
-----------------

对于 {IDF_TARGET_NAME} ULP FSM 协处理器的 ``JUMP``、``ST``、``LD`` 系列指令，地址参数应以如下方式表示（具体取决于使用的地址参数类型）：

- 当地址参数作为标签时，指令中的地址应为 32 位字。

  对于示例程序::

    entry:
            NOP
            NOP
            NOP
            NOP
    loop:
            MOVE R1, loop
            JUMP R1

  当此程序被汇编和链接时，标签 ``loop`` 的地址将为 16 字节。然而 ``JUMP`` 指令期望寄存器 ``R1`` 中存储的地址以 32 位字表示。由于这种情况较为常见，汇编程序会在生成 ``MOVE`` 指令时将标签 ``loop`` 的地址从字节转换为字。因此，生成的代码相当于::

    0000    NOP
    0004    NOP
    0008    NOP
    000c    NOP
    0010    MOVE R1, 4
    0014    JUMP R1

- 另一种情况是，``MOVE`` 指令的参数不是标签，而是常量。此时汇编程序将 **直接使用该常量** ，不进行任何转换::

            .set        val, 0x10
            MOVE        R1, val

  在这种情况下，加载到 ``R1`` 的值为 ``0x10``。

  但是，当使用立即数作为 ``LD`` 和 ``ST`` 指令的偏移量时，汇编程序会认为地址参数是字节，并在执行指令前将其转换为 32 位字::

            ST R1, R2, 4        // offset = 4 bytes; Mem[R2 + 4 / 4] = R1

  在这种情况下， R1 中的值存储在 [R2 + offset / 4] 指向的内存位置。

  请看以下代码::

            .global array
    array:  .long 0
            .long 0
            .long 0
            .long 0

            MOVE R1, array
            MOVE R2, 0x1234
            ST R2, R1, 0      // 将 R2 的值写入第一个数组元素，
                              // 即 array[0]

            ST R2, R1, 4      // 将 R2 的值写入第二个数组元素，
                              //（4 字节偏移量），即 array[1]

            ADD R1, R1, 2     // 将地址递增 2 个字（8 个字节）
            ST R2, R1, 0      // 将 R2 的值写入第三个数组元素，
                              // 即 array[2]

指令执行时间注意事项
---------------------------------

ULP 协处理器的时钟 ``RTC_FAST_CLK`` 通常来自内部的 8 MHz 振荡器。如果应用程序需要获知精确 ULP 时钟频率，可以根据主 XTAL 时钟进行校准::

    #include "soc/rtc.h"

    // calibrate 8M/256 clock against XTAL, get 8M/256 clock period
    uint32_t rtc_8md256_period = rtc_clk_cal(RTC_CAL_8MD256, 100);
    uint32_t rtc_fast_freq_hz = 1000000ULL * (1 << RTC_CLK_CAL_FRACT) * 256 / rtc_8md256_period;

ULP 协处理器在获取每个指令时需要一定的时钟周期，执行时同样需要一定的时钟周期，此周期数取决于具体的指令。下文详细列出了每个指令所需的执行时间信息。

指令获取时间：

- 2 个时钟周期 - ALU 和分支类的指令
- 4 个时钟周期 - 其他指令

注意，访问 RTC 存储器和 RTC 寄存器时，ULP 协处理器的优先级低于主 CPU。这意味着当主 CPU 与 ULP 访问同一块内存区域时，ULP 协处理器需要等待，主 CPU 会优先访问。

.. only:: esp32s2 or esp32s3

    ESP32 ULP 和 {IDF_TARGET_NAME} ULP 指令集的区别
    -----------------------------------------------------------------------

    与 ESP32 ULP FSM 协处理器相比，{IDF_TARGET_NAME} ULP FSM 协处理器具有扩展的指令集。{IDF_TARGET_NAME} ULP FSM 与 ESP32 ULP FSM 二进制不兼容，但在重新构建后，ESP32 ULP FSM 的汇编程序应能在 {IDF_TARGET_NAME} ULP FSM 上运行。

    添加到 {IDF_TARGET_NAME} ULP FSM 的新指令包括：``LDL``、``LDH``、``STL``、``STH``、``ST32``、``STO``、``STI``、``STI32``。


以下是所有指令的详细描述：

**NOP** - 无操作
----------------------

**语法**

  **NOP**

**操作数**

  无

**周期**

  执行需要 2 个周期，获取下一条指令需要 4 个周期

**描述**

  不执行任何操作，只增加 PC

**示例**::

  1:    NOP


**ADD** - 做加法运算
-----------------------------------------

**语法**

    **ADD**      **Rdst, Rsrc1, Rsrc2**

    **ADD**      **Rdst, Rsrc1, imm**


**操作数**

  - **Rdst** - 寄存器 R[0..3]
  - **Rsrc1** - 寄存器 R[0..3]
  - **Rsrc2** - 寄存器 R[0..3]
  - **Imm** - 16 位有符号值

**周期**

  执行需要 2 个周期，获取下一条指令需要 4 个周期

**描述**

  该指令将源寄存器与另一个源寄存器中的值相加或与一个 16 位有符号值相加，并将结果存储在目标寄存器中。

**示例**::

  1:    ADD R1, R2, R3        // R1 = R2 + R3

  2:    Add R1, R2, 0x1234    // R1 = R2 + 0x1234

  3:    .set value1, 0x03     // constant value1=0x03
        Add R1, R2, value1    // R1 = R2 + value1

  4:    .global label         // declaration of variable label
        add R1, R2, label     // R1 = R2 + label
          ...
        label: nop            // definition of variable label


**SUB** - 做减法运算
--------------------------------

**语法**

  **SUB** **Rdst, Rsrc1, Rsrc2**

  **SUB** **Rdst, Rsrc1, imm**

**操作数**

  - **Rdst**  - 寄存器 R[0..3]
  - **Rsrc1** - 寄存器 R[0..3]
  - **Rsrc2** - 寄存器 R[0..3]
  - **Imm**   - 16 位有符号值

**周期**

  执行需要 2 个周期，获取下一条指令需要 4 个周期

**描述**

  该指令将两个源寄存器中的值相减，或从一个源寄存器中减去一个 16 位有符号值，并将结果存储到目标寄存器中。

**示例**::

  1:         SUB R1, R2, R3             // R1 = R2 - R3

  2:         sub R1, R2, 0x1234         // R1 = R2 - 0x1234

  3:         .set value1, 0x03          // constant value1=0x03
             SUB R1, R2, value1         // R1 = R2 - value1
  4:         .global label              // declaration of variable label
             SUB R1, R2, label          // R1 = R2 - label
               ....
    label:   nop                        // definition of variable label


**AND** - 两个操作数的按位与
----------------------------------------

**语法**

    **AND** **Rdst, Rsrc1, Rsrc2**

    **AND** **Rdst, Rsrc1, imm**

**操作数**

  - **Rdst** - 寄存器 R[0..3]
  - **Rsrc1** - 寄存器 R[0..3]
  - **Rsrc2** - 寄存器 R[0..3]
  - **Imm** - 16 位有符号值

**周期**

  执行需要 2 个周期，获取下一条指令需要 4 个周期

**描述**

  该指令对源寄存器的值和另一个源寄存器的值或一个 16 位有符号值进行按位与操作，并将结果存储到目标寄存器中。

**示例**::

  1:        AND R1, R2, R3          // R1 = R2 & R3

  2:        AND R1, R2, 0x1234      // R1 = R2 & 0x1234

  3:        .set value1, 0x03       // constant value1=0x03
            AND R1, R2, value1      // R1 = R2 & value1

  4:        .global label           // declaration of variable label
            AND R1, R2, label       // R1 = R2 & label
                ...
    label:  nop                     // definition of variable label


**OR** - 两个操作数的按位或
-------------------------------------------

**语法**

  **OR** **Rdst, Rsrc1, Rsrc2**

  **OR** **Rdst, Rsrc1, imm**

**操作数**

  - **Rdst** - 寄存器 R[0..3]
  - **Rsrc1** - 寄存器 R[0..3]
  - **Rsrc2** - 寄存器 R[0..3]
  - **Imm** - 16 位有符号值

**周期**

  执行需要 2 个周期，获取下一条指令需要 4 个周期

**描述**

  该指令对源寄存器的值和另一个源寄存器的值或一个 16 位有符号值进行按位或操作，并将结果存储到目标寄存器中。

**示例**::

  1:       OR R1, R2, R3           // R1 = R2 || R3

  2:       OR R1, R2, 0x1234       // R1 = R2 || 0x1234

  3:       .set value1, 0x03       // constant value1=0x03
           OR R1, R2, value1       // R1 = R2 || value1

  4:       .global label           // declaration of variable label
           OR R1, R2, label        // R1 = R2 || label
           ...
    label: nop                     // definition of variable label


**LSH** - 逻辑左移
----------------------------

**语法**

  **LSH** **Rdst, Rsrc1, Rsrc2**

  **LSH** **Rdst, Rsrc1, imm**

**操作数**

  - **Rdst** - 寄存器 R[0..3]
  - **Rsrc1** - 寄存器 R[0..3]
  - **Rsrc2** - 寄存器 R[0..3]
  - **Imm** - 16 位有符号值

**周期**

  执行需要 2 个周期，获取下一条指令需要 4 个周期

**描述**

  该指令对源寄存器进行逻辑左移，移动的位数由另一个源寄存器或一个16位有符号值确定，并将结果存储到目标寄存器中。

.. note::

  大于 15 位的移位操作结果不确定。

**示例**::

  1:       LSH R1, R2, R3            // R1 = R2 << R3

  2:       LSH R1, R2, 0x03          // R1 = R2 << 0x03

  3:       .set value1, 0x03         // constant value1=0x03
           LSH R1, R2, value1        // R1 = R2 << value1

  4:       .global label             // declaration of variable label
           LSH R1, R2, label         // R1 = R2 << label
           ...
    label:  nop                      // definition of variable label


**RSH** - 逻辑右移
-----------------------------

**语法**

   **RSH** **Rdst, Rsrc1, Rsrc2**

   **RSH** **Rdst, Rsrc1, imm**

**操作数**

  - **Rdst** - 寄存器 R[0..3]
  - **Rsrc1** - 寄存器 R[0..3]
  - **Rsrc2** - 寄存器 R[0..3]
  - **Imm** - 16 位有符号值

**周期**

  执行需要 2 个周期，获取下一条指令需要 4 个周期

**描述**

  该指令对源寄存器进行逻辑右移，移动的位数由另一个源寄存器或一个 16 位有符号值确定，并将结果存储到目标寄存器中。

.. note::

   大于 15 位的移位操作结果未定义。

**示例**::

  1:        RSH R1, R2, R3              // R1 = R2 >> R3

  2:        RSH R1, R2, 0x03            // R1 = R2 >> 0x03

  3:        .set value1, 0x03           // constant value1=0x03
            RSH R1, R2, value1          // R1 = R2 >> value1

  4:        .global label               // declaration of variable label
            RSH R1, R2, label           // R1 = R2 >> label
    label:  nop                         // definition of variable label


**MOVE** – 移动到寄存器
---------------------------

**语法**

  **MOVE**     **Rdst, Rsrc**

  **MOVE**     **Rdst, imm**

**操作数**

  - **Rdst** – 寄存器 R[0..3]
  - **Rsrc** – 寄存器 R[0..3]
  - **Imm**  – 16 位有符号值

**周期**

  执行需要 2 个周期，获取下一条指令需要 4 个周期

**描述**

   该指令将源寄存器的值或一个 16 位有符号值移动到目标寄存器。

.. note::

    注意，当标签用作立即数时，标签的地址会从字节转换为字。这是因为对于 ``LD``、``ST`` 和 ``JUMP`` 指令，地址寄存器的值应以字表示，而不以字节表示。更多详细信息请参阅 :ref:`ulp-fsm-addressing`。


**示例**::

  1:        MOVE       R1, R2            // R1 = R2

  2:        MOVE       R1, 0x03          // R1 = 0x03

  3:        .set       value1, 0x03      // constant value1=0x03
            MOVE       R1, value1        // R1 = value1

  4:        .global     label            // declaration of label
            MOVE        R1, label        // R1 = address_of(label) / 4
            ...
    label:  nop                          // definition of label


**ST** – 将数据存储到内存中
---------------------------------

**语法**

  **ST**     **Rsrc, Rdst, offset**

**操作数**

  - **Rsrc** – 寄存器 R[0..3]，保存要存储的 16 位值
  - **Rdst** – 寄存器 R[0..3]，目标地址，以 32 位字为单位
  - **Offset** – 13 位有符号值，以字节表示

**周期**

  执行需要 4 个周期，获取下一条指令需要 4 个周期

**描述**

  该指令将 Rsrc 的 16 位值存储到目标内存地址 [Rdst + offset] 中，存储的数据占目标内存中一个字的低半部分，而高半部分由当前程序计数器 PC 的值（以字为单位，左移 5 位）与 Rdst（0..3）进行逻辑“或”运算::

    Mem[Rdst + offset / 4]{31:0} = {PC[10:0], 3'b0, Rdst, Rsrc[15:0]}

  应用程序可以使用高 16 位来确定 ULP 程序中的哪条指令将某个特定的字写入了内存。

.. note::

        注意，以字节为单位指定的偏移量会在执行前被转换为 32 位字偏移量。更多信息请参阅 :ref:`ulp-fsm-addressing`。

**示例**::

  1:        ST  R1, R2, 0x12        // MEM[R2 + 0x12 / 4] = R1

  2:        .data                   // Data section definition
    Addr1:  .word     123           // Define label Addr1 16 bit
            .set      offs, 0x00    // Define constant offs
            .text                   // Text section definition
            MOVE      R1, 1         // R1 = 1
            MOVE      R2, Addr1     // R2 = Addr1
            ST        R1, R2, offs  // MEM[R2 +  0 / 4] = R1
                                    // MEM[Addr1 + 0] will be 32'h600001

.. only:: esp32s2 or esp32s3

    **STL** – 将数据存储到 32 位内存的低 16 位
    ----------------------------------------------------------------

    **语法**

      **STL**    **Rsrc, Rdst, offset, Label**

    **操作数**

      - **Rsrc** – 寄存器 R[0..3]，保存要存储的 16 位值
      - **Rdst** – 寄存器 R[0..3]，目标地址，以 32 位字为单位
      - **Offset** – 11 位有符号值，以字节为单位的偏移
      - **Label** – 用户定义的 2 位无符号值

    **周期**

      执行需要 4 个周期，获取下一条指令需要 4 个周期

    **描述**

      该指令将 Rsrc 的 16 位值存储到地址为 [Rdst + offset / 4] 的内存的低半字中::

        Mem[Rdst + offset / 4]{15:0} = {Rsrc[15:0]}
        Mem[Rdst + offset / 4]{15:0} = {Label[1:0],Rsrc[13:0]}

      ``ST`` 和 ``STL`` 命令可以互换使用，以保持对早期版本 ULP 核的向后兼容性。

    .. note::

        注意，以字节为单位指定的偏移量会在执行前被转换为 32 位字偏移量。更多信息请参阅 :ref:`ulp-fsm-addressing`。

    **示例**::

      1:        STL  R1, R2, 0x12           // MEM[R2 + 0x12 / 4] = R1

      2:        .data                       // Data section definition
        Addr1:  .word     123               // Define label Addr1 16 bit
                .set      offs, 0x00        // Define constant offs
                .text                       // Text section definition
                MOVE      R1, 1             // R1 = 1
                MOVE      R2, Addr1         // R2 = Addr1
                STL       R1, R2, offs      // MEM[R2 +  0 / 4] = R1
                                            // MEM[Addr1 + 0] will be 32'hxxxx0001
      3:
                MOVE      R1, 1             // R1 = 1
                STL       R1, R2, 0x12, 1   // MEM[R2 + 0x12 / 4] = 0xxxxx4001


    **STH** – 将数据存储到 32 位内存的高 16 位
    ------------------------------------------------------------

    **语法**

      **STH**     **Rsrc, Rdst, offset, Label**

    **操作数**

      - **Rsrc** – 寄存器 R[0..3]，保存要存储的 16 位值
      - **Rdst** – 寄存器 R[0..3]，目标地址，以 32 位字为单位
      - **Offset** – 11 位有符号值，以字节为单位的偏移
      - **Label** – 用户定义的 2 位无符号值

    **周期**

      执行需要 4 个周期，获取下一条指令需要 4 个周期

    **描述**

      该指令将 Rsrc 的 16 位值存储到地址为 [Rdst + offset / 4] 的内存的高半字中::

        Mem[Rdst + offset / 4]{31:16} = {Rsrc[15:0]}
        Mem[Rdst + offset / 4]{31:16} = {Label[1:0],Rsrc[13:0]}

    .. note::

        注意，以字节为单位指定的偏移量会在执行前被转换为 32 位字偏移量。更多信息请参阅 :ref:`ulp-fsm-addressing`。

    **示例**::

      1:        STH  R1, R2, 0x12           // MEM[R2 + 0x12 / 4][31:16] = R1

      2:        .data                       // Data section definition
        Addr1:  .word     123               // Define label Addr1 16 bit
                .set      offs, 0x00        // Define constant offs
                .text                       // Text section definition
                MOVE      R1, 1             // R1 = 1
                MOVE      R2, Addr1         // R2 = Addr1
                STH       R1, R2, offs      // MEM[R2 +  0 / 4] = R1
                                            // MEM[Addr1 + 0] will be 32'h0001xxxx
      3:
                MOVE      R1, 1             // R1 = 1
                STH       R1, R2, 0x12, 1   // MEM[R2 + 0x12 / 4] 0x4001xxxx


    **ST32** – 将 32 位数据存储到 32 位内存
    ---------------------------------------------------

    **语法**

      **ST32**     **Rsrc, Rdst, offset, Label**

    **操作数**

      - **Rsrc** – 寄存器 R[0..3]，保存要存储的 16 位值
      - **Rdst** – 寄存器 R[0..3]，目标地址，以 32 位字为单位
      - **Offset** – 11 位有符号值，以字节为单位的偏移
      - **Label** – 用户定义的 2 位无符号值

    **周期**

      执行需要 4 个周期，获取下一条指令需要 4 个周期

    **描述**

      该指令将 PC 的 11 位值，标签值和 Rsrc 的 16 位值存储到地址为 [Rdst + offset / 4] 的 32 位内存中::

        Mem[Rdst + offset / 4]{31:0} = {PC[10:0],0[2:0],Label[1:0],Rsrc[15:0]}

    .. note::

        注意，以字节为单位指定的偏移量会在执行前被转换为 32 位字偏移量。更多信息请参阅 :ref:`ulp-fsm-addressing`。

    **示例**::

      1:        ST32  R1, R2, 0x12, 0       // MEM[R2 + 0x12 / 4][31:0] = {PC[10:0],0[2:0],Label[1:0],Rsrc[15:0]}

      2:        .data                       // Data section definition
        Addr1:  .word     123               // Define label Addr1 16 bit
                .set      offs, 0x00        // Define constant offs
                .text                       // Text section definition
                MOVE      R1, 1             // R1 = 1
                MOVE      R2, Addr1         // R2 = Addr1
                ST32      R1, R2, offs, 1   // MEM[R2 +  0] = {PC[10:0],0[2:0],Label[1:0],Rsrc[15:0]}
                                            // MEM[Addr1 + 0] will be 32'h00010001


    **STO** – 设置自动递增操作的偏移值
    -------------------------------------------------------

    **语法**

      **STO**     **offset**

    **操作数**

      - **Offset** – 11 位有符号值，以字节为单位的偏移

    **周期**

      执行需要 4 个周期，获取下一条指令需要 4 个周期

    **描述**

      该指令将 16 位值设置到偏移寄存器::

        offset = value / 4

    .. note::

        注意，以字节为单位指定的偏移量会在执行前被转换为 32 位字偏移量。更多信息请参阅 :ref:`ulp-fsm-addressing`。

    **示例**::

      1:        STO  0x12               // Offset = 0x12 / 4

      2:        .data                   // Data section definition
        Addr1:  .word     123           // Define label Addr1 16 bit
                .set      offs, 0x00    // Define constant offs
                .text                   // Text section definition
                STO       offs          // Offset = 0x00


    **STI** – 将数据存储到 32 位内存中，并自动递增预定义地址偏移
    -------------------------------------------------------------------------------------------

    **语法**

      **STI**     **Rsrc, Rdst, Label**

    **操作数**

      - **Rsrc** – 寄存器 R[0..3]，保存要存储的 16 位值
      - **Rdst** – 寄存器 R[0..3]，目标地址，以 32 位字为单位
      - **Label** – 用户定义的 2 位无符号值

    **周期**

      执行需要 4 个周期，获取下一条指令需要 4 个周期

    **描述**

      该指令将 Rsrc 的 16 位值存储到地址为 [Rdst + offset / 4] 的内存的低半字和高半字中。

      当 STI 指令被调用两次时，会自增偏移量。请确保在执行 STI 指令之前，执行 ``STO`` 指令来设置该偏移值::

        Mem[Rdst + offset / 4]{15:0/31:16} = {Rsrc[15:0]}
        Mem[Rdst + offset / 4]{15:0/31:16} = {Label[1:0],Rsrc[13:0]}

    **示例**::

      1:        STO  4                  // Set offset to 4
                STI  R1, R2             // MEM[R2 + 4 / 4][15:0] = R1
                STI  R1, R2             // MEM[R2 + 4 / 4][31:16] = R1
                                        // offset += (1 * 4) //offset is incremented by 1 word
                STI  R1, R2             // MEM[R2 + 8 / 4][15:0] = R1
                STI  R1, R2             // MEM[R2 + 8 / 4][31:16] = R1


    **STI32** – 将 32 位数据存储到 32 位内存中，并自动递增地址偏移
    -----------------------------------------------------------------------------------------

    **语法**

      **STI32**     **Rsrc, Rdst, Label**

    **操作数**

      - **Rsrc** – 寄存器 R[0..3]，保存要存储的 16 位值
      - **Rdst** – 寄存器 R[0..3]，目标地址，以 32 位字为单位
      - **Label** – 用户定义的 2 位无符号值

    **周期**

      执行需要 4 个周期，获取下一条指令需要 4 个周期

    **描述**

      该指令将 PC 的 11 位值，标签值和 Rsrc 的 16 位值存储到地址为 [Rdst + offset / 4] 的 32 位内存中。

      每次调用 STI32 指令时，偏移值都会自动增加。确保在执行 STI32 指令之前，执行 ``STO`` 指令来设置偏移值::

        Mem[Rdst + offset / 4]{31:0} = {PC[10:0],0[2:0],Label[1:0],Rsrc[15:0]}

    **示例**::

      1:        STO    0x12
                STI32  R1, R2, 0    // MEM[R2 + 0x12 / 4][31:0] = {PC[10:0],0[2:0],Label[1:0],Rsrc[15:0]}
                                    // offset += (1 * 4) //offset is incremented by 1 word
                STI32  R1, R2, 0    // MEM[R2 + 0x16 / 4][31:0] = {PC[10:0],0[2:0],Label[1:0],Rsrc[15:0]}


**LD** – 从内存中加载数据
------------------------------------

**语法**

   **LD**      **Rdst, Rsrc, offset**

**操作数**

   - **Rdst** – 寄存器 R[0..3]，目标寄存器
   - **Rsrc** – 寄存器 R[0..3]，保存目标地址，以 32 位字为单位
   - **Offset** – 13 位有符号值，以字节为单位的偏移量

**周期**

  执行需要 4 个周期，获取下一条指令需要 4 个周期

**描述**

  该指令将地址为 [Rsrc + offset / 4] 的内存中的 16 位低半字加载到目标寄存器 Rdst 中::

    Rdst[15:0] = Mem[Rsrc + offset / 4][15:0]

.. note::

        注意，以字节为单位指定的偏移量会在执行前被转换为 32 位字偏移量。详情请参阅 :ref:`ulp-fsm-addressing`。

**示例**::

  1:        LD  R1, R2, 0x12            // R1 = MEM[R2 + 0x12 / 4]

  2:        .data                       // Data section definition
    Addr1:  .word     123               // Define label Addr1 16 bit
            .set      offs, 0x00        // Define constant offs
            .text                       // Text section definition
            MOVE      R1, 1             // R1 = 1
            MOVE      R2, Addr1         // R2 = Addr1 / 4 (address of label is converted into words)
            LD        R1, R2, offs      // R1 = MEM[R2 +  0]
                                        // R1 will be 123

.. only:: esp32s2 or esp32s3

    **LDL** – 从 32 位内存的低半字中加载数据
    -------------------------------------------------

    **语法**

       **LDL**     **Rdst, Rsrc, offset**

    **操作数**

       - **Rdst** – 寄存器 R[0..3]，目标寄存器
       - **Rsrc** – 寄存器 R[0..3]，保存目标地址，以 32 位字为单位
       - **Offset** – 13 位有符号值，以字节为单位的偏移量

    **周期**

      执行需要 4 个周期，获取下一条指令需要 4 个周期

    **描述**

       该指令将地址为 [Rsrc + offset / 4] 的内存中的 16 位低半字加载到目标寄存器 Rdst 中::

         Rdst[15:0] = Mem[Rsrc + offset / 4][15:0]

       ``LD`` 和 ``LDL`` 命令可以互换使用，以保持对早期版本 ULP 核的向后兼容性。

    .. note::

        注意，以字节为单位指定的偏移量会在执行前被转换为 32 位字偏移量。更多信息请参阅 :ref:`ulp-fsm-addressing`。

    **示例**::

      1:        LDL  R1, R2, 0x12           // R1 = MEM[R2 + 0x12 / 4]

      2:        .data                       // Data section definition
        Addr1:  .word     123               // Define label Addr1 16 bit
                .set      offs, 0x00        // Define constant offs
                .text                       // Text section definition
                MOVE      R1, 1             // R1 = 1
                MOVE      R2, Addr1         // R2 = Addr1 / 4 (address of label is converted into words)
                LDL       R1, R2, offs      // R1 = MEM[R2 +  0]
                                            // R1 will be 123


    **LDH** – 从 32 位内存的高半字加载数据
    --------------------------------------------------------------

    **语法**

       **LDH**     **Rdst, Rsrc, offset**

    **操作数**

       - **Rdst**  – 寄存器 R[0..3]，目标寄存器
       - **Rsrc** – 寄存器 R[0..3]，保存目标地址，以 32 位字为单位
       - **Offset** – 13 位有符号值，以字节为单位的偏移

    **周期**

      执行需要 4 个周期，获取下一条指令需要 4 个周期

    **描述**

       该指令将地址为 [Rsrc + offset / 4] 的内存中的 16 位高半字加载到目标寄存器 Rdst 中::

         Rdst[15:0] = Mem[Rsrc + offset / 4][15:0]

    .. note::

        注意，以字节为单位指定的偏移量会在执行前被转换为 32 位字偏移量。更多信息请参阅 :ref:`ulp-fsm-addressing`。

    **示例**::

      1:        LDH  R1, R2, 0x12           // R1 = MEM[R2 + 0x12 / 4]

      2:        .data                       // Data section definition
        Addr1:  .word     0x12345678        // Define label Addr1 16 bit
                .set      offs, 0x00        // Define constant offs
                .text                       // Text section definition
                MOVE      R1, 1             // R1 = 1
                MOVE      R2, Addr1         // R2 = Addr1 / 4 (address of label is converted into words)
                LDH       R1, R2, offs      // R1 = MEM[R2 +  0]
                                            // R1 will be 0x1234


**JUMP** – 跳转到绝对地址
-------------------------------

**语法**

  **JUMP**        **Rdst**

  **JUMP**        **ImmAddr**

  **JUMP**        **Rdst, Condition**

  **JUMP**        **ImmAddr, Condition**


**操作数**

  - **Rdst** – 寄存器 R[0..3] 包含要跳转到的地址（以 32 位字表示）
  - **ImmAddr** – 13 位地址（以字节表示），对齐为 4 字节
  - **Condition**：
    - EQ – 如果最后的 ALU 操作结果为零，则跳转
    - OV – 如果最后的 ALU 设置了溢出 flag，则跳转

**周期**

  执行需要 2 个周期，获取下一条指令需要 2 个周期。

**描述**

  该指令跳转到指定的地址。既可以无条件跳转，也可以基于 ALU flag 跳转。

**示例**::

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


**JUMPR** – 跳转到相对偏移（条件基于 R0）
-------------------------------------------------------------

**语法**

   **JUMPR**        **Step, Threshold, Condition**

**操作数**

   - **Step**          – 相对于当前位置的偏移量，以字节为单位
   - **Threshold**     – 分支条件的阈值
   - **Condition**：
      - **EQ** (等于) – 如果 R0 中的值 == 阈值，则跳转
      - **LT** (小于) – 如果 R0 中的值 < 阈值，则跳转
      - **LE** (小于或等于) – 如果 R0 中的值 <= 阈值，则跳转
      - **GT** (大于) – 如果 R0 中的值 > 阈值，则跳转
      - **GE** (大于或等于) – 如果 R0 中的值 >= 阈值，则跳转

**周期**

.. only:: esp32

    条件 **LT**，**GE**，**LE** 和 **GT**：执行需要 2 个周期，获取下一条指令需要 2 个周期。

    条件 **LE** 和 **GT** 在汇编程序中用一个 **JUMPR** 指令实现::

      // JUMPR target, threshold, GT 的实现为：

               JUMPR target, threshold+1, GE

      // JUMPR target, threshold, LE 的实现为：

               JUMPR target, threshold + 1, LT

    条件 **EQ** 在汇编程序中用两个 **JUMPR** 指令实现::

      // JUMPR target, threshold, EQ 的实现为：

               JUMPR next, threshold + 1, GE
               JUMPR target, threshold, GE
      next:


.. only:: esp32s2 or esp32s3

    条件 **EQ**，**GT** 和 **LT**：执行需要 2 个周期，获取下一条指令需要 2 个周期。

    条件 **LE** 和 **GE** 在汇编程序中使用两个 **JUMPR** 指令实现::

      // JUMPR target, threshold, LE is implemented as:

               JUMPR target, threshold, EQ
               JUMPR target, threshold, LT

      // JUMPR target, threshold, GE is implemented as:

               JUMPR target, threshold, EQ
               JUMPR target, threshold, GT

    因此，执行时间取决于所用分支：要么执行 2 个周期 + 获取 2 个周期，要么执行 4 个周期 + 获取 4 个周期。


**描述**

   如果条件为真，该指令会跳转到相对地址。条件是指 R0 寄存器的值和阈值的比较结果。

**示例**::

  1:pos:    JUMPR       16, 20, GE   // Jump to address (position + 16 bytes) if value in R0 >= 20

  2:        // Down counting loop using R0 register
            MOVE        R0, 16       // load 16 into R0
    label:  SUB         R0, R0, 1    // R0--
            NOP                      // do something
            JUMPR       label, 1, GE // jump to label if R0 >= 1


**JUMPS** – 跳转到相对地址（条件基于阶段数）
-------------------------------------------------------

**语法**

   **JUMPS**        **步骤, 阈值, 条件**

**操作数**

   - **步骤** – 相对于当前位置的偏移，以字节为单位
   - **阈值** – 分支条件的阈值
   - **条件**：
       - **EQ** (等于) – 如果 stage_cnt == 阈值，则跳转
       - **LT** (小于) – 如果 stage_cnt < 阈值，则跳转
       - **LE** (小于或等于) - 如果 stage_cnt <= 阈值，则跳转
       - **GT** (大于) – 如果 stage_cnt > 阈值，则跳转
       - **GE** (大于或等于) — 如果 stage_cnt >= 阈值，则跳转

**周期**

.. only:: esp32

    条件 **LE**, **LT**, **GE**：执行需要 2 个周期，获取下一条指令需要 2 个周期。

    条件 **EQ**, **GT** 在汇编程序中用两个 **JUMPS** 指令实现::

      // JUMPS target, threshold, EQ 的实现为：

               JUMPS next, threshold, LT
               JUMPS target, threshold, LE
      next:

      // JUMPS target, threshold, GT 的实现为:

               JUMPS next, threshold, LE
               JUMPS target, threshold, GE
      next:

    因此，执行时间取决于所用分支：要么执行 2 个周期 + 获取 2 个周期，要么执行 4 个周期 + 获取 4 个周期。


.. only:: esp32s2 or esp32s3

    执行需要 2 个周期，获取下一条指令需要 2 个周期。


**描述**

    如果条件为真，指令将跳转到相对地址。条件是计数寄存器的值和阈值的比较结果。

**示例**::

   1:pos:    JUMPS     16, 20, EQ     // Jump to (position + 16 bytes) if stage_cnt == 20

   2:        // Up counting loop using stage count register
             STAGE_RST                  // set stage_cnt to 0
     label:  STAGE_INC  1               // stage_cnt++
             NOP                        // do something
             JUMPS       label, 16, LT  // jump to label if stage_cnt < 16


**STAGE_RST** – 重置阶段计数寄存器
------------------------------------------

**语法**

     **STAGE_RST**

**操作数**

   无操作数

**描述**

   该指令将阶段计数寄存器设置为 0

**周期**

  执行需要 2 个周期，获取下一条指令需要 4 个周期

**示例**::

   1:       STAGE_RST      // 重置阶段计数寄存器


**STAGE_INC** – 增加阶段计数寄存器
----------------------------------------------

**语法**

  **STAGE_INC**      **值**

**操作数**

   - **值** – 8 位值

**周期**

  执行需要 2 个周期，获取下一条指令需要 4 个周期。

**描述**

   该指令将给定值增加到阶段计数寄存

**示例**::

  1:        STAGE_INC      10          // stage_cnt += 10

  2:        // Up counting loop example:
            STAGE_RST                  // set stage_cnt to 0
    label:  STAGE_INC  1               // stage_cnt++
            NOP                        // do something
            JUMPS      label, 16, LT   // jump to label if stage_cnt < 16


**STAGE_DEC** – 减少阶段计数寄存器
----------------------------------------------

**语法**

  **STAGE_DEC**      **值**

**操作数**

   - **值** – 8 位值

**周期**

  执行需要 2 个周期，获取下一条指令需要 4 个周期

**描述**

   该指令从阶段计数寄存器中减去给定值

**示例**::

  1:        STAGE_DEC      10        // stage_cnt -= 10;

  2:        // Down counting loop example
            STAGE_RST                // set stage_cnt to 0
            STAGE_INC  16            // increment stage_cnt to 16
    label:  STAGE_DEC  1             // stage_cnt--;
            NOP                      // do something
            JUMPS      label, 0, GT  // jump to label if stage_cnt > 0


**HALT** – 结束程序
--------------------------

**语法**

  **HALT**

**操作数**

  无操作数

**周期**

  执行需要 2 个周期

**描述**

    该指令会停止 ULP 协处理器并重新启动 ULP 唤醒定时器（如果定时器已启用）

**示例**::

    1:       HALT      // Halt the coprocessor


**WAKE** – 唤醒芯片
---------------------------

**语法**

   **WAKE**

**操作数**

  无操作数

**周期**

  执行需要 2 个周期，获取下一条指令需要 4 个周期

**描述**

  该指令从 ULP 协处理器向 RTC 控制器发送一个中断。

  - 如果 SoC 处于深度睡眠模式并启用了 ULP 唤醒，会唤醒 SoC。

  - 如果 SoC 不处在深度睡眠模式，并且在 RTC_CNTL_INT_ENA_REG 寄存器中设置了 ULP 中断位 (RTC_CNTL_ULP_CP_INT_ENA)，则会触发 RTC 中断。

.. note::

    注意，在使用 WAKE 指令前，ULP 程序可能需要等待 RTC 控制器就绪，才能唤醒主 CPU。此信息通过 RTC_CNTL_LOW_POWER_ST_REG 寄存器的 RTC_CNTL_RDY_FOR_WAKEUP 位来指示。当 RTC_CNTL_RDY_FOR_WAKEUP 为零时，执行 WAKE 指令唤醒无效。如果希望在主 CPU 不处于睡眠模式时使用 WAKE 指令，可以用 RTC_CNTL_LOW_POWER_ST_REG 的 RTC_CNTL_MAIN_STATE_IN_IDLE 位（位 27）来检查主 CPU 状态，确定其处于正常模式还是睡眠模式。

**示例**::

  1: is_rdy_for_wakeup:                   // Read RTC_CNTL_RDY_FOR_WAKEUP bit
            READ_RTC_FIELD(RTC_CNTL_LOW_POWER_ST_REG, RTC_CNTL_RDY_FOR_WAKEUP)
            AND r0, r0, 1
            JUMP is_rdy_for_wakeup, eq    // Retry until the bit is set
            WAKE                          // Trigger wake up
            REG_WR 0x006, 24, 24, 0       // Stop ULP timer (clear RTC_CNTL_ULP_CP_SLP_TIMER_EN)
            HALT                          // Stop the ULP program
            // After these instructions, SoC will wake up,
            // and ULP will not run again until started by the main program.

  1: check_wakeup:                        // Read RTC_CNTL_RDY_FOR_WAKEUP and RTC_CNTL_MAIN_STATE_IN_IDLE bit
            READ_RTC_REG(RTC_CNTL_LOW_POWER_ST_REG, 27, 1)
            MOVE r1, r0                   // Copy result in to r1
            READ_RTC_FIELD(RTC_CNTL_LOW_POWER_ST_REG, RTC_CNTL_RDY_FOR_WAKEUP)
            OR r0, r0, r1
            JUMP check_wakeup, eq         // Retry until either of the bit are set
            WAKE                          // Trigger wake up
            HALT                          // Stop the ULP program


.. only:: esp32

    **SLEEP** – 设置 ULP 唤醒计时器周期
    ---------------------------------------

    **语法**

      **SLEEP**   **sleep_reg**

    **操作数**

       - **sleep_reg** – 0..4，选择一个 ``SENS_ULP_CP_SLEEP_CYCx_REG`` 寄存器

    **周期**

      执行需要 2 个周期，获取下一条指令需要 4 个周期

    **描述**

      该指令决定 ULP 唤醒计时器使用哪一个 ``SENS_ULP_CP_SLEEP_CYCx_REG`` (x = 0..4) 寄存器值作为唤醒周期。默认情况下，使用 ``SENS_ULP_CP_SLEEP_CYC0_REG`` 的值

    **示例**::

      1:        SLEEP     1         // Use period set in SENS_ULP_CP_SLEEP_CYC1_REG

      2:        .set sleep_reg, 4   // Set constant
                SLEEP  sleep_reg    // Use period set in SENS_ULP_CP_SLEEP_CYC4_REG


**WAIT** – 等待一定的周期数
-------------------------------------

**语法**

   **WAIT**   **Cycles**

**操作数**

  - **Cycles** – 等待的周期数

**周期**

  执行需要 (2 + **Cycles**) 个周期，获取下一条指令需要 4 个周期

**描述**

  该指令指示延迟一定的周期数。

**示例**::

  1:        WAIT     10         // Do nothing for 10 cycles

  2:        .set  wait_cnt, 10  // Set a constant
            WAIT  wait_cnt      // wait for 10 cycles

.. only:: not esp32

    **TSENS** – 使用温度传感器进行测量
    --------------------------------------------

    **语法**

      - **TSENS**   **Rdst, Wait_Delay**

    **操作数**

      - **Rdst** – 目标寄存器 R[0..3]，结果将存储到此寄存器
      - **Wait_Delay** – 执行测量所需的周期数

    **周期**

      执行需要（2 + **Wait_Delay** + 3 * TSENS_CLK）个周期，获取下一条指令需要 4 个周期

    **描述**

      该指令使用 TSENS 进行测量，并将结果存储到通用寄存器

    **示例**::

      1:        TSENS     R1, 1000     // Measure temperature sensor for 1000 cycles,
                                      // and store result to R1


**ADC** – 使用 ADC 进行测量
---------------------------------

**语法**

  - **ADC**   **Rdst, Sar_sel, Mux**

  - **ADC**   **Rdst, Sar_sel, Mux, 0** — 形式已弃用

**操作数**

  - **Rdst** – 目标寄存器 R[0..3]，结果将存储到此寄存器
  - **Sar_sel** – 选择 ADC：0 = SARADC1，1 = SARADC2

.. only:: esp32

    - **Mux** - 启用 ADC 通道，通道号为 [Mux-1]。如果传递了 Mux 值 1，会使用 ADC 通道 0。

.. only:: esp32s2 or esp32s3

    - **Mux** - 选择的 PAD，SARADC Pad[Mux-1] 被启用。如果传递了 Mux 值 1，会使用 ADC pad 0。

**周期**

  执行需要 ``23 + max(1, SAR_AMP_WAIT1) + max(1, SAR_AMP_WAIT2) + max(1, SAR_AMP_WAIT3) + SARx_SAMPLE_CYCLE + SARx_SAMPLE_BIT`` 个周期，获取下一条指令需要 4 个周期

**描述**

  该指令通过 ADC 进行测量

.. only:: esp32

    **示例** ::

        1:        ADC      R1, 0, 1      // Measure value using ADC1 channel 0 and store result into R1

.. only:: esp32s2 or esp32s3

    **示例** ::

        1:        ADC      R1, 0, 1      // Measure value using ADC1 pad 2 and store result into R1

.. only:: esp32

    **I2C_RD** - 从 I2C 从机中读取单字节
    ----------------------------------------------

    **语法**

      - **I2C_RD**   **Sub_addr, High, Low, Slave_sel**

    **操作数**

      - **Sub_addr** – 要从 I2C 从机中读取的地址
      - **High*, *Low** — 定义要读取的位的范围。[High, Low] 以外的位会被屏蔽
      - **Slave_sel**  -  要使用的 I2C 从机地址的索引

    **周期**

      执行时间主要取决于 I2C 通信时间。获取下一条指令需要 4 个周期

    **描述**

      ``I2C_RD`` 指令从索引为 ``Slave_sel`` 的 I2C 从机读取一个字节。从机地址（格式为 7 位）必须预先设置到寄存器字段 ``SENS_I2C_SLAVE_ADDRx`` 中，其中 ``x == Slave_sel``。 8 位读取结果存储到 ``R0`` 寄存器。

    **示例**::

        1:        I2C_RD      0x10, 7, 0, 0      // Read byte from sub-address 0x10 of slave with address set in SENS_I2C_SLAVE_ADDR0


    **I2C_WR** - 向 I2C 从机写入单字节
    -----------------------------------------

    **语法**

      - **I2C_WR**   **Sub_addr, Value, High, Low, Slave_sel**

    **操作数**

      - **Sub_addr** –  I2C 从机内要写入的地址
      - **Value** – 要写入的 8 位值
      - **High**, **Low** — 定义要写入的位范围。[High, Low] 范围外的位会被屏蔽
      - **Slave_sel**  -  要使用的 I2C 从机地址的索引

    **周期**

      执行时间主要取决于 I2C 通信时间。获取下一条指令需要 4 个周期

    **描述**

      ``I2C_WR`` 指令向索引为 ``Slave_sel`` 的 I2C 从机地址写入一个字节。从机地址（格式为 7 位）必须预先设置到寄存器字段 ``SENS_I2C_SLAVE_ADDRx`` 中，其中 ``x == Slave_sel``。

    **示例**::

        1:        I2C_WR      0x20, 0x33, 7, 0, 1      // Write byte 0x33 to sub-address 0x20 of slave with address set in SENS_I2C_SLAVE_ADDR1.


**REG_RD** – 从外设寄存器读取
------------------------------------------

**语法**

   **REG_RD**   **Addr, High, Low**

**操作数**

  - **Addr** – 寄存器地址，以 32 位字为单位
  - **High** – 寄存器结束位号
  - **Low** – 寄存器起始位号

**周期**

  执行需要 4 个周期，获取下一条指令需要 4 个周期

**描述**

  该指令从外设寄存器读取最多 16 位到一个通用寄存器：``R0 = REG[Addr][High:Low]``。

.. only:: esp32

  该指令可以访问 RTC_CNTL、RTC_IO、SENS 和 RTC_I2C 外设中的寄存器。从 ULP 中看到的寄存器地址可以根据 DPORT 总线上相同寄存器的地址计算得出，如下所示::

    addr_ulp = (addr_dport - DR_REG_RTCCNTL_BASE) / 4

.. only:: esp32s2 or esp32s3

  该指令可以访问 RTC_CNTL、RTC_IO、SENS 和 RTC_I2C 外设中的寄存器。从 ULP 看到的寄存器地址可以根据 PeriBUS1 总线上相同寄存器的地址计算得出，如下所示::

    addr_ulp = (addr_peribus1 - DR_REG_RTCCNTL_BASE) / 4

**示例**::

  1:        REG_RD      0x120, 7, 4     // load 4 bits: R0 = {12'b0, REG[0x120][7:4]}


**REG_WR** – 写入外设寄存器
-----------------------------------------

**语法**

  **REG_WR**   **Addr, High, Low, Data**

**操作数**

  - **Addr** – 寄存器地址，以 32 位字为单位
  - **High** – 寄存器结束位号
  - **Low** – 寄存器起始位号
  - **Data** – 要写入的值，8 位

**周期**

  执行需要 8 个周期，获取下一条指令需要 4 个周期

**描述**

  该指令将一个立即数的最多 8 位写入到外设寄存器中：``REG[Addr][High:Low] = data``。

  .. only:: esp32

    此指令可以访问 RTC_CNTL、RTC_IO、SENS 和 RTC_I2C 外设中的寄存器。从 ULP 看到的寄存器地址可以根据 DPORT 总线上同一寄存器的地址计算得出，如下所示::

      addr_ulp = (addr_dport - DR_REG_RTCCNTL_BASE) / 4

  .. only:: esp32s2 or esp32s3

    此指令可以访问 RTC_CNTL 、 RTC_IO 、 SENS 和 RTC_I2C 外设中的寄存器。从 ULP 看到的寄存器地址可以根据 PeriBUS1 上同一寄存器的地址按如下方式计算::

      addr_ulp = (addr_peribus1 - DR_REG_RTCCNTL_BASE) / 4

**示例**::

    1:        REG_WR      0x120, 7, 0, 0x10   // set 8 bits: REG[0x120][7:0] = 0x10


方便的外设寄存器访问宏
--------------------------------------------------

ULP 源文件在进入汇编程序之前先通过 C 预处理器，因此可以使用某些宏来方便地访问外设寄存器。

一些现有的宏定义在 ``soc/soc_ulp.h`` 头文件中，这些宏允许通过的名称访问外设寄存器的字段。可以通过这些宏使用的外设寄存器名称定义在 ``soc/rtc_cntl_reg.h``、``soc/rtc_io_reg.h``、``soc/sens_reg.h`` 和 ``soc/rtc_i2c_reg.h`` 中。

READ_RTC_REG(rtc_reg, low_bit, bit_width)
  将 rtc_reg[low_bit + bit_width - 1 : low_bit] 中的数读到 R0，最多 16 位。如::

    #include "soc/soc_ulp.h"
    #include "soc/rtc_cntl_reg.h"

    /* 将 RTC_CNTL_TIME0_REG 的低 16 位读入 R0 */
    READ_RTC_REG(RTC_CNTL_TIME0_REG, 0, 16)

READ_RTC_FIELD(rtc_reg, field)
  将 rtc_reg 的一个字段读取到 R0，最多 16 位。如::

    #include "soc/soc_ulp.h"
    #include "soc/sens_reg.h"

    /* 将 SENS_SAR_SLAVE_ADDR3_REG 的 8 位 SENS_TSENS_OUT 字段读入 R0 */
    READ_RTC_FIELD(SENS_SAR_SLAVE_ADDR3_REG, SENS_TSENS_OUT)

WRITE_RTC_REG(rtc_reg, low_bit, bit_width, value)
  将立即数写入 rtc_reg[low_bit + bit_width - 1 : low_bit]，最多 8 位。如::

    #include "soc/soc_ulp.h"
    #include "soc/rtc_io_reg.h"

    /* 设置 RTC_GPIO_OUT_W1TS_REG 中 RTC_GPIO_OUT_DATA_W1TS 字段的 BIT(2) */
    WRITE_RTC_REG(RTC_GPIO_OUT_W1TS_REG, RTC_GPIO_OUT_DATA_W1TS_S + 2, 1, 1)

WRITE_RTC_FIELD(rtc_reg, field, value)
  将立即数写入 rtc_reg 的一个字段，最多 8 位。如::

    #include "soc/soc_ulp.h"
    #include "soc/rtc_cntl_reg.h"

    /* 将 RTC_CNTL_STATE0_REG 的 RTC_CNTL_ULP_CP_SLP_TIMER_EN 字段设置为 0 */
    WRITE_RTC_FIELD(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN, 0)
