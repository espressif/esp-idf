#define ASSIST_DEBUG_BASE DR_REG_ASSIST_DEBUG_BASE

#define ASSIST_DEBUG_INTERRUPT_ENA              ((ASSIST_DEBUG_BASE) +0x00)
#define ASSIST_DEBUG_INTERRUPT_RAW              ((ASSIST_DEBUG_BASE) +0x04)
#define ASSIST_DEBUG_INTERRUPT_RLS              ((ASSIST_DEBUG_BASE) +0x08)
#define ASSIST_DEBUG_INTERRUPT_CLR              ((ASSIST_DEBUG_BASE) +0x0C)
#define ASSIST_DEBUG_INTERRUPT_AREA_0_TOUCH_PRO_RD                   BIT(0)
#define ASSIST_DEBUG_INTERRUPT_AREA_0_TOUCH_PRO_WR                   BIT(1)
#define ASSIST_DEBUG_INTERRUPT_AREA_1_TOUCH_PRO_RD                   BIT(2)
#define ASSIST_DEBUG_INTERRUPT_AREA_1_TOUCH_PRO_WR                   BIT(3)
#define ASSIST_DEBUG_INTERRUPT_PRO_SP_SPILL_MIN                      BIT(4)
#define ASSIST_DEBUG_INTERRUPT_PRO_SP_SPILL_MAX                      BIT(5)


#define ASSIST_DEBUG_AREA_0_MIN                 ((ASSIST_DEBUG_BASE) +0x10)
#define ASSIST_DEBUG_AREA_0_MAX                 ((ASSIST_DEBUG_BASE) +0x14)
#define ASSIST_DEBUG_AREA_0_PC                  ((ASSIST_DEBUG_BASE) +0x18)
#define ASSIST_DEBUG_AREA_0_SP                  ((ASSIST_DEBUG_BASE) +0x1C)

#define ASSIST_DEBUG_AREA_1_MIN                 ((ASSIST_DEBUG_BASE) +0x20)
#define ASSIST_DEBUG_AREA_1_MAX                 ((ASSIST_DEBUG_BASE) +0x24)
#define ASSIST_DEBUG_AREA_1_PC                  ((ASSIST_DEBUG_BASE) +0x28)
#define ASSIST_DEBUG_AREA_1_SP                  ((ASSIST_DEBUG_BASE) +0x2C)

#define ASSIST_DEBUG_PRO_SP_UNSTABLE            ((ASSIST_DEBUG_BASE) +0x30)
#define ASSIST_DEBUG_PRO_SP_MIN                 ((ASSIST_DEBUG_BASE) +0x34)
#define ASSIST_DEBUG_PRO_SP_MAX                 ((ASSIST_DEBUG_BASE) +0x38)
#define ASSIST_DEBUG_PRO_SP_PC                  ((ASSIST_DEBUG_BASE) +0x3C)

#define ASSIST_DEBUG_PRO_PDEBUGENABLE		((ASSIST_DEBUG_BASE) +0x40)
#define ASSIST_DEBUG_PRO_RCD_RECORDING		((ASSIST_DEBUG_BASE) +0x44)
#define ASSIST_DEBUG_PRO_RCD_PDEBUGINST		((ASSIST_DEBUG_BASE) +0x48)
/* register layout:
 * SIZE [7..0]           : Instructions normally complete in the W stage. The size of the instruction in the W is given
 *                         by this field in number of bytes. If it is 8’b0 in a given cycle the W stage has no completing
 *                         instruction. This is also known as a bubble cycle. Also see DPORT_PRO_CPU_RECORD_PDEBUGSTATUS_REG.
 * ISRC [14..12]         : Instruction source.
** LOOP [23..20]         : Loopback status.
** CINTLEVEL [27..24]: CINTLEVEL.
*/
#define DPORT_RECORD_PDEBUGINST_SZ_M            ((DPORT_RECORD_PDEBUGINST_SZ_V)<<(DPORT_RECORD_PDEBUGINST_SZ_S))
#define DPORT_RECORD_PDEBUGINST_SZ_V            0xFF
#define DPORT_RECORD_PDEBUGINST_SZ_S            0
#define DPORT_RECORD_PDEBUGINST_SZ(_r_)         (((_r_)>>DPORT_RECORD_PDEBUGINST_SZ_S) & DPORT_RECORD_PDEBUGINST_SZ_V)
#define DPORT_RECORD_PDEBUGINST_ISRC_M          ((DPORT_RECORD_PDEBUGINST_ISRC_V)<<(DPORT_RECORD_PDEBUGINST_ISRC_S))
#define DPORT_RECORD_PDEBUGINST_ISRC_V          0x07
#define DPORT_RECORD_PDEBUGINST_ISRC_S          12
#define DPORT_RECORD_PDEBUGINST_ISRC(_r_)       (((_r_)>>DPORT_RECORD_PDEBUGINST_ISRC_S) & DPORT_RECORD_PDEBUGINST_ISRC_V)
// #define DPORT_RECORD_PDEBUGINST_LOOP_M       ((DPORT_RECORD_PDEBUGINST_LOOP_V)<<(DPORT_RECORD_PDEBUGINST_LOOP_S))
// #define DPORT_RECORD_PDEBUGINST_LOOP_V       0x0F
// #define DPORT_RECORD_PDEBUGINST_LOOP_S       20
// #define DPORT_RECORD_PDEBUGINST_LOOP(_r_)    (((_r_)>>DPORT_RECORD_PDEBUGINST_LOOP_S) & DPORT_RECORD_PDEBUGINST_LOOP_V)
#define DPORT_RECORD_PDEBUGINST_LOOP_REP        (BIT(20)) /* loopback will occur */
#define DPORT_RECORD_PDEBUGINST_LOOP            (BIT(21)) /* last inst of loop */
#define DPORT_RECORD_PDEBUGINST_CINTL_M         ((DPORT_RECORD_PDEBUGINST_CINTL_V)<<(DPORT_RECORD_PDEBUGINST_CINTL_S))
#define DPORT_RECORD_PDEBUGINST_CINTL_V         0x0F
#define DPORT_RECORD_PDEBUGINST_CINTL_S         24
#define DPORT_RECORD_PDEBUGINST_CINTL(_r_)      (((_r_)>>DPORT_RECORD_PDEBUGINST_CINTL_S) & DPORT_RECORD_PDEBUGINST_CINTL_V)

