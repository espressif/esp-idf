/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft, 2019 William D. Jones for Adafruit Industries
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 * Additions Copyright (c) 2020, Espressif Systems (Shanghai) Co. Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#include "device_controller_driver.h"
#include "esp_rom_sys.h"

static const char *TAG = "TUSB:DCD";
static intr_handle_t usb_ih;
static volatile TU_ATTR_ALIGNED(4) uint32_t _setup_packet[6];
static uint8_t s_setup_phase = 0; /*  00 - got setup,
                                    01 - got done setup,
                                    02 - setup cmd sent*/

#define XFER_CTL_BASE(_ep, _dir) &xfer_status[_ep][_dir]
static xfer_ctl_t xfer_status[USB_MAX_EP_NUM][USB_EP_DIRECTIONS];

static inline void readyfor1setup_pkg(int ep_num)
{
    USB0.out_ep_reg[ep_num].doeptsiz |= (1 << USB_SUPCNT0_S); // doeptsiz 29:30 will decremented on every setup received
}

// Setup the control endpoint 0.
static void bus_reset(void)
{

    for (int ep_num = 0; ep_num < USB_OUT_EP_NUM; ep_num++) {
        USB0.out_ep_reg[ep_num].doepctl |= USB_DO_SNAK0_M; // DOEPCTL0_SNAK
    }

    USB0.dcfg &= ~USB_DEVADDR_M; // reset address

    // Peripheral FIFO architecture
    //
    // --------------- 320 ( 1280 bytes )
    // | IN FIFO 3  |
    // --------------- y + x + 16 + GRXFSIZ
    // | IN FIFO 2  |
    // --------------- x + 16 + GRXFSIZ
    // | IN FIFO 1  |
    // --------------- 16 + GRXFSIZ
    // | IN FIFO 0  |
    // --------------- GRXFSIZ
    // | OUT FIFO   |
    // | ( Shared ) |
    // --------------- 0
    //
    // FIFO sizes are set up by the following rules (each word 32-bits):
    // All EP OUT shared a unique OUT FIFO which uses (based on page 1354 of Rev 17 of reference manual):
    // * 10 locations in hardware for setup packets + setup control words
    // (up to 3 setup packets).
    // * 2 locations for OUT endpoint control words.
    // * 16 for largest packet size of 64 bytes. ( TODO Highspeed is 512 bytes)
    // * 1 location for global NAK (not required/used here).
    //
    // It is recommended to allocate 2 times the largest packet size, therefore
    // Recommended value = 10 + 1 + 2 x (16+2) = 47 --> Let's make it 50
    USB0.grstctl |= 0x10 << USB_TXFNUM_S; // fifo 0x10,
    USB0.grstctl |= USB_TXFFLSH_M;        // Flush fifo
    USB0.grxfsiz = 50;

    USB0.gintmsk = USB_MODEMISMSK_M |
                   USB_SOFMSK_M |
                   USB_RXFLVIMSK_M |
                   USB_ERLYSUSPMSK_M |
                   USB_USBSUSPMSK_M |
                   USB_USBRSTMSK_M |
                   USB_ENUMDONEMSK_M |
                   USB_IEPINTMSK_M |
                   USB_OEPINTMSK_M |
                   USB_RESETDETMSK_M |
                   USB_DISCONNINTMSK_M;

    USB0.daintmsk |= USB_OUTEPMSK0_M | USB_INEPMSK0_M;
    USB0.doepmsk |= USB_SETUPMSK_M | USB_XFERCOMPLMSK;
    USB0.diepmsk |= USB_TIMEOUTMSK_M | USB_DI_XFERCOMPLMSK_M;

    USB0.gnptxfsiz = 16 << USB_NPTXFDEP_S; // Control IN uses FIFO 0 with 64 bytes ( 16 32-bit word )

    readyfor1setup_pkg(0);
}

