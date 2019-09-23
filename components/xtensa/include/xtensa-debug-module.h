#ifndef XTENSA_DEBUG_MODULE_H
#define XTENSA_DEBUG_MODULE_H

/*
ERI registers / OCD offsets and field definitions
*/

#define ERI_DEBUG_OFFSET 0x100000

#define ERI_TRAX_OFFSET         (ERI_DEBUG_OFFSET+0)
#define ERI_PERFMON_OFFSET      (ERI_DEBUG_OFFSET+0x1000)
#define ERI_OCDREG_OFFSET       (ERI_DEBUG_OFFSET+0x2000)
#define ERI_MISCDBG_OFFSET      (ERI_DEBUG_OFFSET+0x3000)
#define ERI_CORESIGHT_OFFSET    (ERI_DEBUG_OFFSET+0x3F00)

#define ERI_TRAX_TRAXID         (ERI_TRAX_OFFSET+0x00)
#define ERI_TRAX_TRAXCTRL       (ERI_TRAX_OFFSET+0x04)
#define ERI_TRAX_TRAXSTAT       (ERI_TRAX_OFFSET+0x08)
#define ERI_TRAX_TRAXDATA       (ERI_TRAX_OFFSET+0x0C)
#define ERI_TRAX_TRAXADDR       (ERI_TRAX_OFFSET+0x10)
#define ERI_TRAX_TRIGGERPC      (ERI_TRAX_OFFSET+0x14)
#define ERI_TRAX_PCMATCHCTRL    (ERI_TRAX_OFFSET+0x18)
#define ERI_TRAX_DELAYCNT       (ERI_TRAX_OFFSET+0x1C)
#define ERI_TRAX_MEMADDRSTART   (ERI_TRAX_OFFSET+0x20)
#define ERI_TRAX_MEMADDREND     (ERI_TRAX_OFFSET+0x24)

#define TRAXCTRL_TREN           (1<<0)  //Trace enable. Tracing starts on 0->1
#define TRAXCTRL_TRSTP          (1<<1)  //Trace Stop. Make 1 to stop trace.
#define TRAXCTRL_PCMEN          (1<<2)  //PC match enable
#define TRAXCTRL_PTIEN          (1<<4)  //Processor-trigger enable
#define TRAXCTRL_CTIEN          (1<<5)  //Cross-trigger enable
#define TRAXCTRL_TMEN           (1<<7)  //Tracemem Enable. Always set.
#define TRAXCTRL_CNTU           (1<<9)  //Post-stop-trigger countdown units; selects when DelayCount-- happens.
                                        //0 - every 32-bit word written to tracemem, 1 - every cpu instruction
#define TRAXCTRL_TSEN           (1<<11) //Undocumented/deprecated?
#define TRAXCTRL_SMPER_SHIFT    12      //Send sync every 2^(9-smper) messages. 7=reserved, 0=no sync msg
#define TRAXCTRL_SMPER_MASK     0x7     //Synchronization message period
#define TRAXCTRL_PTOWT          (1<<16) //Processor Trigger Out (OCD halt) enabled when stop triggered
#define TRAXCTRL_PTOWS          (1<<17) //Processor Trigger Out (OCD halt) enabled when trace stop completes
#define TRAXCTRL_CTOWT          (1<<20) //Cross-trigger Out enabled when stop triggered
#define TRAXCTRL_CTOWS          (1<<21) //Cross-trigger Out enabled when trace stop completes
#define TRAXCTRL_ITCTO          (1<<22) //Integration mode: cross-trigger output
#define TRAXCTRL_ITCTIA         (1<<23) //Integration mode: cross-trigger ack
#define TRAXCTRL_ITATV          (1<<24) //replaces ATID when in integration mode: ATVALID output
#define TRAXCTRL_ATID_MASK      0x7F    //ARB source ID
#define TRAXCTRL_ATID_SHIFT     24
#define TRAXCTRL_ATEN           (1<<31) //ATB interface enable

#define TRAXSTAT_TRACT          (1<<0)  //Trace active flag.
#define TRAXSTAT_TRIG           (1<<1)  //Trace stop trigger. Clears on TREN 1->0
#define TRAXSTAT_PCMTG          (1<<2)  //Stop trigger caused by PC match. Clears on TREN 1->0
#define TRAXSTAT_PJTR           (1<<3)  //JTAG transaction result. 1=err in preceding jtag transaction.
#define TRAXSTAT_PTITG          (1<<4)  //Stop trigger caused by Processor Trigger Input. Clears on TREN 1->0
#define TRAXSTAT_CTITG          (1<<5)  //Stop trigger caused by Cross-Trigger Input. Clears on TREN 1->0
#define TRAXSTAT_MEMSZ_SHIFT    8       //Traceram size inducator. Usable trace ram is 2^MEMSZ bytes.
#define TRAXSTAT_MEMSZ_MASK     0x1F
#define TRAXSTAT_PTO            (1<<16) //Processor Trigger Output: current value
#define TRAXSTAT_CTO            (1<<17) //Cross-Trigger Output: current value
#define TRAXSTAT_ITCTOA         (1<<22) //Cross-Trigger Out Ack: current value
#define TRAXSTAT_ITCTI          (1<<23) //Cross-Trigger Input: current value
#define TRAXSTAT_ITATR          (1<<24) //ATREADY Input: current value

#define TRAXADDR_TADDR_SHIFT    0       //Trax memory address, in 32-bit words.
#define TRAXADDR_TADDR_MASK     0x1FFFFF //Actually is only as big as the trace buffer size max addr.
#define TRAXADDR_TWRAP_SHIFT    21      //Amount of times TADDR has overflown
#define TRAXADDR_TWRAP_MASK     0x3FF
#define TRAXADDR_TWSAT          (1<<31) //1 if TWRAP has overflown, clear by disabling tren.

#define PCMATCHCTRL_PCML_SHIFT  0       //Amount of lower bits to ignore in pc trigger register
#define PCMATCHCTRL_PCML_MASK   0x1F 
#define PCMATCHCTRL_PCMS        (1<<31) //PC Match Sense, 0 - match when procs PC is in-range, 1 - match when
                                        //out-of-range


#endif