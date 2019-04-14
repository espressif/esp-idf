#include "soc/lldesc.h"

void lldesc_setup_link(lldesc_t *dmadesc, const void *data, int len, bool isrx)
{
    int n = 0;
    while (len) {
        int dmachunklen = len;
        if (dmachunklen > LLDESC_MAX_NUM_PER_DESC) {
            dmachunklen = LLDESC_MAX_NUM_PER_DESC;
        }
        if (isrx) {
            //Receive needs DMA length rounded to next 32-bit boundary
            dmadesc[n].size = (dmachunklen + 3) & (~3);
            dmadesc[n].length = (dmachunklen + 3) & (~3);
        } else {
            dmadesc[n].size = dmachunklen;
            dmadesc[n].length = dmachunklen;
        }
        dmadesc[n].buf = (uint8_t *)data;
        dmadesc[n].eof = 0;
        dmadesc[n].sosf = 0;
        dmadesc[n].owner = 1;
        dmadesc[n].qe.stqe_next = &dmadesc[n + 1];
        len -= dmachunklen;
        data += dmachunklen;
        n++;
    }
    dmadesc[n - 1].eof = 1; //Mark last DMA desc as end of stream.
    dmadesc[n - 1].qe.stqe_next = NULL;
}