static void enum_done_processing(void)
{

    ESP_EARLY_LOGV(TAG, "dcd_irq_handler - Speed enumeration done! Sending DCD_EVENT_BUS_RESET then");
    // On current silicon on the Full Speed core, speed is fixed to Full Speed.
    // However, keep for debugging and in case Low Speed is ever supported.
    uint32_t enum_spd = (USB0.dsts >> USB_ENUMSPD_S) & (USB_ENUMSPD_V);

    // Maximum packet size for EP 0 is set for both directions by writing DIEPCTL
    if (enum_spd == 0x03) { // Full-Speed (PHY on 48 MHz)
        USB0.in_ep_reg[0].diepctl &= ~USB_D_MPS0_V;    // 64 bytes
        USB0.in_ep_reg[0].diepctl &= ~USB_D_STALL0_M; // clear Stall
        xfer_status[0][TUSB_DIR_OUT].max_size = 64;
        xfer_status[0][TUSB_DIR_IN].max_size = 64;
    } else {
        USB0.in_ep_reg[0].diepctl |= USB_D_MPS0_V;     // 8 bytes
        USB0.in_ep_reg[0].diepctl &= ~USB_D_STALL0_M; // clear Stall
        xfer_status[0][TUSB_DIR_OUT].max_size = 8;
        xfer_status[0][TUSB_DIR_IN].max_size = 8;
    }

    USB0.gintmsk |= USB_SOFMSK_M; // SOF unmask
}




/*------------------------------------------------------------------*/
/* Controller API
 *------------------------------------------------------------------*/
void dcd_init(uint8_t rhport)
{
    ESP_LOGV(TAG, "DCD init - Start");

    // A. Disconnect
    ESP_LOGV(TAG, "DCD init - Soft DISCONNECT and Setting up");
    USB0.dctl |= USB_SFTDISCON_M; // Soft disconnect

    // B. Programming DCFG
    /* If USB host misbehaves during status portion of control xfer
    (non zero-length packet), send STALL back and discard. Full speed. */
    USB0.dcfg |= USB_NZSTSOUTHSHK_M | // NonZero .... STALL
                 (3 << 0);            // dev speed: fullspeed 1.1 on 48 mhz  // TODO no value in usb_reg.h (IDF-1476)

    USB0.gahbcfg |= USB_NPTXFEMPLVL_M | USB_GLBLLNTRMSK_M; // Global interruptions ON
    USB0.gusbcfg |= USB_FORCEDEVMODE_M;                    // force devmode

    USB0.gotgctl &= ~(USB_BVALIDOVVAL_M | USB_BVALIDOVEN_M | USB_VBVALIDOVVAL_M); //no overrides
#ifdef CONFIG_IDF_TARGET_ESP32S2BETA // needed for beta chip only
    //C. chip 7.2.2 hack
    ESP_LOGV(TAG, "DCD init - chip ESP32-S2 beta hack");
    USB0.gotgctl = (0 << USB_BVALIDOVVAL_S); //B override value
    esp_rom_delay_us(20);
    USB0.gotgctl = (0 << USB_BVALIDOVVAL_S) | (1 << USB_BVALIDOVEN_S); //B override value & enable
    esp_rom_delay_us(20);
#endif

    // C. Setting SNAKs, then connect
    for (int n = 0; n < USB_OUT_EP_NUM; n++) {
        USB0.out_ep_reg[n].doepctl |= USB_DO_SNAK0_M; // DOEPCTL0_SNAK
    }
    ESP_LOGV(TAG, "DCD init - Soft CONNECT");
    USB0.dctl &= ~USB_SFTDISCON_M; // Connect

    // D. Interruption masking
    USB0.gintmsk = 0;   //mask all
    USB0.gotgint = ~0U; //clear OTG ints
    USB0.gintsts = ~0U; //clear pending ints
    USB0.gintmsk = USB_MODEMISMSK_M |
                   USB_SOFMSK_M |
                   USB_RXFLVIMSK_M |
                   USB_ERLYSUSPMSK_M |
                   USB_USBSUSPMSK_M |
                   USB_USBRSTMSK_M |
                   USB_ENUMDONEMSK_M |
                   USB_RESETDETMSK_M |
                   USB_DISCONNINTMSK_M;
    esp_rom_delay_us(100);
}

