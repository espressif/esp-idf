#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include <xtensa/simcall.h>
#include "esp_panic.h"
#include "sdkconfig.h"
#include "soc/soc.h"


#if CONFIG_SPIRAM_CACHE_WORKAROUND_TEST

    .section .iram1,"ax"
    .global     xt_highint5
    .type       xt_highint5,@function
    .align      4
xt_highint5:

    esync
    rsr a0, CCOUNT
    addi a0, a0, 40
    wsr a0, CCOMPARE2
    esync

.L_xt_highint5_exit:
    rsr     a0, EXCSAVE_5                   /* restore a0 */
    rfi     5


#endif

/* The linker has no reason to link in this file; all symbols it exports are already defined
   (weakly!) in the default int handler. Define a symbol here so we can use it to have the 
   linker inspect this anyway. */

    .global ld_include_psram_tst
ld_include_psram_tst:
