/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
Registers and fields were generated based on a set of USB-DWC configuration options.
ESP32-P4 contains 2 instances of USB-DWC with different configurations, the structure below corresponds to the HS instance.
The FS instance contains a subset of registers from HS instance, the user (HAL) is responsible for accessing only existing fields.

See ESP32-P4 "usb_dwc_cfg.h" for more details.
*/

/* ---------------------------- Register Types ------------------------------ */

typedef union {
    struct {
        uint32_t sesreqscs: 1;
        uint32_t sesreq: 1;
        uint32_t vbvalidoven: 1;
        uint32_t vbvalidovval: 1;
        uint32_t avalidoven: 1;
        uint32_t avalidovval: 1;
        uint32_t bvalidoven: 1;
        uint32_t bvalidovval: 1;
        uint32_t hstnegscs: 1;
        uint32_t hnpreq: 1;
        uint32_t hstsethnpen: 1;
        uint32_t devhnpen: 1;
        uint32_t ehen: 1;    // codespell:ignore ehen
        uint32_t reserved_13: 2;
        uint32_t dbncefltrbypass: 1;
        uint32_t conidsts: 1;
        uint32_t dbnctime: 1;
        uint32_t asesvld: 1;
        uint32_t bsesvld: 1;
        uint32_t otgver: 1;
        uint32_t curmod: 1;
        uint32_t reserved_22: 5;
        uint32_t reserved_27: 1;
        uint32_t reserved_28: 4;
    };
    uint32_t val;
} usb_dwc_gotgctl_reg_t;

typedef union {
    struct {
        uint32_t reserved_0: 2;
        uint32_t sesenddet: 1;
        uint32_t reserved_3: 5;
        uint32_t sesreqsucstschng: 1;
        uint32_t hstnegsucstschng: 1;
        uint32_t reserved_10: 7;
        uint32_t hstnegdet: 1;
        uint32_t adevtoutchg: 1;
        uint32_t dbncedone: 1;
        uint32_t reserved_20: 1;
        uint32_t reserved_21: 11;
    };
    uint32_t val;
} usb_dwc_gotgint_reg_t;

typedef union {
    struct {
        uint32_t glbllntrmsk: 1;
        uint32_t hbstlen: 4;
        uint32_t dmaen: 1;
        uint32_t reserved_6: 1;
        uint32_t nptxfemplvl: 1;
        uint32_t ptxfemplvl: 1;
        uint32_t reserved_9: 12;
        uint32_t remmemsupp: 1;
        uint32_t notialldmawrit: 1;
        uint32_t ahbsingle: 1;
        uint32_t invdescendianess: 1;
        uint32_t reserved_25: 7;
    };
    uint32_t val;
} usb_dwc_gahbcfg_reg_t;

typedef union {
    struct {
        uint32_t toutcal: 3;
        uint32_t phyif: 1;
        uint32_t ulpiutmisel: 1;
        uint32_t fsintf: 1;
        uint32_t physel: 1;
        uint32_t ddrsel: 1;
        uint32_t srpcap: 1;
        uint32_t hnpcap: 1;
        uint32_t usbtrdtim: 4;
        uint32_t reserved_14: 1;
        uint32_t phylpwrclksel: 1;
        uint32_t reserved_16: 1;
        uint32_t ulpifsls: 1;
        uint32_t ulpiautores: 1;
        uint32_t ulpiclksusm: 1;
        uint32_t ulpiextvbusdrv: 1;
        uint32_t ulpiextvbusindicator: 1;
        uint32_t termseldlpulse: 1;
        uint32_t complement: 1;
        uint32_t indicator: 1;
        uint32_t ulpi: 1;
        uint32_t icusbcap: 1;
        uint32_t reserved_27: 1;
        uint32_t txenddelay: 1;
        uint32_t forcehstmode: 1;
        uint32_t forcedevmode: 1;
        uint32_t corrupttxpkt: 1;
    };
    uint32_t val;
} usb_dwc_gusbcfg_reg_t;

typedef union {
    struct {
        uint32_t csftrst: 1;
        uint32_t piufssftrst: 1;
        uint32_t frmcntrrst: 1;
        uint32_t reserved_3: 1;
        uint32_t rxfflsh: 1;
        uint32_t txfflsh: 1;
        uint32_t txfnum: 5;
        uint32_t reserved_11: 19;
        uint32_t dmareq: 1;
        uint32_t ahbidle: 1;
    };
    uint32_t val;
} usb_dwc_grstctl_reg_t;

typedef union {
    struct {
        uint32_t curmod: 1;
        uint32_t modemis: 1;
        uint32_t otgint: 1;
        uint32_t sof: 1;
        uint32_t rxflvl: 1;
        uint32_t nptxfemp: 1;
        uint32_t ginnakeff: 1;
        uint32_t goutnakeff: 1;
        uint32_t reserved_8: 1;
        uint32_t reserved_9: 1;
        uint32_t erlysusp: 1;
        uint32_t usbsusp: 1;
        uint32_t usbrst: 1;
        uint32_t enumdone: 1;
        uint32_t isooutdrop: 1;
        uint32_t eopf: 1;
        uint32_t reserved_16: 1;
        uint32_t epmis: 1;
        uint32_t iepint: 1;
        uint32_t oepint: 1;
        uint32_t incompisoin: 1;
        uint32_t incompip: 1;
        uint32_t fetsusp: 1;
        uint32_t resetdet: 1;
        uint32_t prtint: 1;
        uint32_t hchint: 1;
        uint32_t ptxfemp: 1;
        uint32_t reserved_27: 1;
        uint32_t conidstschng: 1;
        uint32_t disconnint: 1;
        uint32_t sessreqint: 1;
        uint32_t wkupint: 1;
    };
    uint32_t val;
} usb_dwc_gintsts_reg_t;