void dcd_set_address(uint8_t rhport, uint8_t dev_addr)
{
    (void)rhport;
    ESP_LOGV(TAG, "DCD init - Set address : %u", dev_addr);
    USB0.dcfg |= ((dev_addr & USB_DEVADDR_V) << USB_DEVADDR_S);
    // Response with status after changing device address
    dcd_edpt_xfer(rhport, tu_edpt_addr(0, TUSB_DIR_IN), NULL, 0);
}

void dcd_set_config(uint8_t rhport, uint8_t config_num)
{
    (void)rhport;
    (void)config_num;
    // Nothing to do
}

void dcd_remote_wakeup(uint8_t rhport)
{
    (void)rhport;
}

/*------------------------------------------------------------------*/
/* DCD Endpoint port
 *------------------------------------------------------------------*/

bool dcd_edpt_open(uint8_t rhport, tusb_desc_endpoint_t const *desc_edpt)
{

    ESP_LOGV(TAG, "DCD endpoint opened");
    (void)rhport;

    usb_out_endpoint_t *out_ep = &(USB0.out_ep_reg[0]);
    usb_in_endpoint_t *in_ep = &(USB0.in_ep_reg[0]);

    uint8_t const epnum = tu_edpt_number(desc_edpt->bEndpointAddress);
    uint8_t const dir = tu_edpt_dir(desc_edpt->bEndpointAddress);

    // Unsupported endpoint numbers/size.
    if ((desc_edpt->wMaxPacketSize.size > 64) || (epnum > 3)) {
        return false;
    }

    xfer_ctl_t *xfer = XFER_CTL_BASE(epnum, dir);
    xfer->max_size = desc_edpt->wMaxPacketSize.size;

    if (dir == TUSB_DIR_OUT) {
        out_ep[epnum].doepctl |= USB_USBACTEP0_M |
                                 desc_edpt->bmAttributes.xfer << USB_EPTYPE0_S |
                                 desc_edpt->wMaxPacketSize.size << USB_MPS0_S;
        USB0.daintmsk |= (1 << (16 + epnum));
    } else {
        // Peripheral FIFO architecture (Rev18 RM 29.11)
        //
        // --------------- 320 ( 1280 bytes )
        // | IN FIFO 3  |
        // --------------- y + x + 16 + GRXFSIZ
        // | IN FIFO 2  |
        // --------------- x + 16 + GRXFSIZ
        // | IN FIFO 1  |
        // --------------- 16 + GRXFSIZ
        // | IN FIFO 0  |
        // --------------- GRXFSIZ
        // | OUT FIFO   |
        // | ( Shared ) |
        // --------------- 0
        //
        // Since OUT FIFO = 50, FIFO 0 = 16, average of FIFOx = (312-50-16) / 3 = 82 ~ 80
        in_ep[epnum].diepctl |= USB_D_USBACTEP1_M |
                                (epnum - 1) << USB_D_TXFNUM1_S |
                                desc_edpt->bmAttributes.xfer << USB_D_EPTYPE1_S |
                                (desc_edpt->bmAttributes.xfer != TUSB_XFER_ISOCHRONOUS ? (1 << USB_DI_SETD0PID1_S) : 0) |
                                desc_edpt->wMaxPacketSize.size << 0;
        USB0.daintmsk |= (1 << (0 + epnum));

        // Both TXFD and TXSA are in unit of 32-bit words
        uint16_t const fifo_size = 80;
        uint32_t const fifo_offset = (USB0.grxfsiz & USB_NPTXFDEP_V) + 16 + fifo_size * (epnum - 1);
        USB0.dieptxf[epnum - 1] = (80 << USB_NPTXFDEP_S) | fifo_offset;
    }
    return true;
}

