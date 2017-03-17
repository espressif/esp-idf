#include "../ff.h"

#if _USE_LFN != 0

#if   _CODE_PAGE == 932	/* Japanese Shift_JIS */
#include "cc932.c.inc"
#elif _CODE_PAGE == 936	/* Simplified Chinese GBK */
#include "cc936.c.inc"
#elif _CODE_PAGE == 949	/* Korean */
#include "cc949.c.inc"
#elif _CODE_PAGE == 950	/* Traditional Chinese Big5 */
#include "cc950.c.inc"
#else					/* Single Byte Character-Set */
#include "ccsbcs.c.inc"
#endif

#endif