typedef union {
    struct {
        uint32_t reserved_0: 1;
        uint32_t modemismsk: 1;
        uint32_t otgintmsk: 1;
        uint32_t sofmsk: 1;
        uint32_t rxflvlmsk: 1;
        uint32_t nptxfempmsk: 1;
        uint32_t ginnakeffmsk: 1;
        uint32_t goutnackeffmsk: 1;
        uint32_t reserved_8: 1;
        uint32_t reserved_9: 1;
        uint32_t erlysuspmsk: 1;
        uint32_t usbsuspmsk: 1;
        uint32_t usbrstmsk: 1;
        uint32_t enumdonemsk: 1;
        uint32_t isooutdropmsk: 1;
        uint32_t eopfmsk: 1;
        uint32_t reserved_16: 1;
        uint32_t epmismsk: 1;
        uint32_t iepintmsk: 1;
        uint32_t oepintmsk: 1;
        uint32_t incompisoinmsk: 1;
        uint32_t incompipmsk: 1;
        uint32_t fetsuspmsk: 1;
        uint32_t resetdetmsk: 1;
        uint32_t prtintmsk: 1;
        uint32_t hchintmsk: 1;
        uint32_t ptxfempmsk: 1;
        uint32_t reserved_27: 1;
        uint32_t conidstschngmsk: 1;
        uint32_t disconnintmsk: 1;
        uint32_t sessreqintmsk: 1;
        uint32_t wkupintmsk: 1;
    };
    uint32_t val;
} usb_dwc_gintmsk_reg_t;

typedef union {
    struct {
        uint32_t chnum: 4;
        uint32_t bcnt: 11;
        uint32_t dpid: 2;
        uint32_t pktsts: 4;
        uint32_t fn: 4;
        uint32_t reserved_25: 6;
        uint32_t reserved_31: 1;
    };
    uint32_t val;
} usb_dwc_grxstsr_reg_t;

typedef union {
    struct {
        uint32_t chnum: 4;
        uint32_t bcnt: 11;
        uint32_t dpid: 2;
        uint32_t pktsts: 4;
        uint32_t fn: 4;
        uint32_t reserved_25: 6;
        uint32_t reserved_31: 1;
    };
    uint32_t val;
} usb_dwc_grxstsp_reg_t;