bool dcd_edpt_xfer(uint8_t rhport, uint8_t ep_addr, uint8_t *buffer, uint16_t total_bytes)
{

    (void)rhport;

    uint8_t const epnum = tu_edpt_number(ep_addr);
    uint8_t const dir = tu_edpt_dir(ep_addr);

    xfer_ctl_t *xfer = XFER_CTL_BASE(epnum, dir);
    xfer->buffer = buffer;
    xfer->total_len = total_bytes;
    xfer->queued_len = 0;
    xfer->short_packet = false;

    uint16_t num_packets = (total_bytes / xfer->max_size);
    uint8_t short_packet_size = total_bytes % xfer->max_size;

    // Zero-size packet is special case.
    if (short_packet_size > 0 || (total_bytes == 0)) {
        num_packets++;
    }

    ESP_LOGV(TAG, "Transfer <-> EP%i, %s, pkgs: %i, bytes: %i",
             epnum, ((dir == TUSB_DIR_IN) ? "USB0.HOST (in)" : "HOST->DEV (out)"),
             num_packets, total_bytes);

    // IN and OUT endpoint xfers are interrupt-driven, we just schedule them
    // here.
    if (dir == TUSB_DIR_IN) {
        // A full IN transfer (multiple packets, possibly) triggers XFRC.
        int bytes2fifo_left = total_bytes;
        uint32_t val; // 32 bit val from 4 buff addresses

        USB0.in_ep_reg[epnum].diepint = ~0U; // clear all ints
        USB0.in_ep_reg[epnum].dieptsiz = (num_packets << USB_D_PKTCNT0_S) | total_bytes;
        USB0.in_ep_reg[epnum].diepctl |= USB_D_EPENA1_M | USB_D_CNAK1_M; // Enable | CNAK
        while (bytes2fifo_left > 0) {  // TODO move it to ep_in_handle (IDF-1475)
            /* ATTENTION! In cases when CFG_TUD_ENDOINT0_SIZE, CFG_TUD_CDC_EPSIZE, CFG_TUD_MIDI_EPSIZE or
            CFG_TUD_MSC_BUFSIZE < 4 next line can be a cause of an error.*/
            val = (*(buffer + 3) << 24) |
                  (*(buffer + 2) << 16) |
                  (*(buffer + 1) << 8) |
                  (*(buffer + 0) << 0);
            ESP_LOGV(TAG, "Transfer 0x%08x -> FIFO%d", val, epnum);
            USB0.fifo[epnum][0] = val; //copy and next buffer address
            buffer += 4;
            bytes2fifo_left -= 4;
        }
        // USB0.dtknqr4_fifoemptymsk |= (1 << epnum);
    } else {
        // Each complete packet for OUT xfers triggers XFRC.
        USB0.out_ep_reg[epnum].doeptsiz = USB_PKTCNT0_M |
                                          ((xfer->max_size & USB_XFERSIZE0_V) << USB_XFERSIZE0_S);
        USB0.out_ep_reg[epnum].doepctl |= USB_EPENA0_M | USB_CNAK0_M;
    }
    return true;
}

void dcd_edpt_stall(uint8_t rhport, uint8_t ep_addr)
{
    (void)rhport;

    usb_out_endpoint_t *out_ep = &(USB0.out_ep_reg[0]);
    usb_in_endpoint_t *in_ep = &(USB0.in_ep_reg[0]);

    uint8_t const epnum = tu_edpt_number(ep_addr);
    uint8_t const dir = tu_edpt_dir(ep_addr);

    if (dir == TUSB_DIR_IN) {
        // Only disable currently enabled non-control endpoint
        if ((epnum == 0) || !(in_ep[epnum].diepctl & USB_D_EPENA1_M)) {
            in_ep[epnum].diepctl |= (USB_DI_SNAK1_M | USB_D_STALL1_M);
        } else {
            // Stop transmitting packets and NAK IN xfers.
            in_ep[epnum].diepctl |= USB_DI_SNAK1_M;
            while ((in_ep[epnum].diepint & USB_DI_SNAK1_M) == 0)
                ;

            // Disable the endpoint. Note that both SNAK and STALL are set here.
            in_ep[epnum].diepctl |= (USB_DI_SNAK1_M | USB_D_STALL1_M |
                                     USB_D_EPDIS1_M);
            while ((in_ep[epnum].diepint & USB_D_EPDISBLD0_M) == 0)
                ;
            in_ep[epnum].diepint = USB_D_EPDISBLD0_M;
        }

        // Flush the FIFO, and wait until we have confirmed it cleared.
        USB0.grstctl |= ((epnum - 1) << USB_TXFNUM_S);
        USB0.grstctl |= USB_TXFFLSH_M;
        while ((USB0.grstctl & USB_TXFFLSH_M) != 0)
            ;
    } else {
        // Only disable currently enabled non-control endpoint
        if ((epnum == 0) || !(out_ep[epnum].doepctl & USB_EPENA0_M)) {
            out_ep[epnum].doepctl |= USB_STALL0_M;
        } else {
            // Asserting GONAK is required to STALL an OUT endpoint.
            // Simpler to use polling here, we don't use the "B"OUTNAKEFF interrupt
            // anyway, and it can't be cleared by user code. If this while loop never
            // finishes, we have bigger problems than just the stack.
            USB0.dctl |= USB_SGOUTNAK_M;
            while ((USB0.gintsts & USB_GOUTNAKEFF_M) == 0)
                ;

            // Ditto here- disable the endpoint. Note that only STALL and not SNAK
            // is set here.
            out_ep[epnum].doepctl |= (USB_STALL0_M | USB_EPDIS0_M);
            while ((out_ep[epnum].doepint & USB_EPDISBLD0_M) == 0)
                ;
            out_ep[epnum].doepint = USB_EPDISBLD0_M;

            // Allow other OUT endpoints to keep receiving.
            USB0.dctl |= USB_CGOUTNAK_M;
        }
    }
}