#define ASSIST_DEBUG_PRO_RCD_PDEBUGSTATUS	((ASSIST_DEBUG_BASE) +0x4C)
/* register layout:
 * BBCAUSE [5..0]: Indicates cause for bubble cycle. See below for posible values. When DPORT_RECORD_PDEBUGINST_SZ == 0
 * INSNTYPE[5..0]: Indicates type of instruction retiring in the W stage. See below for posible values. When DPORT_RECORD_PDEBUGINST_SZ > 0
*/
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_M             ((DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_V)<<(DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_S))
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_V             0x3F
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_S             0
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE(_r_)          (((_r_)>>DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_S) & DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_V)
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_PSO           0x00 /* Power shut off */
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_DEP           0x02 /* Register dependency or resource conflict. See DPORT_XXX_CPU_RECORD_PDEBUGDATA_REG for extra info. */
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_CTL           0x04 /* Control transfer bubble */
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_ICM           0x08 /* I-cache miss (incl uncached miss) */
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_DCM           0x0C /* D-cache miss (excl uncached miss) */
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_EXC0          0x10 /* Exception or interrupt (W stage). See DPORT_XXX_CPU_RECORD_PDEBUGDATA_REG for extra info.
                                                                                                                        The virtual address of the instruction that was killed appears on DPORT_PRO_CPU_RECORD_PDEBUGPC_REG[31:0] */
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_EXC1          0x11 /* Exception or interrupt (W+1 stage). See DPORT_XXX_CPU_RECORD_PDEBUGDATA_REG for extra info. */
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_RPL           0x14 /* Instruction replay (other). DPORT_XXX_CPU_RECORD_PDEBUGDATA_REG has the PC of the replaying instruction. */
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_ITLB          0x18 /* HW ITLB refill. The refill address and data are available on
                                                                                                                        DPORT_PRO_CPU_RECORD_PDEBUGLS0ADDR_REG and DPORT_PRO_CPU_RECORD_PDEBUGLS0DATA_REG. */
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_ITLBM         0x1A /* ITLB miss */
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_DTLB          0x1C /* HW DTLB refill. The refill address and data are available on
                                                                                                                        DPORT_PRO_CPU_RECORD_PDEBUGLS0ADDR_REG and DPORT_PRO_CPU_RECORD_PDEBUGLS0DATA_REG. */
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_DTLBM         0x1E /* DTLB miss */
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_STALL         0x20 /* Stall . The cause of the global stall is further classified in the DPORT_XXX_CPU_RECORD_PDEBUGDATA_REG. */
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_HWMEC         0x24 /* HW-corrected memory error */
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_WAITI         0x28 /* WAITI mode */
#define DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_OTHER         0x3C /* all other bubbles */
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_M            ((DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_V)<<(DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_S))
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_V            0x3F
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_S            0
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE(_r_)         (((_r_)>>DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_S) & DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_V)
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_JX           0x00 /* JX */
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_CALLX        0x04 /* CALLX */
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_CRET         0x08 /* All call returns */
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_ERET         0x0C /* All exception returns */
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_B            0x10 /* Branch taken or loop not taken */
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_J            0x14 /* J */
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_CALL         0x18 /* CALL */
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_BN           0x1C /* Branch not taken */
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_LOOP         0x20 /* Loop instruction (taken) */
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_S32C1I       0x24 /* S32C1I. The address and load data (before the conditional store) are available on the LS signals*/
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_WXSR2LB      0x28 /* WSR/XSR to LBEGIN */
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_WSR2MMID     0x2C /* WSR to MMID */
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_RWXSR        0x30 /* RSR or WSR (except MMID and LBEGIN) or XSR (except LBEGIN) */
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_RWER         0x34 /* RER or WER */
#define DPORT_RECORD_PDEBUGSTATUS_INSNTYPE_DEF          0x3C /* Default */


#define ASSIST_DEBUG_PRO_RCD_PDEBUGDATA		((ASSIST_DEBUG_BASE) +0x50)
#define ASSIST_DEBUG_PRO_RCD_PDEBUGPC		((ASSIST_DEBUG_BASE) +0x54)
#define ASSIST_DEBUG_PRO_RCD_PDEBUGLS0STAT	((ASSIST_DEBUG_BASE) +0x58)
#define ASSIST_DEBUG_PRO_RCD_PDEBUGLS0ADDR	((ASSIST_DEBUG_BASE) +0x5C)
#define ASSIST_DEBUG_PRO_RCD_PDEBUGLS0DATA	((ASSIST_DEBUG_BASE) +0x60)
#define ASSIST_DEBUG_PRO_RCD_SP			((ASSIST_DEBUG_BASE) +0x64)