typedef union {
    struct {
        uint32_t rxfdep: 16;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} usb_dwc_grxfsiz_reg_t;

typedef union {
    struct {
        uint32_t nptxfstaddr: 16;
        uint32_t nptxfdep: 16;
    };
    uint32_t val;
} usb_dwc_gnptxfsiz_reg_t;

typedef union {
    struct {
        uint32_t nptxfspcavail: 16;
        uint32_t nptxqspcavail: 8;
        uint32_t nptxqtop: 7;
        uint32_t reserved_31: 1;
    };
    uint32_t val;
} usb_dwc_gnptxsts_reg_t;

typedef union {
    struct {
        uint32_t regdata: 8;
        uint32_t vctrl: 8;
        uint32_t regaddr: 6;
        uint32_t regwr: 1;
        uint32_t reserved_23: 2;
        uint32_t newregreq: 1;
        uint32_t vstsbsy: 1;
        uint32_t vstsdone: 1;
        uint32_t reserved_28: 3;
        uint32_t disulpidrvr: 1;
    };
    uint32_t val;
} usb_dwc_gpvndctl_reg_t;

typedef union {
    struct {
        uint32_t synopsysid;
    };
    uint32_t val;
} usb_dwc_gsnpsid_reg_t;

typedef union {
    struct {
        uint32_t epdir;
    };
    uint32_t val;
} usb_dwc_ghwcfg1_reg_t;

typedef union {
    struct {
        uint32_t otgmode: 3;
        uint32_t otgarch: 2;
        uint32_t singpnt: 1;
        uint32_t hsphytype: 2;
        uint32_t fsphytype: 2;
        uint32_t numdeveps: 4;
        uint32_t numhstchnl: 4;
        uint32_t periosupport: 1;
        uint32_t dynfifosizing: 1;
        uint32_t multiprocintrpt: 1;
        uint32_t reserved_21: 1;
        uint32_t nptxqdepth: 2;
        uint32_t ptxqdepth: 2;
        uint32_t tknqdepth: 5;
        uint32_t reserved_31: 1;
    };
    uint32_t val;
} usb_dwc_ghwcfg2_reg_t;

typedef union {
    struct {
        uint32_t xfersizewidth: 4;
        uint32_t pktsizewidth: 3;
        uint32_t otgen: 1;
        uint32_t i2cintsel: 1;
        uint32_t vndctlsupt: 1;
        uint32_t optfeature: 1;
        uint32_t rsttype: 1;
        uint32_t adpsupport: 1;
        uint32_t hsicmode: 1;
        uint32_t bcsupport: 1;
        uint32_t lpmmode: 1;
        uint32_t dfifodepth: 16;
    };
    uint32_t val;
} usb_dwc_ghwcfg3_reg_t;

typedef union {
    struct {
        uint32_t numdevperioeps: 4;
        uint32_t partialpwrdn: 1;
        uint32_t ahbfreq: 1;
        uint32_t hibernation: 1;
        uint32_t extendedhibernation: 1;
        uint32_t reserved_8: 4;
        uint32_t acgsupt: 1;
        uint32_t enhancedlpmsupt: 1;
        uint32_t phydatawidth: 2;
        uint32_t numctleps: 4;
        uint32_t iddqfltr: 1;
        uint32_t vbusvalidfltr: 1;
        uint32_t avalidfltr: 1;
        uint32_t bvalidfltr: 1;
        uint32_t sessendfltr: 1;
        uint32_t dedfifomode: 1;
        uint32_t ineps: 4;
        uint32_t descdmaenabled: 1;
        uint32_t descdma: 1;
    };
    uint32_t val;
} usb_dwc_ghwcfg4_reg_t;

typedef union {
    struct {
        uint32_t pmuintsel: 1;
        uint32_t pmuactv: 1;
        uint32_t reserved_2: 1;
        uint32_t pwrdnclmp: 1;
        uint32_t pwrdnrst_n: 1;
        uint32_t pwrdnswtch: 1;
        uint32_t disablevbus: 1;
        uint32_t reserved_7: 1;
        uint32_t reserved_8: 1;
        uint32_t reserved_9: 1;
        uint32_t reserved_10: 1;
        uint32_t reserved_11: 1;
        uint32_t reserved_12: 1;
        uint32_t reserved_13: 1;
        uint32_t reserved_14: 1;
        uint32_t srpdetect: 1;
        uint32_t srpdetectmsk: 1;
        uint32_t stschngint: 1;
        uint32_t stschngintmsk: 1;
        uint32_t linestate: 2;
        uint32_t iddig: 1;
        uint32_t bsessvld: 1;
        uint32_t adpint: 1;
        uint32_t reserved_24: 5;
        uint32_t reserved_29: 3;
    };
    uint32_t val;
} usb_dwc_gpwrdn_reg_t;

typedef union {
    struct {
        uint32_t gdfifocfg: 16;
        uint32_t epinfobaseaddr: 16;

    };
    uint32_t val;
} usb_dwc_gdfifocfg_reg_t;

typedef union {
    struct {
        uint32_t prbdschg: 2;
        uint32_t prbdelta: 2;
        uint32_t prbper: 2;
        uint32_t rtim: 11;
        uint32_t enaprb: 1;
        uint32_t enasns: 1;
        uint32_t adpres: 1;
        uint32_t adpen: 1;
        uint32_t adpprbint: 1;
        uint32_t adpsnsint: 1;
        uint32_t adptoutint: 1;
        uint32_t adpprbintmsk: 1;
        uint32_t adpsnsintmsk: 1;
        uint32_t adptoutmsk: 1;
        uint32_t ar: 2;
        uint32_t reserved_29: 3;
    };
    uint32_t val;
} usb_dwc_gadpctl_reg_t;

typedef union {
    struct {
        uint32_t ptxfstaddr: 16;
        uint32_t ptxfsize: 16;
    };
    uint32_t val;
} usb_dwc_hptxfsiz_reg_t;

typedef union {
    struct {
        uint32_t inepntxfstaddr: 16;
        uint32_t inepntxfdep: 16;
    };
    uint32_t val;
} usb_dwc_dieptxfi_reg_t;

typedef union {
    struct {
        uint32_t fslspclksel: 2;
        uint32_t fslssupp: 1;
        uint32_t reserved_3: 4;
        uint32_t ena32khzs: 1;
        uint32_t resvalid: 8;
        uint32_t reserved_16: 1;
        uint32_t reserved_17: 6;
        uint32_t descdma: 1;
        uint32_t frlisten: 2;
        uint32_t perschedena: 1;
        uint32_t reserved_27: 4;
        uint32_t modechtimen: 1;
    };
    uint32_t val;
} usb_dwc_hcfg_reg_t;

typedef union {
    struct {
        uint32_t frint: 16;
        uint32_t hfirrldctrl: 1;
        uint32_t reserved_17: 15;
    };
    uint32_t val;
} usb_dwc_hfir_reg_t;

typedef union {
    struct {
        uint32_t frnum: 16;
        uint32_t frrem: 16;
    };
    uint32_t val;
} usb_dwc_hfnum_reg_t;

typedef union {
    struct {
        uint32_t ptxfspcavail: 16;
        uint32_t ptxqspcavail: 8;
        uint32_t ptxqtop: 8;
    };
    uint32_t val;
} usb_dwc_hptxsts_reg_t;

typedef union {
    struct {
        uint32_t haint: 16;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} usb_dwc_haint_reg_t;

typedef union {
    struct {
        uint32_t haintmsk: 16;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} usb_dwc_haintmsk_reg_t;

typedef union {
    struct {
        uint32_t hflbaddr;
    };
    uint32_t val;
} usb_dwc_hflbaddr_reg_t;

typedef union {
    struct {
        uint32_t prtconnsts: 1;
        uint32_t prtconndet: 1;
        uint32_t prtena: 1;
        uint32_t prtenchng: 1;
        uint32_t prtovrcurract: 1;
        uint32_t prtovrcurrchng: 1;
        uint32_t prtres: 1;
        uint32_t prtsusp: 1;
        uint32_t prtrst: 1;
        uint32_t reserved_9: 1;
        uint32_t prtlnsts: 2;
        uint32_t prtpwr: 1;
        uint32_t prttstctl: 4;
        uint32_t prtspd: 2;
        uint32_t reserved_19: 13;
    };
    uint32_t val;
} usb_dwc_hprt_reg_t;

typedef union {
    struct {
        uint32_t mps: 11;
        uint32_t epnum: 4;
        uint32_t epdir: 1;
        uint32_t reserved_16: 1;
        uint32_t lspddev: 1;
        uint32_t eptype: 2;
        uint32_t ec: 2;
        uint32_t devaddr: 7;
        uint32_t oddfrm: 1;
        uint32_t chdis: 1;
        uint32_t chena: 1;
    };
    uint32_t val;
} usb_dwc_hcchar_reg_t;

typedef union {
    struct {
        uint32_t prtaddr: 7;
        uint32_t hubaddr: 7;
        uint32_t xactpos: 2;
        uint32_t compsplt: 1;
        uint32_t reserved_17: 14;
        uint32_t spltena: 1;
    };
    uint32_t val;
} usb_dwc_hcsplt_reg_t;

typedef union {
    struct {
        uint32_t xfercompl: 1;
        uint32_t chhltd: 1;
        uint32_t ahberr: 1;
        uint32_t stall: 1;
        uint32_t nack: 1;
        uint32_t ack: 1;
        uint32_t nyet: 1;
        uint32_t xacterr: 1;
        uint32_t bblerr: 1;
        uint32_t frmovrun: 1;
        uint32_t datatglerr: 1;
        uint32_t bnaintr: 1;
        uint32_t xcs_xact_err: 1;
        uint32_t desc_lst_rollintr: 1;
        uint32_t reserved_14: 18;
    };
    uint32_t val;
} usb_dwc_hcint_reg_t;

typedef union {
    struct {
        uint32_t xfercomplmsk: 1;
        uint32_t chhltdmsk: 1;
        uint32_t ahberrmsk: 1;
        uint32_t reserved_3: 1;
        uint32_t reserved_4: 1;
        uint32_t reserved_5: 1;
        uint32_t reserved_6: 1;
        uint32_t reserved_7: 1;
        uint32_t reserved_8: 1;
        uint32_t reserved_9: 1;
        uint32_t reserved_10: 1;
        uint32_t bnaintrmsk: 1;
        uint32_t reserved_12: 1;
        uint32_t desc_lst_rollintrmsk: 1;
        uint32_t reserved_14: 18;
    };
    uint32_t val;
} usb_dwc_hcintmsk_reg_t;

typedef union {
    struct {
        uint32_t xfersize: 19;  // Note: Width depends on OTG_TRANS_COUNT_WIDTH (see databook).
        uint32_t pktcnt: 10;    // Note: Width depends on OTG_PACKET_COUNT_WIDTH (see databook).
        uint32_t pid: 2;
        uint32_t dopng: 1;
    };
    uint32_t val;
} usb_dwc_hctsiz_reg_t;

typedef union {
    struct {
        uint32_t dmaaddr;
    };
    uint32_t val;
} usb_dwc_hcdma_reg_t;

typedef union {
    struct {
        uint32_t hcdmab;
    };
    uint32_t val;
} usb_dwc_hcdmab_reg_t;

typedef union {
    struct {
        uint32_t devspd: 2;
        uint32_t nzstsouthshk: 1;
        uint32_t ena32khzsusp: 1;
        uint32_t devaddr: 7;
        uint32_t perfrint: 2;
        uint32_t endevoutnak: 1;
        uint32_t xcvrdly: 1;
        uint32_t erraticintmsk: 1;
        uint32_t reserved_16: 2;
        uint32_t reserved_18: 5;
        uint32_t descdma: 1;
        uint32_t perschintvl: 2;
        uint32_t resvalid: 6;
    };
    uint32_t val;
} usb_dwc_dcfg_reg_t;

typedef union {
    struct {
        uint32_t rmtwkupsig: 1;
        uint32_t sftdiscon: 1;
        uint32_t gnpinnaksts: 1;
        uint32_t goutnaksts: 1;
        uint32_t tstctl: 3;
        uint32_t sgnpinnak: 1;
        uint32_t cgnpinnak: 1;
        uint32_t sgoutnak: 1;
        uint32_t cgoutnak: 1;
        uint32_t pwronprgdone: 1;
        uint32_t reserved_12: 1;
        uint32_t gmc: 2;
        uint32_t ignrfrmnum: 1;
        uint32_t nakonbble: 1;
        uint32_t encontonbna: 1;
        uint32_t reserved_18: 1;
        uint32_t reserved_19: 13;
    };
    uint32_t val;
} usb_dwc_dctl_reg_t;

typedef union {
    struct {
        uint32_t suspsts: 1;
        uint32_t enumspd: 2;
        uint32_t errticerr: 1;
        uint32_t reserved_4: 4;
        uint32_t soffn: 14;
        uint32_t devlnsts: 2;
        uint32_t reserved_24: 8;
    };
    uint32_t val;
} usb_dwc_dsts_reg_t;

typedef union {
    struct {
        uint32_t xfercomplmsk: 1;
        uint32_t epdisbldmsk: 1;
        uint32_t ahberrmsk: 1;
        uint32_t timeoutmsk: 1;
        uint32_t intkntxfempmsk: 1;
        uint32_t intknepmismsk: 1;
        uint32_t inepnakeffmsk: 1;
        uint32_t reserved_7: 1;
        uint32_t txfifoundrnmsk: 1;
        uint32_t bnainintrmsk: 1;
        uint32_t reserved_10: 3;
        uint32_t nakmsk: 1;
        uint32_t reserved_14: 18;
    };
    uint32_t val;
} usb_dwc_diepmsk_reg_t;

typedef union {
    struct {
        uint32_t xfercomplmsk: 1;
        uint32_t epdisbldmsk: 1;
        uint32_t ahberrmsk: 1;
        uint32_t setupmsk: 1;
        uint32_t outtknepdismsk: 1;
        uint32_t stsphsercvdmsk: 1;
        uint32_t back2backsetup: 1;
        uint32_t reserved_7: 1;
        uint32_t outpkterrmsk: 1;
        uint32_t bnaoutintrmsk: 1;
        uint32_t reserved_10: 2;
        uint32_t bbleerrmsk: 1;
        uint32_t nakmsk: 1;
        uint32_t nyetmsk: 1;
        uint32_t reserved_15: 17;
    };
    uint32_t val;
} usb_dwc_doepmsk_reg_t;

typedef union {
    struct {
        uint32_t inepint0: 1;
        uint32_t inepint1: 1;
        uint32_t inepint2: 1;
        uint32_t inepint3: 1;
        uint32_t inepint4: 1;
        uint32_t inepint5: 1;
        uint32_t inepint6: 1;
        uint32_t inepint7: 1;
        uint32_t inepint8: 1;
        uint32_t inepint9: 1;
        uint32_t inepint10: 1;
        uint32_t inepint11: 1;
        uint32_t inepint12: 1;
        uint32_t inepint13: 1;
        uint32_t inepint14: 1;
        uint32_t inepint15: 1;
        uint32_t outepint0: 1;
        uint32_t outepint1: 1;
        uint32_t outepint2: 1;
        uint32_t outepint3: 1;
        uint32_t outepint4: 1;
        uint32_t outepint5: 1;
        uint32_t outepint6: 1;
        uint32_t outepint7: 1;
        uint32_t outepint8: 1;
        uint32_t outepint9: 1;
        uint32_t outepint10: 1;
        uint32_t outepint11: 1;
        uint32_t outepint12: 1;
        uint32_t outepint13: 1;
        uint32_t outepint14: 1;
        uint32_t outepint15: 1;
    };
    uint32_t val;
} usb_dwc_daint_reg_t;

typedef union {
    struct {
        uint32_t inepmsk0: 1;
        uint32_t inepmsk1: 1;
        uint32_t inepmsk2: 1;
        uint32_t inepmsk3: 1;
        uint32_t inepmsk4: 1;
        uint32_t inepmsk5: 1;
        uint32_t inepmsk6: 1;
        uint32_t inepmsk7: 1;
        uint32_t inepmsk8: 1;
        uint32_t inepmsk9: 1;
        uint32_t inepmsk10: 1;
        uint32_t inepmsk11: 1;
        uint32_t inepmsk12: 1;
        uint32_t inepmsk13: 1;
        uint32_t inepmsk14: 1;
        uint32_t inepmsk15: 1;
        uint32_t outepmsk0: 1;
        uint32_t outepmsk1: 1;
        uint32_t outepmsk2: 1;
        uint32_t outepmsk3: 1;
        uint32_t outepmsk4: 1;
        uint32_t outepmsk5: 1;
        uint32_t outepmsk6: 1;
        uint32_t outepmsk7: 1;
        uint32_t outepmsk8: 1;
        uint32_t outepmsk9: 1;
        uint32_t outepmsk10: 1;
        uint32_t outepmsk11: 1;
        uint32_t outepmsk12: 1;
        uint32_t outepmsk13: 1;
        uint32_t outepmsk14: 1;
        uint32_t outepmsk15: 1;
    };
    uint32_t val;
} usb_dwc_daintmsk_reg_t;

typedef union {
    struct {
        uint32_t dvbusdis: 16;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} usb_dwc_dvbusdis_reg_t;

typedef union {
    struct {
        uint32_t dvbuspulse: 12;
        uint32_t reserved_12: 20;
    };
    uint32_t val;
} usb_dwc_dvbuspulse_reg_t;

typedef union {
    struct {
        uint32_t nonisothren: 1;
        uint32_t isothren: 1;
        uint32_t txthrlen: 9;
        uint32_t ahbthrratio: 2;
        uint32_t reserved_13: 3;
        uint32_t rxthren: 1;
        uint32_t rxthrlen: 9;
        uint32_t reserved_26: 1;
        uint32_t arbprken: 1;
        uint32_t reserved_28: 4;
    };
    uint32_t val;
} usb_dwc_dthrctl_reg_t;

typedef union {
    struct {
        uint32_t ineptxfempmsk: 16;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} usb_dwc_diepempmsk_reg_t;

typedef union {
    struct {
        uint32_t echinepint: 16;
        uint32_t echoutepint: 16;
    };
    uint32_t val;
} usb_dwc_deachint_reg_t;

typedef union {
    struct {
        uint32_t echinepmsk0: 1;
        uint32_t echinepmsk1: 1;
        uint32_t echinepmsk2: 1;
        uint32_t echinepmsk3: 1;
        uint32_t echinepmsk4: 1;
        uint32_t echinepmsk5: 1;
        uint32_t echinepmsk6: 1;
        uint32_t echinepmsk7: 1;
        uint32_t echinepmsk8: 1;
        uint32_t echinepmsk9: 1;
        uint32_t echinepmsk10: 1;
        uint32_t echinepmsk11: 1;
        uint32_t echinepmsk12: 1;
        uint32_t echinepmsk13: 1;
        uint32_t echinepmsk14: 1;
        uint32_t echinepmsk15: 1;
        uint32_t echoutepmsk0: 1;
        uint32_t echoutepmsk1: 1;
        uint32_t echoutepmsk2: 1;
        uint32_t echoutepmsk3: 1;
        uint32_t echoutepmsk4: 1;
        uint32_t echoutepmsk5: 1;
        uint32_t echoutepmsk6: 1;
        uint32_t echoutepmsk7: 1;
        uint32_t echoutepmsk8: 1;
        uint32_t echoutepmsk9: 1;
        uint32_t echoutepmsk10: 1;
        uint32_t echoutepmsk11: 1;
        uint32_t echoutepmsk12: 1;
        uint32_t echoutepmsk13: 1;
        uint32_t echoutepmsk14: 1;
        uint32_t echoutepmsk15: 1;
    };
    uint32_t val;
} usb_dwc_deachintmsk_reg_t;

typedef union {
    struct {
        uint32_t xfercomplmsl: 1;
        uint32_t epdisbldmsk: 1;
        uint32_t ahberrmsk: 1;
        uint32_t timeoutmsk: 1;
        uint32_t intkntxfempmsk: 1;
        uint32_t intknepmismsk: 1;
        uint32_t inepnakeffmsk: 1;
        uint32_t reserved_7: 1;
        uint32_t txfifoundrnmsk: 1;
        uint32_t bnainintrmsk: 1;
        uint32_t reserved_10: 3;
        uint32_t nakmsk: 1;
        uint32_t reserved_14: 18;
    };
    uint32_t val;
} usb_dwc_diepeachmski_reg_t;

typedef union {
    struct {
        uint32_t xfercomplmsk: 1;
        uint32_t epdisbldmsk: 1;
        uint32_t ahberrmsk: 1;
        uint32_t setupmsk: 1;
        uint32_t outtknepdismsk: 1;
        uint32_t reserved_5: 1;
        uint32_t back2backsetup: 1;
        uint32_t reserved_7: 1;
        uint32_t outpkterrmsk: 1;
        uint32_t bnaoutintrmsk: 1;
        uint32_t reserved_10: 2;
        uint32_t bbleerrmsk: 1;
        uint32_t nakmsk: 1;
        uint32_t nyetmsk: 1;
        uint32_t reserved_15: 17;
    };
    uint32_t val;
} usb_dwc_doepeachmski_reg_t;

typedef union {
    struct {
        uint32_t mps: 2;
        uint32_t reserved_2: 9;
        uint32_t reserved_11: 4;
        uint32_t usbactep: 1;
        uint32_t reserved_16: 1;
        uint32_t naksts: 1;
        uint32_t eptype: 2;
        uint32_t reserved_20: 1;
        uint32_t stall: 1;
        uint32_t txfnum: 4;
        uint32_t cnak: 1;
        uint32_t snak: 1;
        uint32_t reserved_28: 2;
        uint32_t epdis: 1;
        uint32_t epena: 1;
    };
    uint32_t val;
} usb_dwc_diepctl0_reg_t;

typedef union {
    struct {
        uint32_t xfercompl: 1;
        uint32_t epdisbld: 1;
        uint32_t ahberr: 1;
        uint32_t timeout: 1;
        uint32_t intkntxfemp: 1;
        uint32_t intknepmis: 1;
        uint32_t inepnakeff: 1;
        uint32_t txfemp: 1;
        uint32_t txfifoundrn: 1;
        uint32_t bnaintr: 1;
        uint32_t reserved_10: 1;
        uint32_t pktdrpsts: 1;
        uint32_t bbleerr: 1;
        uint32_t nakintrpt: 1;
        uint32_t nyetintrpt: 1;
        uint32_t reserved_15: 17;
    };
    uint32_t val;
} usb_dwc_diepint0_reg_t;

typedef union {
    struct {
        uint32_t xfersize: 7;
        uint32_t reserved_7: 12;
        uint32_t pktcnt: 2;
        uint32_t reserved_21: 11;
    };
    uint32_t val;
} usb_dwc_dieptsiz0_reg_t;

typedef union {
    struct {
        uint32_t dmaaddr;
    };
    uint32_t val;
} usb_dwc_diepdma0_reg_t;

typedef union {
    struct {
        uint32_t ineptxfspcavail: 16;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} usb_dwc_dtxfsts0_reg_t;

typedef union {
    struct {
        uint32_t dmabufferaddr;
    };
    uint32_t val;
} usb_dwc_diepdmab0_reg_t;

typedef union {
    struct {
        uint32_t mps: 11;
        uint32_t reserved_11: 4;
        uint32_t usbactep: 1;
        uint32_t dpid: 1;
        uint32_t naksts: 1;
        uint32_t eptype: 2;
        uint32_t reserved_20: 1;
        uint32_t stall: 1;
        uint32_t txfnum: 4;
        uint32_t cnak: 1;
        uint32_t snak: 1;
        uint32_t setd0pid: 1;
        uint32_t setd1pid: 1;
        uint32_t epdis: 1;
        uint32_t epena: 1;
    };
    uint32_t val;
} usb_dwc_diepctl_reg_t;

typedef union {
    struct {
        uint32_t xfercompl: 1;
        uint32_t epdisbld: 1;
        uint32_t ahberr: 1;
        uint32_t timeout: 1;
        uint32_t intkntxfemp: 1;
        uint32_t intknepmis: 1;
        uint32_t inepnakeff: 1;
        uint32_t txfemp: 1;
        uint32_t txfifoundrn: 1;
        uint32_t bnaintr: 1;
        uint32_t reserved_10: 1;
        uint32_t pktdrpsts: 1;
        uint32_t bbleerr: 1;
        uint32_t nakintrpt: 1;
        uint32_t nyetintrpt: 1;
        uint32_t reserved_15: 17;
    };
    uint32_t val;
} usb_dwc_diepint_reg_t;

typedef union {
    struct {
        uint32_t xfersize: 19;  // Note: Width depends on OTG_TRANS_COUNT_WIDTH (see databook).
        uint32_t pktcnt: 10;    // Note: Width depends on OTG_PACKET_COUNT_WIDTH (see databook).
        uint32_t mc: 2;
        uint32_t reserved: 1;
    };
    uint32_t val;
} usb_dwc_dieptsiz_reg_t;

typedef union {
    struct {
        uint32_t dmaddr;
    };
    uint32_t val;
} usb_dwc_diepdma_reg_t;

typedef union {
    struct {
        uint32_t ineptxfspcavail: 16;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} usb_dwc_dtxfsts_reg_t;

typedef union {
    struct {
        uint32_t dmabufferaddr;
    };
    uint32_t val;
} usb_dwc_diepdmab_reg_t;

typedef union {
    struct {
        uint32_t mps: 2;
        uint32_t reserved_2: 13;
        uint32_t usbactep: 1;
        uint32_t reserved_16: 1;
        uint32_t naksts: 1;
        uint32_t eptype: 2;
        uint32_t snp: 1;
        uint32_t stall: 1;
        uint32_t reserved_22: 4;
        uint32_t cnak: 1;
        uint32_t snak: 1;
        uint32_t reserved_28: 2;
        uint32_t epdis: 1;
        uint32_t epena: 1;
    };
    uint32_t val;
} usb_dwc_doepctl0_reg_t;

typedef union {
    struct {
        uint32_t xfercompl: 1;
        uint32_t epdisbld: 1;
        uint32_t ahberr: 1;
        uint32_t setup: 1;
        uint32_t outtknepdis: 1;
        uint32_t stsphsercvd: 1;
        uint32_t back2backsetup: 1;
        uint32_t reserved_7: 1;
        uint32_t outpkterr: 1;
        uint32_t bnaintr: 1;
        uint32_t reserved_10: 1;
        uint32_t pktdrpsts: 1;
        uint32_t bbleerr: 1;
        uint32_t nakintrpt: 1;
        uint32_t nyepintrpt: 1;
        uint32_t stuppktrcvd: 1;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} usb_dwc_doepint0_reg_t;

typedef union {
    struct {
        uint32_t xfersize: 7;
        uint32_t reserved_7: 12;
        uint32_t pktcnt: 1;
        uint32_t reserved_20: 9;
        uint32_t supcnt: 2;
        uint32_t reserved_31: 1;
    };
    uint32_t val;
} usb_dwc_doeptsiz0_reg_t;

typedef union {
    struct {
        uint32_t dmaaddr;
    };
    uint32_t val;
} usb_dwc_doepdma0_reg_t;

typedef union {
    struct {
        uint32_t dmabufferaddr;
    };
    uint32_t val;
} usb_dwc_doepdmab0_reg_t;

typedef union {
    struct {
        uint32_t mps: 11;
        uint32_t reserved_11: 4;
        uint32_t usbactep: 1;
        uint32_t dpid: 1;
        uint32_t naksts: 1;
        uint32_t eptype: 2;
        uint32_t snp: 1;
        uint32_t stall: 1;
        uint32_t reserved_22: 4;
        uint32_t cnak: 1;
        uint32_t snak: 1;
        uint32_t setd0pid: 1;
        uint32_t setd1pid: 1;
        uint32_t epdis: 1;
        uint32_t epena: 1;
    };
    uint32_t val;
} usb_dwc_doepctl_reg_t;

typedef union {
    struct {
        uint32_t xfercompl: 1;
        uint32_t epdisbld: 1;
        uint32_t ahberr: 1;
        uint32_t setup: 1;
        uint32_t outtknepdis: 1;
        uint32_t stsphsercvd: 1;
        uint32_t back2backsetup: 1;
        uint32_t reserved_7: 1;
        uint32_t outpkterr: 1;
        uint32_t bnaintr: 1;
        uint32_t reserved_10: 1;
        uint32_t pktdrpsts: 1;
        uint32_t bbleerr: 1;
        uint32_t nakintrpt: 1;
        uint32_t nyetintrpt: 1;
        uint32_t stuppktrcvd: 1;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} usb_dwc_doepint_reg_t;

typedef union {
    struct {
        uint32_t xfersize: 19;  // Note: Width depends on OTG_TRANS_COUNT_WIDTH (see databook).
        uint32_t pktcnt: 10;    // Note: Width depends on OTG_PACKET_COUNT_WIDTH (see databook).
        uint32_t rxdpid: 2;
        uint32_t reserved_31: 1;
    };
    uint32_t val;
} usb_dwc_doeptsiz_reg_t;

typedef union {
    struct {
        uint32_t dmaaddr;
    };
    uint32_t val;
} usb_dwc_doepdma_reg_t;

typedef union {
    struct {
        uint32_t dmabufferaddr;
    };
    uint32_t val;
} usb_dwc_doepdmab_reg_t;

typedef union {
    struct {
        uint32_t stoppclk: 1;
        uint32_t gatehclk: 1;
        uint32_t pwrclmp: 1;
        uint32_t rstpdwnmodule: 1;
        uint32_t reserved_4: 1;
        uint32_t reserved_5: 1;
        uint32_t physleep: 1;
        uint32_t l1suspended: 1;
        uint32_t resetaftersusp: 1;
        uint32_t reserved_9: 1;
        uint32_t reserved_10: 1;
        uint32_t reserved_11: 1;
        uint32_t reserved_12: 1;
        uint32_t reserved_13: 1;
        uint32_t reserved_14: 18;
    };
    uint32_t val;
} usb_dwc_pcgcctl_reg_t;

/* --------------------------- Register Groups ------------------------------ */

typedef struct {
    volatile usb_dwc_hcchar_reg_t hcchar_reg;       // 0x00
    volatile usb_dwc_hcsplt_reg_t hcsplt_reg;       // 0x04
    volatile usb_dwc_hcint_reg_t hcint_reg;         // 0x08
    volatile usb_dwc_hcintmsk_reg_t hcintmsk_reg;   // 0x0c
    volatile usb_dwc_hctsiz_reg_t hctsiz_reg;       // 0x10
    volatile usb_dwc_hcdma_reg_t hcdma_reg;         // 0x14
    uint32_t reserved_0x18[1];                      // 0x18
    volatile usb_dwc_hcdmab_reg_t hcdmab_reg;       // 0x1c
} usb_dwc_host_chan_regs_t;

typedef struct {
    volatile usb_dwc_diepctl_reg_t diepctl_reg;     // 0x00
    uint32_t reserved_0x04[1];                      // 0x04
    volatile usb_dwc_diepint_reg_t diepint_reg;     // 0x08
    uint32_t reserved_0x0c[1];                      // 0x0c
    volatile usb_dwc_dieptsiz_reg_t dieptsiz_reg;   // 0x10
    volatile usb_dwc_diepdma_reg_t diepdma_reg;     // 0x14
    volatile usb_dwc_dtxfsts_reg_t dtxfsts_reg;     // 0x18
    volatile usb_dwc_diepdmab_reg_t diepdmab_reg;   // 0x1c
} usb_dwc_in_ep_regs_t;

typedef struct {
    volatile usb_dwc_doepctl_reg_t doepctl_reg;     // 0x00
    uint32_t reserved_0x04[1];                      // 0x04
    volatile usb_dwc_doepint_reg_t doepint_reg;     // 0x08
    uint32_t reserved_0x0c[1];                      // 0x0c
    volatile usb_dwc_doeptsiz_reg_t doeptsiz_reg;   // 0x10
    volatile usb_dwc_doepdma_reg_t doepdma_reg;     // 0x14
    uint32_t reserved_0x18[1];                      // 0x18
    volatile usb_dwc_doepdmab_reg_t doepdmab_reg;   // 0x1c
} usb_dwc_out_ep_regs_t;

/* --------------------------- Register Layout ------------------------------ */

typedef struct {
    // Global Registers
    volatile usb_dwc_gotgctl_reg_t gotgctl_reg;             // 0x0000
    volatile usb_dwc_gotgint_reg_t gotgint_reg;             // 0x0004
    volatile usb_dwc_gahbcfg_reg_t gahbcfg_reg;             // 0x0008
    volatile usb_dwc_gusbcfg_reg_t gusbcfg_reg;             // 0x000c
    volatile usb_dwc_grstctl_reg_t grstctl_reg;             // 0x0010
    volatile usb_dwc_gintsts_reg_t gintsts_reg;             // 0x0014
    volatile usb_dwc_gintmsk_reg_t gintmsk_reg;             // 0x0018
    volatile usb_dwc_grxstsr_reg_t grxstsr_reg;             // 0x001c
    volatile usb_dwc_grxstsp_reg_t grxstsp_reg;             // 0x0020
    volatile usb_dwc_grxfsiz_reg_t grxfsiz_reg;             // 0x0024
    volatile usb_dwc_gnptxfsiz_reg_t gnptxfsiz_reg;         // 0x0028
    volatile usb_dwc_gnptxsts_reg_t gnptxsts_reg;           // 0x002c
    uint32_t reserved_0x0030;                               // 0x0030
    volatile usb_dwc_gpvndctl_reg_t gpvndctl_reg;           // 0x0034
    uint32_t reserved_0x0038;                               // 0x0038
    uint32_t reserved_0x003c;                               // 0x003c
    volatile usb_dwc_gsnpsid_reg_t gsnpsid_reg;             // 0x0040
    volatile usb_dwc_ghwcfg1_reg_t ghwcfg1_reg;             // 0x0044
    volatile usb_dwc_ghwcfg2_reg_t ghwcfg2_reg;             // 0x0048
    volatile usb_dwc_ghwcfg3_reg_t ghwcfg3_reg;             // 0x004c
    volatile usb_dwc_ghwcfg4_reg_t ghwcfg4_reg;             // 0x0050
    uint32_t reserved_0x0054;                               // 0x0054
    volatile usb_dwc_gpwrdn_reg_t gpwrdn_reg;               // 0x0058
    volatile usb_dwc_gdfifocfg_reg_t gdfifocfg_reg;         // 0x005c
    volatile usb_dwc_gadpctl_reg_t gadpctl_reg;             // 0x0060
    uint32_t reserved_0x0064_0x0100[39];                    // 0x0064 to 0x0100
    volatile usb_dwc_hptxfsiz_reg_t hptxfsiz_reg;           // 0x0100
    volatile usb_dwc_dieptxfi_reg_t dieptxfi_regs[7];       // 0x0104 to 0x011c (depends on OTG_NUM_IN_EPS)
    usb_dwc_dieptxfi_reg_t reserved_0x0120_0x013c[8];       // 0x0120 to 0x013c (depends on OTG_NUM_IN_EPS)
    uint32_t reserved_0x140_0x3fc[176];                     // 0x0140 to 0x03fc

    // Host Mode Registers
    volatile usb_dwc_hcfg_reg_t hcfg_reg;                   // 0x0400
    volatile usb_dwc_hfir_reg_t hfir_reg;                   // 0x0404
    volatile usb_dwc_hfnum_reg_t hfnum_reg;                 // 0x0408
    uint32_t reserved_0x40c[1];                             // 0x040c
    volatile usb_dwc_hptxsts_reg_t hptxsts_reg;             // 0x0410
    volatile usb_dwc_haint_reg_t haint_reg;                 // 0x0414
    volatile usb_dwc_haintmsk_reg_t haintmsk_reg;           // 0x0418
    volatile usb_dwc_hflbaddr_reg_t hflbaddr_reg;           // 0x041c
    uint32_t reserved_0x420_0x43c[8];                       // 0x0420 to 0x043c
    volatile usb_dwc_hprt_reg_t hprt_reg;                   // 0x0440
    uint32_t reserved_0x0444_0x04fc[47];                    // 0x0444 to 0x04fc

    // Host Channel Registers
    usb_dwc_host_chan_regs_t host_chans[16];                // 0x0500 to 0x06fc (depends on OTG_NUM_HOST_CHAN)
    uint32_t reserved_0x0704_0x07fc[64];                    // 0x0700 to 0x07fc

    // Device Mode Registers
    volatile usb_dwc_dcfg_reg_t dcfg_reg;                   // 0x0800
    volatile usb_dwc_dctl_reg_t dctl_reg;                   // 0x0804
    volatile usb_dwc_dsts_reg_t dsts_reg;                   // 0x0808
    uint32_t reserved_0x080c[1];                            // 0x080c
    volatile usb_dwc_diepmsk_reg_t diepmsk_reg;             // 0x0810
    volatile usb_dwc_doepmsk_reg_t doepmsk_reg;             // 0x0814
    volatile usb_dwc_daint_reg_t daint_reg;                 // 0x0818
    volatile usb_dwc_daintmsk_reg_t daintmsk_reg;           // 0x081c
    uint32_t reserved_0x0820;                               // 0x0820
    uint32_t reserved_0x0824;                               // 0x0824
    volatile usb_dwc_dvbusdis_reg_t dvbusdis_reg;           // 0x0828
    volatile usb_dwc_dvbuspulse_reg_t dvbuspulse_reg;       // 0x082c
    volatile usb_dwc_dthrctl_reg_t dthrctl_reg;             // 0x0830
    volatile usb_dwc_diepempmsk_reg_t diepempmsk_reg;       // 0x0834
    volatile usb_dwc_deachint_reg_t deachint_reg;           // 0x0838
    volatile usb_dwc_deachintmsk_reg_t deachintmsk_reg;     // 0x083c
    volatile usb_dwc_diepeachmski_reg_t diepeachmsk0_reg;   // 0x0840
    volatile usb_dwc_diepeachmski_reg_t diepeachmsk[15];    // 0x0844 to 0x087c (depends on OTG_NUM_EPS)
    volatile usb_dwc_doepeachmski_reg_t doepeachmsk0_reg;   // 0x0880
    volatile usb_dwc_doepeachmski_reg_t doepeachmsk[15];    // 0x0884 to 0x08c0 (depends on OTG_NUM_EPS)
    uint32_t reserved_0x08c4_0x08fc[16];                    // 0x08c4 to 0x08fc

    // Device: IN EP0 registers
    volatile usb_dwc_diepctl0_reg_t diepctl0_reg;           // 0x0900
    uint32_t reserved_0x0904[1];                            // 0x0904
    volatile usb_dwc_diepint0_reg_t diepint0_reg;           // 0x0908
    uint32_t reserved_0x090c[1];                            // 0x090c
    volatile usb_dwc_dieptsiz0_reg_t dieptsiz0_reg;         // 0x0910
    volatile usb_dwc_diepdma0_reg_t diepdma0_reg;           // 0x0914
    volatile usb_dwc_dtxfsts0_reg_t dtxfsts0_reg;           // 0x0918
    volatile usb_dwc_diepdmab0_reg_t diepdmab0_reg;         // 0x091c

    // Device: IN EP registers
    usb_dwc_in_ep_regs_t in_eps[15];                        // 0x0920 to 0x0afc (depends on OTG_NUM_EPS)

    // Device: OUT EP0 registers
    volatile usb_dwc_doepctl0_reg_t doepctl0_reg;           // 0x0b00
    uint32_t reserved_0x0b04[1];                            // 0x0b04
    volatile usb_dwc_doepint0_reg_t doepint0_reg;           // 0b0b08
    uint32_t reserved_0x0b0c[1];                            // 0x0b0c
    volatile usb_dwc_doeptsiz0_reg_t doeptsiz0_reg;         // 0x0b10
    volatile usb_dwc_doepdma0_reg_t doepdma0_reg;           // 0x0b14
    uint32_t reserved_0x0b18[1];                            // 0x0b18
    volatile usb_dwc_doepdmab0_reg_t doepdmab0_reg;         // 0x0b1c

    // Device: OUT EP registers
    usb_dwc_out_ep_regs_t out_eps[15];                      // 0x0b20 to 0x0cfc
    uint32_t reserved_0x0d00_0x0dfc[64];                    // 0x0d00 to 0x0dfc

    // Power and Clock Gating
    volatile usb_dwc_pcgcctl_reg_t pcgcctl_reg;             // 0x0e00
    uint32_t reserved_0x0e04[1];                            // 0x0e04
} usb_dwc_dev_t;

#ifndef __cplusplus
_Static_assert(sizeof(usb_dwc_dev_t) == 0xe08, "Invalid size of usb_dwc_dev_t structure");
#endif

extern usb_dwc_dev_t USB_DWC_HS;
extern usb_dwc_dev_t USB_DWC_FS;

#ifdef __cplusplus
}
#endif