void dcd_edpt_clear_stall(uint8_t rhport, uint8_t ep_addr)
{
    (void)rhport;

    usb_out_endpoint_t *out_ep = &(USB0.out_ep_reg[0]);
    usb_in_endpoint_t *in_ep = &(USB0.in_ep_reg[0]);

    uint8_t const epnum = tu_edpt_number(ep_addr);
    uint8_t const dir = tu_edpt_dir(ep_addr);

    if (dir == TUSB_DIR_IN) {
        in_ep[epnum].diepctl &= ~USB_D_STALL1_M;

        uint8_t eptype = (in_ep[epnum].diepctl & USB_D_EPTYPE1_M) >> USB_D_EPTYPE1_S;
        // Required by USB spec to reset DATA toggle bit to DATA0 on interrupt
        // and bulk endpoints.
        if (eptype == 2 || eptype == 3) {
            in_ep[epnum].diepctl |= USB_DI_SETD0PID1_M;
        }
    } else {
        out_ep[epnum].doepctl &= ~USB_STALL1_M;

        uint8_t eptype = (out_ep[epnum].doepctl & USB_EPTYPE1_M) >> USB_EPTYPE1_S;
        // Required by USB spec to reset DATA toggle bit to DATA0 on interrupt
        // and bulk endpoints.
        if (eptype == 2 || eptype == 3) {
            out_ep[epnum].doepctl |= USB_DO_SETD0PID1_M;
        }
    }
}

/*------------------------------------------------------------------*/

static void receive_packet(xfer_ctl_t *xfer, /* usb_out_endpoint_t * out_ep, */ uint16_t xfer_size)
{
    ESP_EARLY_LOGV(TAG, "USB - receive_packet");
    uint32_t *rx_fifo = USB0.fifo[0];

    // See above TODO
    // uint16_t remaining = (out_ep->DOEPTSIZ & UsbDOEPTSIZ_XFRSIZ_Msk) >> UsbDOEPTSIZ_XFRSIZ_Pos;
    // xfer->queued_len = xfer->total_len - remaining;

    uint16_t remaining = xfer->total_len - xfer->queued_len;
    uint16_t to_recv_size;

    if (remaining <= xfer->max_size) {
        // Avoid buffer overflow.
        to_recv_size = (xfer_size > remaining) ? remaining : xfer_size;
    } else {
        // Room for full packet, choose recv_size based on what the microcontroller
        // claims.
        to_recv_size = (xfer_size > xfer->max_size) ? xfer->max_size : xfer_size;
    }

    uint8_t to_recv_rem = to_recv_size % 4;
    uint16_t to_recv_size_aligned = to_recv_size - to_recv_rem;

    // Do not assume xfer buffer is aligned.
    uint8_t *base = (xfer->buffer + xfer->queued_len);

    // This for loop always runs at least once- skip if less than 4 bytes
    // to collect.
    if (to_recv_size >= 4) {
        for (uint16_t i = 0; i < to_recv_size_aligned; i += 4) {
            uint32_t tmp = (*rx_fifo);
            base[i] = tmp & 0x000000FF;
            base[i + 1] = (tmp & 0x0000FF00) >> 8;
            base[i + 2] = (tmp & 0x00FF0000) >> 16;
            base[i + 3] = (tmp & 0xFF000000) >> 24;
        }
    }

    // Do not read invalid bytes from RX FIFO.
    if (to_recv_rem != 0) {
        uint32_t tmp = (*rx_fifo);
        uint8_t *last_32b_bound = base + to_recv_size_aligned;

        last_32b_bound[0] = tmp & 0x000000FF;
        if (to_recv_rem > 1) {
            last_32b_bound[1] = (tmp & 0x0000FF00) >> 8;
        }
        if (to_recv_rem > 2) {
            last_32b_bound[2] = (tmp & 0x00FF0000) >> 16;
        }
    }

    xfer->queued_len += xfer_size;

    // Per USB spec, a short OUT packet (including length 0) is always
    // indicative of the end of a transfer (at least for ctl, bulk, int).
    xfer->short_packet = (xfer_size < xfer->max_size);
}

