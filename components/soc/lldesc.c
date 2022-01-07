#include "soc/lldesc.h"

void lldesc_setup_link_constrained(lldesc_t *dmadesc, const void *data, int len, int max_desc_size, bool isrx)
{
    int n = 0;
    while (len) {
        int dmachunklen = len;
        if (dmachunklen > max_desc_size) {
            dmachunklen = max_desc_size;
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

int lldesc_get_received_len(lldesc_t* head, lldesc_t** out_next)
{
    lldesc_t* desc = head;
    int len = 0;
    while(desc) {
        len += desc->length;
        bool eof = desc->eof;
        desc = STAILQ_NEXT(desc, qe);
        if (eof) break;
    }
    if (out_next) {
        *out_next = desc;
    }
    return len;
}