static void transmit_packet(xfer_ctl_t *xfer, volatile usb_in_endpoint_t *in_ep, uint8_t fifo_num)
{

    ESP_EARLY_LOGV(TAG, "USB - transmit_packet");
    uint32_t *tx_fifo = USB0.fifo[0];

    uint16_t remaining = (in_ep->dieptsiz & 0x7FFFFU) >> USB_D_XFERSIZE0_S;
    xfer->queued_len = xfer->total_len - remaining;

    uint16_t to_xfer_size = (remaining > xfer->max_size) ? xfer->max_size : remaining;
    uint8_t to_xfer_rem = to_xfer_size % 4;
    uint16_t to_xfer_size_aligned = to_xfer_size - to_xfer_rem;

    // Buffer might not be aligned to 32b, so we need to force alignment
    // by copying to a temp var.
    uint8_t *base = (xfer->buffer + xfer->queued_len);

    // This for loop always runs at least once- skip if less than 4 bytes
    // to send off.
    if (to_xfer_size >= 4) {
        for (uint16_t i = 0; i < to_xfer_size_aligned; i += 4) {
            uint32_t tmp = base[i] | (base[i + 1] << 8) |
                           (base[i + 2] << 16) | (base[i + 3] << 24);
            (*tx_fifo) = tmp;
        }
    }

    // Do not read beyond end of buffer if not divisible by 4.
    if (to_xfer_rem != 0) {
        uint32_t tmp = 0;
        uint8_t *last_32b_bound = base + to_xfer_size_aligned;

        tmp |= last_32b_bound[0];
        if (to_xfer_rem > 1) {
            tmp |= (last_32b_bound[1] << 8);
        }
        if (to_xfer_rem > 2) {
            tmp |= (last_32b_bound[2] << 16);
        }

        (*tx_fifo) = tmp;
    }
}

static void read_rx_fifo(void)
{
    // Pop control word off FIFO (completed xfers will have 2 control words,
    // we only pop one ctl word each interrupt).
    volatile uint32_t ctl_word = USB0.grxstsp;
    uint8_t pktsts = (ctl_word & USB_PKTSTS_M) >> USB_PKTSTS_S;
    uint8_t epnum = (ctl_word & USB_CHNUM_M) >> USB_CHNUM_S;
    uint16_t bcnt = (ctl_word & USB_BCNT_M) >> USB_BCNT_S;
    switch (pktsts) {
    case 0x01: // Global OUT NAK (Interrupt)
        ESP_EARLY_LOGV(TAG, "TUSB IRQ - RX type : Global OUT NAK");
        break;
    case 0x02: { // Out packet recvd
        ESP_EARLY_LOGV(TAG, "TUSB IRQ - RX type : Out packet");
        xfer_ctl_t *xfer = XFER_CTL_BASE(epnum, TUSB_DIR_OUT);
        receive_packet(xfer, bcnt);
    }
    break;
    case 0x03: // Out packet done (Interrupt)
        ESP_EARLY_LOGV(TAG, "TUSB IRQ - RX type : Out packet done");
        break;
    case 0x04:                // Setup packet done (Interrupt)
        if (s_setup_phase == 0) { // only if setup is started
            s_setup_phase = 1;
            ESP_EARLY_LOGV(TAG, "TUSB IRQ - setup_phase 1"); //finished
            ESP_EARLY_LOGV(TAG, "TUSB IRQ - RX : Setup packet done");
        }

        break;
    case 0x06: { // Setup packet recvd
        s_setup_phase = 0;
        ESP_EARLY_LOGV(TAG, "TUSB IRQ - setup_phase 0"); // new setup process
        // For some reason, it's possible to get a mismatch between
        // how many setup packets were received versus the location
        // of the Setup packet done word. This leads to situations
        // where stale setup packets are in the RX FIFO that were received
        // after the core loaded the Setup packet done word. Workaround by
        // only accepting one setup packet at a time for now.
        _setup_packet[0] = (USB0.grxstsp);
        _setup_packet[1] = (USB0.grxstsp);
        ESP_EARLY_LOGV(TAG, "TUSB IRQ - RX : Setup packet : 0x%08x 0x%08x",
                       _setup_packet[0], _setup_packet[1]);
    }
    break;
    default: // Invalid, do something here, like breakpoint?
        break;
    }
}

static void handle_epout_ints(void)
{
    // GINTSTS will be cleared with DAINT == 0
    // DAINT for a given EP clears when DOEPINTx is cleared.
    // DOEPINT will be cleared when DAINT's out bits are cleared.
    for (int n = 0; n < USB_OUT_EP_NUM; n++) {
        xfer_ctl_t *xfer = XFER_CTL_BASE(n, TUSB_DIR_OUT);
        if (USB0.daint & (1 << (16 + n))) {
            // SETUP packet Setup Phase done.
            if ((USB0.out_ep_reg[n].doepint & USB_SETUP0_M)) {
                USB0.out_ep_reg[n].doepint |= USB_STUPPKTRCVD0_M | USB_SETUP0_M; // clear
                if (s_setup_phase == 1) {                                          // only if setup is done, but not handled
                    s_setup_phase = 2;
                    ESP_EARLY_LOGV(TAG, "TUSB IRQ - setup_phase 2"); // sending to  a handling queue
                    ESP_EARLY_LOGV(TAG, "TUSB IRQ - EP OUT - Setup Phase done (irq-s 0x%08x)", USB0.out_ep_reg[n].doepint);
                    dcd_event_setup_received(0, (uint8_t *)&_setup_packet[0], true);
                }
                readyfor1setup_pkg(0);
            }

            // OUT XFER complete (single packet).q
            if (USB0.out_ep_reg[n].doepint & USB_XFERCOMPL0_M) {

                ESP_EARLY_LOGV(TAG, "TUSB IRQ - EP OUT - XFER complete (single packet)");
                USB0.out_ep_reg[n].doepint = USB_XFERCOMPL0_M;

                // Transfer complete if short packet or total len is transferred
                if (xfer->short_packet || (xfer->queued_len == xfer->total_len)) {
                    xfer->short_packet = false;
                    dcd_event_xfer_complete(0, n, xfer->queued_len, XFER_RESULT_SUCCESS, true);
                } else {
                    // Schedule another packet to be received.
                    USB0.out_ep_reg[n].doeptsiz = USB_PKTCNT0_M |
                                                  ((xfer->max_size & USB_XFERSIZE0_V) << USB_XFERSIZE0_S);
                    USB0.out_ep_reg[n].doepctl |= USB_EPENA0_M | USB_CNAK0_M;
                }
            }
        }
    }
}

static void handle_epin_ints(void)
{

    // GINTSTS will be cleared with DAINT == 0
    // DAINT for a given EP clears when DIEPINTx is cleared.
    // IEPINT will be cleared when DAINT's out bits are cleared.
    for (uint32_t n = 0; n < USB_IN_EP_NUM; n++) {
        xfer_ctl_t *xfer = &xfer_status[n][TUSB_DIR_IN];

        if (USB0.daint & (1 << (0 + n))) {
            ESP_EARLY_LOGV(TAG, "TUSB IRQ - EP IN %u", n);
            // IN XFER complete (entire xfer).
            if (USB0.in_ep_reg[n].diepint & USB_D_XFERCOMPL0_M) {
                ESP_EARLY_LOGV(TAG, "TUSB IRQ - IN XFER complete!");
                USB0.in_ep_reg[n].diepint = USB_D_XFERCOMPL0_M;
                // USB0.dtknqr4_fifoemptymsk &= ~(1 << n); // Turn off TXFE b/c xfer inactive.
                dcd_event_xfer_complete(0, n | TUSB_DIR_IN_MASK, xfer->total_len, XFER_RESULT_SUCCESS, true);
            }

            // XFER FIFO empty
            if (USB0.in_ep_reg[n].diepint & USB_D_XFERCOMPL0_M) {
                ESP_EARLY_LOGV(TAG, "TUSB IRQ - IN XFER FIFO empty!");
                USB0.in_ep_reg[n].diepint = USB_D_TXFEMP0_M;
                transmit_packet(xfer, &USB0.in_ep_reg[n], n);
            }
        }
    }
}


void dcd_irq_handler(void)
{
    uint32_t int_status = USB0.gintsts;
    uint32_t int_msk = USB0.gintmsk;

    if (int_status & USB_DISCONNINT_M) {
        ESP_EARLY_LOGV(TAG, "dcd_irq_handler - disconnected");
        USB0.gintsts = USB_DISCONNINT_M;
    }

    if (int_status & USB_USBRST_M) {

        ESP_EARLY_LOGV(TAG, "dcd_irq_handler - reset");
        USB0.gintsts = USB_USBRST_M;
        bus_reset();
    }

    if (int_status & USB_RESETDET_M) {
        ESP_EARLY_LOGV(TAG, "dcd_irq_handler - reset while suspend");
        USB0.gintsts = USB_RESETDET_M;
        bus_reset();
    }

    if (int_status & USB_ENUMDONE_M) {
        // ENUMDNE detects speed of the link. For full-speed, we
        // always expect the same value. This interrupt is considered
        // the end of reset.
        USB0.gintsts = USB_ENUMDONE_M;
        enum_done_processing();
        dcd_event_bus_signal(0, DCD_EVENT_BUS_RESET, true);
    }

    if (int_status & USB_SOF_M) {
        USB0.gintsts = USB_SOF_M;
        dcd_event_bus_signal(0, DCD_EVENT_SOF, true); // do nothing actually
    }

    if ((int_status & USB_RXFLVI_M) & (int_msk & USB_RXFLVIMSK_M)) {
        ESP_EARLY_LOGV(TAG, "dcd_irq_handler - rx!");
        read_rx_fifo();
    }

    // OUT endpoint interrupt handling.
    if (int_status & USB_OEPINT_M) {
        ESP_EARLY_LOGV(TAG, "dcd_irq_handler - OUT endpoint!");
        handle_epout_ints();
    }

    // IN endpoint interrupt handling.
    if (int_status & USB_IEPINT_M) {
        ESP_EARLY_LOGV(TAG, "dcd_irq_handler - IN endpoint!");
        handle_epin_ints();
    }

    // Without handling
    USB0.gintsts |= USB_CURMOD_INT_M |
                    USB_MODEMIS_M |
                    USB_OTGINT_M |
                    USB_NPTXFEMP_M |
                    USB_GINNAKEFF_M |
                    USB_GOUTNAKEFF |
                    USB_ERLYSUSP_M |
                    USB_USBSUSP_M |
                    USB_ISOOUTDROP_M |
                    USB_EOPF_M |
                    USB_EPMIS_M |
                    USB_INCOMPISOIN_M |
                    USB_INCOMPIP_M |
                    USB_FETSUSP_M |
                    USB_PTXFEMP_M;
}

void dcd_int_enable(uint8_t rhport)
{
    (void)rhport;
    esp_intr_alloc(ETS_USB_INTR_SOURCE, ESP_INTR_FLAG_LOWMED, (intr_handler_t)dcd_irq_handler, NULL, &usb_ih);
}

void dcd_int_disable(uint8_t rhport)
{
    (void)rhport;
    esp_intr_free(usb_ih);
}
