/*********************************************************************************************
 *   Filename        : p33.h

 *   Description     :

 *   Author          : Bingquan

 *   Email           : caibingquan@zh-jieli.com

 *   Last modifiled  : 2019-12-09 10:42

 *   Copyright:(c)JIELI  2011-2019  @ , All Rights Reserved.
 *********************************************************************************************/

#ifndef __P33__
#define __P33__


#define P33_ACCESS(x) (*(volatile u8 *)(0x1A0000 + 0x3800 + x))
#define RTC_ACCESS(x) (*(volatile u8 *)(0x1A0000 + 0x3400 + x))

//===============================================================================//
//
//					 p33 analog
//
//===============================================================================//
//............. 0x0000 - 0x000f............

//............. 0x0010 - 0x001f............ for analog others
#define P3_OSL_CON                        P33_ACCESS(0x10)
#define P3_VLVD_CON                       P33_ACCESS(0x11)
#define P3_RST_SRC                        P33_ACCESS(0x12)
#define P3_LRC_CON0                       P33_ACCESS(0x13)
#define P3_LRC_CON1                       P33_ACCESS(0x14)
#define P3_RST_CON0                       P33_ACCESS(0x15)
#define P3_ANA_KEEP                       P33_ACCESS(0x16)
#define P3_VLD_KEEP                       P33_ACCESS(0x17)
#define P3_CLK_CON0                       P33_ACCESS(0x18)
#define P3_ANA_READ                       P33_ACCESS(0x19)
#define P3_CHG_CON0                       P33_ACCESS(0x1a)
#define P3_CHG_CON1                       P33_ACCESS(0x1b)
#define P3_CHG_CON2                       P33_ACCESS(0x1c)
#define P3_CHG_CON3                       P33_ACCESS(0x1d)

//............. 0x0020 - 0x002f............ for PWM LED
//#define P3_PWM_CON0                       P33_ACCESS(0x20)
//#define P3_PWM_CON1                       P33_ACCESS(0x21)
//#define P3_PWM_CON2                       P33_ACCESS(0x22)
//#define P3_PWM_CON3                       P33_ACCESS(0x23)
//#define P3_PWM_BRI_PRDL                   P33_ACCESS(0x24)
//#define P3_PWM_BRI_PRDH                   P33_ACCESS(0x25)
//#define P3_PWM_BRI_DUTY0L                 P33_ACCESS(0x26)
//#define P3_PWM_BRI_DUTY0H                 P33_ACCESS(0x27)
//#define P3_PWM_BRI_DUTY1L                 P33_ACCESS(0x28)
//#define P3_PWM_BRI_DUTY1H                 P33_ACCESS(0x29)
//#define P3_PWM_PRD_DIVL                   P33_ACCESS(0x2a)
//#define P3_PWM_DUTY0                      P33_ACCESS(0x2b)
//#define P3_PWM_DUTY1                      P33_ACCESS(0x2c)
//#define P3_PWM_DUTY2                      P33_ACCESS(0x2d)
//#define P3_PWM_DUTY3                      P33_ACCESS(0x2e)
//#define P3_PWM_CNT_RD                     P33_ACCESS(0x2f)

//............. 0x0030 - 0x003f............ for PMU manager
#define P3_PMU_CON0                       P33_ACCESS(0x30)

#define P3_SFLAG0                         P33_ACCESS(0x38)
#define P3_SFLAG1                         P33_ACCESS(0x39)
//#define P3_SFLAG2                         P33_ACCESS(0x3a)
//#define P3_SFLAG3                         P33_ACCESS(0x3b)
//#define P3_SFLAG4                         P33_ACCESS(0x3c)
//#define P3_SFLAG5                         P33_ACCESS(0x3d)
//#define P3_SFLAG6                         P33_ACCESS(0x3e)
//#define P3_SFLAG7                         P33_ACCESS(0x3f)

//............. 0x0040 - 0x004f............ for
#define P3_IVS_RD                         P33_ACCESS(0x40)
#define P3_IVS_SET                        P33_ACCESS(0x41)
#define P3_IVS_CLR                        P33_ACCESS(0x42)
#define P3_PVDD0_AUTO                     P33_ACCESS(0x43)
#define P3_PVDD1_AUTO                     P33_ACCESS(0x44)

#define P3_WKUP_DLY                       P33_ACCESS(0x45)
#define P3_VLVD_FLT                       P33_ACCESS(0x46)
#define P3_PINR_CON1                      P33_ACCESS(0x47)
#define P3_PINR_CON                       P33_ACCESS(0x48)
#define P3_PCNT_CON                       P33_ACCESS(0x49)
#define P3_PCNT_SET0                      P33_ACCESS(0x4a)
#define P3_PCNT_SET1                      P33_ACCESS(0x4b)
#define P3_PCNT_DAT0                      P33_ACCESS(0x4c)
#define P3_PCNT_DAT1                      P33_ACCESS(0x4d)

//............. 0x0050 - 0x005f............ for port wake up
#define P3_WKUP_EN0                       P33_ACCESS(0x50)
#define P3_WKUP_EN1                       P33_ACCESS(0x51)
#define P3_WKUP_EDGE0                     P33_ACCESS(0x52)
#define P3_WKUP_EDGE1                     P33_ACCESS(0x53)
#define P3_WKUP_LEVEL0                    P33_ACCESS(0x54)
#define P3_WKUP_LEVEL1                    P33_ACCESS(0x55)
#define P3_WKUP_PND0                      P33_ACCESS(0x56)
#define P3_WKUP_PND1                      P33_ACCESS(0x57)
#define P3_WKUP_CPND0                     P33_ACCESS(0x58)
#define P3_WKUP_CPND1                     P33_ACCESS(0x59)

//............. 0x0060 - 0x006f............ for
#define P3_AWKUP_EN                       P33_ACCESS(0x60)
#define P3_AWKUP_P_IE                     P33_ACCESS(0x61)
#define P3_AWKUP_N_IE                     P33_ACCESS(0x62)
#define P3_AWKUP_LEVEL                    P33_ACCESS(0x63)
#define P3_AWKUP_INSEL                    P33_ACCESS(0x64)
#define P3_AWKUP_P_PND                    P33_ACCESS(0x65)
#define P3_AWKUP_N_PND                    P33_ACCESS(0x66)
#define P3_AWKUP_P_CPND                   P33_ACCESS(0x67)
#define P3_AWKUP_N_CPND                   P33_ACCESS(0x68)

//............. 0x0070 - 0x007f............ for power gate
//#define P3_PGDR_CON0                      P33_ACCESS(0x70)
//#define P3_PGDR_CON1                      P33_ACCESS(0x71)
#define P3_PGSD_CON                       P33_ACCESS(0x72)
//#define P3_PGFS_CON                       P33_ACCESS(0x73)

//............. 0x0080 - 0x008f............ for
#define P3_AWKUP_FLT0                     P33_ACCESS(0x80)
#define P3_AWKUP_FLT1                     P33_ACCESS(0x81)
#define P3_AWKUP_FLT2                     P33_ACCESS(0x82)

#define P3_APORT_SEL0                     P33_ACCESS(0x88)
#define P3_APORT_SEL1                     P33_ACCESS(0x89)
#define P3_APORT_SEL2                     P33_ACCESS(0x8a)

//............. 0x0090 - 0x009f............ for analog control
#define P3_ANA_CON0                       P33_ACCESS(0x90)
#define P3_ANA_CON1                       P33_ACCESS(0x91)
#define P3_ANA_CON2                       P33_ACCESS(0x92)
#define P3_ANA_CON3                       P33_ACCESS(0x93)
#define P3_ANA_CON4                       P33_ACCESS(0x94)
#define P3_ANA_CON5                       P33_ACCESS(0x95)
#define P3_ANA_CON6                       P33_ACCESS(0x96)
#define P3_ANA_CON7                       P33_ACCESS(0x97)
#define P3_ANA_CON8                       P33_ACCESS(0x98)
#define P3_ANA_CON9                       P33_ACCESS(0x99)
#define P3_ANA_CON10                      P33_ACCESS(0x9a)
#define P3_ANA_CON11                      P33_ACCESS(0x9b)
#define P3_ANA_CON12                      P33_ACCESS(0x9c)
#define P3_ANA_CON13                      P33_ACCESS(0x9d)
#define P3_ANA_CON14                      P33_ACCESS(0x9e)
#define P3_ANA_CON15                      P33_ACCESS(0x9f)

//............. 0x00a0 - 0x00af............
#define P3_PR_PWR                         P33_ACCESS(0xa0)
#define P3_L5V_CON0                       P33_ACCESS(0xa1)
#define P3_L5V_CON1                       P33_ACCESS(0xa2)

#define P3_LS_P11                         P33_ACCESS(0xa4)

#define P3_WKUP_SRC                       P33_ACCESS(0xa8)
#define P3_ANA_CON16                      P33_ACCESS(0xa9)
#define P3_PMU_DBG_CON                    P33_ACCESS(0xaa)

//............. 0x00b0 - 0x00bf............ for EFUSE
#define P3_EFUSE_CON0                     P33_ACCESS(0xb0)
#define P3_EFUSE_CON1                     P33_ACCESS(0xb1)
#define P3_EFUSE_RDAT                     P33_ACCESS(0xb2)
#define P3_FUNC_EN                        P33_ACCESS(0xb8)

//............. 0x00c0 - 0x00cf............ for port input select
#define P3_PORT_SEL0                      P33_ACCESS(0xc0)
#define P3_PORT_SEL1                      P33_ACCESS(0xc1)
#define P3_PORT_SEL2                      P33_ACCESS(0xc2)
#define P3_PORT_SEL3                      P33_ACCESS(0xc3)
#define P3_PORT_SEL4                      P33_ACCESS(0xc4)
#define P3_PORT_SEL5                      P33_ACCESS(0xc5)
#define P3_PORT_SEL6                      P33_ACCESS(0xc6)
#define P3_PORT_SEL7                      P33_ACCESS(0xc7)
#define P3_PORT_SEL8                      P33_ACCESS(0xc8)
#define P3_PORT_SEL9                      P33_ACCESS(0xc9)
#define P3_PORT_SEL10                     P33_ACCESS(0xca)
#define P3_PORT_SEL11                     P33_ACCESS(0xcb)
#define P3_PORT_SEL12                     P33_ACCESS(0xcc)
#define P3_PORT_SEL13                     P33_ACCESS(0xcd)
#define P3_PORT_SEL14                     P33_ACCESS(0xce)
#define P3_PORT_SEL15                     P33_ACCESS(0xcf)

//............. 0x00d0 - 0x00df............
#define P3_LS_IO_DLY                      P33_ACCESS(0xd0)    //TODO: check sync with verilog head file chip_def.v  LEVEL_SHIFTER
#define P3_LS_IO_ROM                      P33_ACCESS(0xd1)
#define P3_LS_ADC                         P33_ACCESS(0xd2)
#define P3_LS_AUDIO                       P33_ACCESS(0xd3)
#define P3_LS_RF                          P33_ACCESS(0xd4)

//............. 0x00e0 - 0x00ef............


//===============================================================================//
//
//                      p33 rtcvdd
//
//===============================================================================//
//............. 0x0080 - 0x008f............ for RTC
#define R3_ALM_CON                        RTC_ACCESS(0x80)

#define R3_RTC_CON0                       RTC_ACCESS(0x84)
#define R3_RTC_CON1                       RTC_ACCESS(0x85)
#define R3_RTC_DAT0                       RTC_ACCESS(0x86)
#define R3_RTC_DAT1                       RTC_ACCESS(0x87)
#define R3_RTC_DAT2                       RTC_ACCESS(0x88)
#define R3_RTC_DAT3                       RTC_ACCESS(0x89)
#define R3_RTC_DAT4                       RTC_ACCESS(0x8a)
#define R3_ALM_DAT0                       RTC_ACCESS(0x8b)
#define R3_ALM_DAT1                       RTC_ACCESS(0x8c)
#define R3_ALM_DAT2                       RTC_ACCESS(0x8d)
#define R3_ALM_DAT3                       RTC_ACCESS(0x8e)
#define R3_ALM_DAT4                       RTC_ACCESS(0x8f)

//............. 0x0090 - 0x009f............ for PORT control
#define R3_WKUP_EN                        RTC_ACCESS(0x90)
#define R3_WKUP_EDGE                      RTC_ACCESS(0x91)
#define R3_WKUP_CPND                      RTC_ACCESS(0x92)
#define R3_WKUP_PND                       RTC_ACCESS(0x93)
#define R3_WKUP_FLEN                      RTC_ACCESS(0x94)
#define R3_PORT_FLT                       RTC_ACCESS(0x95)

#define R3_PR_IN                          RTC_ACCESS(0x98)
#define R3_PR_OUT                         RTC_ACCESS(0x99)
#define R3_PR_DIR                         RTC_ACCESS(0x9a)
#define R3_PR_DIE                         RTC_ACCESS(0x9b)
#define R3_PR_PU                          RTC_ACCESS(0x9c)
#define R3_PR_PD                          RTC_ACCESS(0x9d)
#define R3_PR_HD                          RTC_ACCESS(0x9e)

//............. 0x00a0 - 0x00af............ for system
#define R3_TIME_CON                       RTC_ACCESS(0xa0)
#define R3_TIME_CPND                      RTC_ACCESS(0xa1)
#define R3_TIME_PND                       RTC_ACCESS(0xa2)

#define R3_ADC_CON                        RTC_ACCESS(0xa4)
#define R3_OSL_CON                        RTC_ACCESS(0xa5)

#define R3_WKUP_SRC                       RTC_ACCESS(0xa8)
#define R3_RST_SRC                        RTC_ACCESS(0xa9)

#define R3_RST_CON                        RTC_ACCESS(0xab)
#define R3_CLK_CON                        RTC_ACCESS(0xac)

////////////////////////////////

//ROM
u8 p33_buf(u8 buf);

// void p33_xor_1byte(u16 addr, u8 data0);
// #define p33_xor_1byte(addr, data0)      (*((volatile u8 *)&addr + 0x300)  = data0)
#define p33_xor_1byte(addr, data0)      addr ^= (data0)

// void p33_and_1byte(u16 addr, u8 data0);
// #define p33_and_1byte(addr, data0)      (*((volatile u8 *)&addr + 0x100)  = (data0))
#define p33_and_1byte(addr, data0)      addr &= (data0)

// void p33_or_1byte(u16 addr, u8 data0);
// #define p33_or_1byte(addr, data0)       (*((volatile u8 *)&addr + 0x200)  = data0)
#define p33_or_1byte(addr, data0)       addr |= (data0)

// void p33_tx_1byte(u16 addr, u8 data0);
#define p33_tx_1byte(addr, data0)       addr = data0

// u8 p33_rx_1byte(u16 addr);
#define p33_rx_1byte(addr)              addr

#define P33_CON_SET(sfr, start, len, data)  (sfr = (sfr & ~((~(0xff << (len))) << (start))) | \
	 (((data) & (~(0xff << (len)))) << (start)))

#define P33_CON_GET(sfr)    sfr


void SET_WVDD_LEV(u8 lev);

void RESET_MASK_SW(u8 sw);

void close_32K(u8 keep_osci_flag);

void evdd_output_2_pb3(u8 en, u8 vol_sel);

//pvdd强驱挡位选择, hd_val范围: 0 ~ 3
void evdd_hd_select(u8 hd_val);

//电压挡位请在adc_api.c 中 pvdd_trim函数修改
void pvdd_output_2_pb3(u8 en);

//pvdd强驱挡位选择, hd_val范围: 0 ~ 7
void pvdd_hd_select(u8 hd_val);

void P3_WKUP_EN(u16 index, u8 en);

void P3_WKUP_EDGE(u16 index, u8 falling);

void P3_WKUP_CPND(u16 index);

//===============================================================================//
//
//      				p33 analog
//
//===============================================================================//

/*
 *-------------------P3_ANA_CON0
 */
#define VDD13TO12_SYS_EN(en)      P33_CON_SET(P3_ANA_CON0, 0, 1, en)

#define VDD13TO12_RVD_EN(en)      P33_CON_SET(P3_ANA_CON0, 1, 1, en)

#define LDO13_EN(en)              P33_CON_SET(P3_ANA_CON0, 2, 1, en)

#define DCDC13_EN(en)             P33_CON_SET(P3_ANA_CON0, 3, 1, en)

#define PVDD_EN(en)               P33_CON_SET(P3_ANA_CON0, 4, 1, en)

#define MVIO_VBAT_EN(en)          P33_CON_SET(P3_ANA_CON0, 5, 1, en)

#define MVIO_VPWR_EN(en)          P33_CON_SET(P3_ANA_CON0, 6, 1, en)

#define MBG_EN(en)				  P33_CON_SET(P3_ANA_CON0, 7, 1, en)

/*******************************************************************/

/*
 *-------------------P3_ANA_CON1
 */

#define RVDD_BYPASS_EN(en)      P33_CON_SET(P3_ANA_CON1, 0, 1, en)

#define WVDD_SHORT_RVDD(en)     P33_CON_SET(P3_ANA_CON1, 1, 1, en)

#define WVDD_SHORT_SVDD(en)     P33_CON_SET(P3_ANA_CON1, 2, 1, en)

#define WLDO06_EN(en)           P33_CON_SET(P3_ANA_CON1, 3, 1, en)

#define WLDO06_OE(en)           P33_CON_SET(P3_ANA_CON1, 4, 1, en)

#define EVD_EN(en)       		P33_CON_SET(P3_ANA_CON1, 5, 1, en)

#define EVD_SHORT_PB3(en)       P33_CON_SET(P3_ANA_CON1, 6, 1, en)

#define PVD_SHORT_PB3(en)       P33_CON_SET(P3_ANA_CON1, 7, 1, en)

/*******************************************************************/

/*
 *-------------------P3_ANA_CON2
 */
#define VCM_DET_EN(en)          P33_CON_SET(P3_ANA_CON2, 3, 1, en)

#define MVIO_VBAT_ILMT_EN(en)   P33_CON_SET(P3_ANA_CON2, 4, 1, en)

#define MVIO_VPWR_ILMT_EN(en)   P33_CON_SET(P3_ANA_CON2, 5, 1, en)

#define DCVD_ILMT_EN(en)        P33_CON_SET(P3_ANA_CON2, 6, 1, en)

#define CURRENT_LIMIT_DISABLE() (P3_ANA_CON2 &= ~(BIT(4) | BIT(5) | BIT(6)))

/********************************************************************/
/*
 *-------------------P3_LS_XX
 */
#define PVDD_ANA_LAT_EN(en)  \
	if(en){				  	 \
		P3_LS_P11 = 0x01; 	 \
		P3_LS_P11 = 0x03; 	 \
	}else{				  	 \
		P3_LS_P11 = 0x0;  	 \
	}

#define DVDD_ANA_LAT_EN(en)      \
	if(en){					  	 \
		P3_LS_IO_DLY      = 0x1; \
        P3_LS_IO_ROM      = 0x1; \
        P3_LS_ADC         = 0x1; \
		P3_LS_AUDIO       = 0x1; \
        P3_LS_RF          = 0x1; \
        P3_LS_IO_DLY      = 0x3; \
        P3_LS_IO_ROM      = 0x3; \
        P3_LS_ADC         = 0x3; \
		P3_LS_AUDIO       = 0x3; \
        P3_LS_RF          = 0x3; \
	}else{						 \
        P3_LS_IO_DLY      = 0x0; \
        P3_LS_IO_ROM      = 0x0; \
        P3_LS_ADC         = 0x0; \
		P3_LS_AUDIO       = 0x0; \
        P3_LS_RF          = 0x0; \
	}

#define DVDD_ANA_ROM_LAT_DISABLE() \
   		P3_LS_IO_ROM  = 0;		  \
    	P3_LS_RF      = 0


/*******************************************************************/

/*
 *-------------------P3_ANA_CON3
 */
#define MVBG_SEL(en)     		P33_CON_SET(P3_ANA_CON3, 0, 4, en)

#define MVBG_GET()     		    (P33_CON_GET(P3_ANA_CON3) & 0x0f)

#define WVBG_SEL(en)           	P33_CON_SET(P3_ANA_CON3, 4, 4, en)

/*******************************************************************/

/*
 *-------------------P3_ANA_CON4
 */
#define PMU_DET_EN(en)          P33_CON_SET(P3_ANA_CON4, 0, 1, en)

#define ADC_CHANNEL_SEL(ch)     P33_CON_SET(P3_ANA_CON4, 1, 4, ch)

#define PMU_DET_BG_BUF_EN(en)   P33_CON_SET(P3_ANA_CON4, 5, 1, en)

#define VBG_TEST_EN(en)   		P33_CON_SET(P3_ANA_CON4, 6, 1, en)

#define VBG_TEST_SEL(en)   		P33_CON_SET(P3_ANA_CON4, 7, 1, en)

/*******************************************************************/


/*
 *-------------------P3_ANA_CON5
 */
#define VDDIOM_VOL_SEL(lev)     P33_CON_SET(P3_ANA_CON5, 0, 3, lev)

#define GET_VDDIOM_VOL()        (P33_CON_GET(P3_ANA_CON5) & 0x7)

#define VDDIOW_VOL_SEL(lev)     P33_CON_SET(P3_ANA_CON5, 3, 3, lev)

#define GET_VDDIOW_VOL()        (P33_CON_GET(P3_ANA_CON5)>>3 & 0x7)

#define VDDIO_HD_SEL(cur)       P33_CON_SET(P3_ANA_CON5, 6, 2, cur)

/*******************************************************************/

/*
 *-------------------P3_ANA_CON6
 */
#define VDC13_VOL_SEL(sel)      P33_CON_SET(P3_ANA_CON6, 0, 4, sel)
//Macro for VDC13_VOL_SEL
enum {
    VDC13_VOL_SEL_100V = 0,
    VDC13_VOL_SEL_105V,
    VDC13_VOL_SEL_110V,
    VDC13_VOL_SEL_115V,
    VDC13_VOL_SEL_120V,
    VDC13_VOL_SEL_125V,
    VDC13_VOL_SEL_130V,
    VDC13_VOL_SEL_135V,
    VDC13_VOL_SEL_140V,
    VDC13_VOL_SEL_145V,
    VDC13_VOL_SEL_150V,
    VDC13_VOL_SEL_155V,
    VDC13_VOL_SEL_160V,
};

#define VD13_DEFAULT_VOL		VDC13_VOL_SEL_125V

#define GET_VD13_VOL_SEL()       (P33_CON_GET(P3_ANA_CON6) & 0xF)

#define VD13_HD_SEL(sel)        P33_CON_SET(P3_ANA_CON6, 4, 2, sel)

#define VD13_CAP_EN(en)         P33_CON_SET(P3_ANA_CON6, 6, 1, en)

#define VD13_DESHOT_EN(en)      P33_CON_SET(P3_ANA_CON6, 7, 1, en)
/*******************************************************************/

/*
 *-------------------P3_ANA_CON7
 */

#define BTDCDC_PFM_MODE(en)     P33_CON_SET(P3_ANA_CON7, 0, 1, en)

#define GET_BTDCDC_PFM_MODE()   (P33_CON_GET(P3_ANA_CON7) & BIT(0) ? 1 : 0)

#define BTDCDC_RAMP_EN(en)      P33_CON_SET(P3_ANA_CON7, 1, 1, en)

#define BTDCDC_DUTY_SEL(sel)    P33_CON_SET(P3_ANA_CON7, 3, 2, sel)

#define BTDCDC_OSC_SEL(sel)     P33_CON_SET(P3_ANA_CON7, 5, 3, sel)
//Macro for BTDCDC_OSC_SEL
enum {
    BTDCDC_OSC_SEL0537MHz = 0,
    BTDCDC_OSC_SEL0789MHz,
    BTDCDC_OSC_SEL1030MHz,
    BTDCDC_OSC_SEL1270MHz,
    BTDCDC_OSC_SEL1720MHz,
    BTDCDC_OSC_SEL1940MHz,
    BTDCDC_OSC_SEL2150MHz,
    BTDCDC_OSC_SEL2360MHz,
};
/*******************************************************************/

/*
 *-------------------P3_ANA_CON8
 */
#define BTDCDC_V21_RES_SEL(sel) P33_CON_SET(P3_ANA_CON8, 0, 2, sel)

#define BTDCDC_DT(sel)          P33_CON_SET(P3_ANA_CON8, 2, 2, sel)

#define BTDCDC_ISENSE_HD_SEL(sel) P33_CON_SET(P3_ANA_CON8, 4, 2, sel)

#define BTDCDC_COMP_HD(sel)     P33_CON_SET(P3_ANA_CON8, 6, 2, sel)
/*******************************************************************/

/*
 *-------------------P3_ANA_CON9
 */
#define BTDCDC_NMOS_SEL(sel)    P33_CON_SET(P3_ANA_CON9, 1, 3, sel)

#define BTDCDC_PMOS_SEL(sel)    P33_CON_SET(P3_ANA_CON9, 5, 3, sel)

/*******************************************************************/

/*
 *-------------------P3_ANA_CON10
 */
#define BTDCDC_OSC_TEST_OE(en)  P33_CON_SET(P3_ANA_CON10, 7, 1, en)

#define BTDCDC_HD_BIAS_SEL(sel) P33_CON_SET(P3_ANA_CON10, 5, 2, sel)

#define BTDCDC_CLK_SEL(sel)     P33_CON_SET(P3_ANA_CON10, 4, 1, sel)

#define GET_BTDCDC_CLK_SEL()    (P33_CON_GET(P3_ANA_CON10) & BIT(4) ? 1 : 0)

#define BTDCDC_ZCD_RES(sel)     P33_CON_SET(P3_ANA_CON10, 2, 2, sel)

#define BTDCDC_ZCD_EN(en)       P33_CON_SET(P3_ANA_CON10, 0, 1, en)

/*******************************************************************/

/*
 *-------------------P3_ANA_CON11
 */

#define SYSVDD_VOL_SEL(sel)     P33_CON_SET(P3_ANA_CON11, 0, 4, sel)
//Macro for SYSVDD_VOL_SEL
enum {
    SYSVDD_VOL_SEL_084V = 0,
    SYSVDD_VOL_SEL_087V,
    SYSVDD_VOL_SEL_090V,
    SYSVDD_VOL_SEL_093V,
    SYSVDD_VOL_SEL_096V,
    SYSVDD_VOL_SEL_099V,
    SYSVDD_VOL_SEL_102V,
    SYSVDD_VOL_SEL_105V,
    SYSVDD_VOL_SEL_108V,
    SYSVDD_VOL_SEL_111V,
    SYSVDD_VOL_SEL_114V,
    SYSVDD_VOL_SEL_117V,
    SYSVDD_VOL_SEL_120V,
    SYSVDD_VOL_SEL_123V,
    SYSVDD_VOL_SEL_126V,
    SYSVDD_VOL_SEL_129V,
};

#define SYSVDD_DEFAULT_VOL	SYSVDD_VOL_SEL_111V

#define GET_SYSVDD_VOL_SEL()     (P33_CON_GET(P3_ANA_CON11) & 0xf)

#define SYSVDD_VOL_HD_SEL(sel)   P33_CON_SET(P3_ANA_CON11, 4, 2, sel)

#define SYSVDD_CAP_EN(en)        P33_CON_SET(P3_ANA_CON11, 6, 1, en)

/*
 *-------------------P3_ANA_CON12
 */
#define RVDD_VOL_SEL(sel)       P33_CON_SET(P3_ANA_CON12, 0, 4, sel)
//Macro for SYSVDD_VOL_SEL
enum {
    RVDD_VOL_SEL_084V = 0,
    RVDD_VOL_SEL_087V,
    RVDD_VOL_SEL_090V,
    RVDD_VOL_SEL_093V,
    RVDD_VOL_SEL_096V,
    RVDD_VOL_SEL_099V,
    RVDD_VOL_SEL_102V,
    RVDD_VOL_SEL_105V,
    RVDD_VOL_SEL_108V,
    RVDD_VOL_SEL_111V,
    RVDD_VOL_SEL_114V,
    RVDD_VOL_SEL_117V,
    RVDD_VOL_SEL_120V,
    RVDD_VOL_SEL_123V,
    RVDD_VOL_SEL_126V,
    RVDD_VOL_SEL_129V,
};

#define RVDD_DEFAULT_VOL	RVDD_VOL_SEL_111V

#define GET_RVDD_VOL_SEL()      (P33_CON_GET(P3_ANA_CON12) & 0xf)

#define RVDD_VOL_HD_SEL(en)     P33_CON_SET(P3_ANA_CON12, 4, 2, en)

#define RVDD_CAP_EN(en)         P33_CON_SET(P3_ANA_CON12, 6, 1, en)
/*
 *-------------------P3_ANA_CON13
 */

#define WVDD_VOL_MIN		500
#define VWDD_VOL_MAX		1250
#define WVDD_VOL_STEP		50
#define WVDD_LEVEL_MAX	    0xf
#define WVDD_LEVEL_ERR      0xff
#define WVDD_LEVEL_DEFAULT  ((WVDD_VOL_TRIM-WVDD_VOL_MIN)/WVDD_VOL_STEP + 2)

#define WVDD_VOL_TRIM	    850

#define WVDD_VOL_SEL(sel)       P33_CON_SET(P3_ANA_CON13, 0, 4, sel)
//Macro for WVDD_VOL_SEL
enum {
    WVDD_VOL_SEL_050V = 0,
    WVDD_VOL_SEL_055V,
    WVDD_VOL_SEL_060V,
    WVDD_VOL_SEL_065V,
    WVDD_VOL_SEL_070V,
    WVDD_VOL_SEL_075V,
    WVDD_VOL_SEL_080V,
    WVDD_VOL_SEL_085V,
    WVDD_VOL_SEL_090V,
    WVDD_VOL_SEL_095V,
    WVDD_VOL_SEL_100V,
    WVDD_VOL_SEL_105V,
    WVDD_VOL_SEL_110V,
    WVDD_VOL_SEL_115V,
    WVDD_VOL_SEL_120V,
    WVDD_VOL_SEL_125V,
};

#define WVDD_LOAD_EN(en)        P33_CON_SET(P3_ANA_CON13, 4, 1, en)

#define WVDDIO_FBRES_AUTO(en)   P33_CON_SET(P3_ANA_CON13, 6, 1, en)

#define WVDDIO_FBRES_SEL_W(en)  P33_CON_SET(P3_ANA_CON13, 7, 1, en)

/*
 *-------------------P3_ANA_CON14
 */

#define PVD_DEUDSHT_EN(en)      P33_CON_SET(P3_ANA_CON14, 3, 1, en)

#define PVD_HD_SEL(sel)         P33_CON_SET(P3_ANA_CON14, 0, 3, sel)


/*
 *-------------------P3_ANA_CON15
 */
#define EVD_VOL_SEL(sel)       P33_CON_SET(P3_ANA_CON15, 0, 2, sel)
enum {
    EVD_VOL_SEL_100V = 0,
    EVD_VOL_SEL_105V,
    EVD_VOL_SEL_110V,
    EVD_VOL_SEL_115V,
};

#define GET_EVD_VOL()           (P33_CON_GET(P3_ANA_CON15) & 0x3)

#define EVD_HD_SEL(sel)         P33_CON_SET(P3_ANA_CON15, 2, 2, sel)

#define EVD_CAP_EN(en)          P33_CON_SET(P3_ANA_CON15, 4, 1, en)

/*
 *-------------------P3_ANA_CON16
 */
#define BTDCDC_V17_TEST_OE(en)  P33_CON_SET(P3_ANA_CON16, 0, 1, en)

/*
 *-------------------P3_PR_PWR
 */
#define	P3_SOFT_RESET()			P33_CON_SET(P3_PR_PWR, 4, 2, 3)

/*
 *-------------------P3_PVDD1_AUTO
 */
#define	P33_SF_KICK_START()		P33_CON_SET(P3_IVS_CLR, 0, 8, 0b10101000)


/*
 *-------------------P3_PVDD1_AUTO
 */

#define PVDD_VOL_MIN        500
#define PVDD_VOL_MAX		1250
#define PVDD_VOL_STEP       50
#define PVDD_LEVEL_MAX      0xf
#define PVDD_LEVEL_ERR		0xff
#define PVDD_LEVEL_DEFAULT  0xc

#define PVDD_VOL_TRIM                     1100//mV
#define PVDD_VOL_TRIM_LOW 				  800 //mv, 如果出现异常, 可以抬高该电压值
#define PVDD_LEVEL_TRIM_LOW 			  ((PVDD_VOL_TRIM - PVDD_VOL_TRIM_LOW) / PVDD_VOL_STEP)

/*
 *-------------------P3_VLVD_CON
 */
#define P33_VLVD_EN(en)         P33_CON_SET(P3_VLVD_CON, 0, 1, en)

#define P33_VLVD_PS(en)         P33_CON_SET(P3_VLVD_CON, 1, 1, en)

#define P33_VLVD_OE(en)         P33_CON_SET(P3_VLVD_CON, 2, 1, en)

#define VLVD_SEL(lev)           P33_CON_SET(P3_VLVD_CON, 3, 3, lev)
//Macro for VLVD_SEL
enum {
    VLVD_SEL_18V = 0,
    VLVD_SEL_19V,
    VLVD_SEL_20V,
    VLVD_SEL_21V,
    VLVD_SEL_22V,
    VLVD_SEL_23V,
    VLVD_SEL_24V,
    VLVD_SEL_25V,
};

#define VLVD_PND_CLR()       	P33_CON_SET(P3_VLVD_CON, 6, 1, 1)

#define VLVD_PND()          	((P33_CON_GET(P3_VLVD_CON) & BIT(7)) ? 1 : 0)

/*
 *-------------------P3_WKUP_DLY
 */
#define P3_WKUP_DLY_SET(val)	P33_CON_SET(P3_WKUP_DLY, 0, 3, val)


/*
 *-------------------P3_PCNT_CON
 */

#define PCNT_PND_CLR()		    P33_CON_SET(P3_PCNT_CON, 6, 1, 1)

/*
 *-------------------P3_PCNT_SET0
 */

#define	SET_EXCEPTION_FLAG()	 P33_CON_SET(P3_PCNT_SET0, 0, 8, 0xab)

#define GET_EXCEPTION_FLAG()	((P33_CON_GET(P3_PCNT_SET0) == 0xab) ? 1 : 0)
#define GET_ASSERT_FLAG()		((P33_CON_GET(P3_PCNT_SET0) == 0xac) ? 1 : 0)

#define SOFT_RESET_FLAG_CLEAR()	(P33_CON_SET(P3_PCNT_SET0, 0, 8, 0))

/*******************************************************************/

/*
 *-------------------P3_RST_CON0
 */
#define DPOR_MASK(en)           P33_CON_SET(P3_RST_CON0, 0, 1, en)

#define VLVD_RST_EN(en)         P33_CON_SET(P3_RST_CON0, 2, 1, en)

#define VLVD_WKUP_EN(en)        P33_CON_SET(P3_RST_CON0, 3, 1, en)

#define PPOR_MASK(en)           P33_CON_SET(P3_RST_CON0, 4, 1, en)

#define P11_TO_P33_RST_MASK(en) P33_CON_SET(P3_RST_CON0, 5, 1, en)

#define PVDDOK_OE(en) 			P33_CON_SET(P3_RST_CON0, 6, 1, en)

#define DVDDOK_OE(en) 			P33_CON_SET(P3_RST_CON0, 7, 1, en)

/*******************************************************************/

/*
 *-------------------P3_ANA_KEEP
 */
#define CLOSE_ANA_KEEP()	P33_CON_SET(P3_ANA_KEEP, 0, 8, 0)

/*******************************************************************/

/*
 *-------------------P3_LRC_CON0
 */
#define RC32K_EN(en)            P33_CON_SET(P3_LRC_CON0, 0, 1, en)

#define RC32K_RN_TRIM(en)       P33_CON_SET(P3_LRC_CON0, 1, 1, en)

#define RC32K_RPPS_SEL(sel)     P33_CON_SET(P3_LRC_CON0, 4, 2, sel)

/*******************************************************************/

/*
 *-------------------P3_LRC_CON1
 */
#define RC32K_PNPS_SEL(sel)     P33_CON_SET(P3_LRC_CON1, 0, 2, sel)

#define RC32K_CAP_SEL(sel)      P33_CON_SET(P3_LRC_CON1, 4, 3, sel)

#define CLOSE_LRC()				P33_CON_SET(P3_LRC_CON0, 0, 8, 0);\
								P33_CON_SET(P3_LRC_CON1, 0, 8, 0)

/*******************************************************************/

/*
 *-------------------P3_VLD_KEEP
 */
#define RTC_WKUP_KEEP(a)        P33_CON_SET(P3_VLD_KEEP, 1, 1, a)

#define P33_WKUP_P11_EN(a)          P33_CON_SET(P3_VLD_KEEP, 2, 1, a)

/*****************************************************************/

/*
 *-------------------P3_CLK_CON0
 */
#define RC_250K_EN(a)          P33_CON_SET(P3_CLK_CON0, 0, 1, a)


/*******************************************************************/

/*
 *-------------------P3_CHG_WKUP
 */
#define CHARGE_LEVEL_DETECT_EN(a)	P33_CON_SET(P3_CHG_WKUP, 0, 1, a)

#define CHARGE_EDGE_DETECT_EN(a)	P33_CON_SET(P3_CHG_WKUP, 1, 1, a)

#define CHARGE_WKUP_SOURCE_SEL(a)	P33_CON_SET(P3_CHG_WKUP, 2, 2, a)

#define CHARGE_WKUP_EN(a)			P33_CON_SET(P3_CHG_WKUP, 4, 1, a)

#define CHARGE_WKUP_EDGE_SEL(a)		P33_CON_SET(P3_CHG_WKUP, 5, 1, a)

#define CHARGE_WKUP_PND_CLR()		P33_CON_SET(P3_CHG_WKUP, 6, 1, 1)

/*
 *-------------------P3_AWKUP_LEVEL
 */
#define CHARGE_FULL_FILTER_GET()	((P33_CON_GET(P3_AWKUP_LEVEL) & BIT(2)) ? 1: 0)

#define LVCMP_DET_FILTER_GET()      ((P33_CON_GET(P3_AWKUP_LEVEL) & BIT(1)) ? 1: 0)

#define LDO5V_DET_FILTER_GET()      ((P33_CON_GET(P3_AWKUP_LEVEL) & BIT(0)) ? 1: 0)

/*
 *-------------------P3_ANA_READ
 */
#define CHARGE_FULL_FLAG_GET()		((P33_CON_GET(P3_ANA_READ) & BIT(0)) ? 1: 0 )

#define LVCMP_DET_GET()			    ((P33_CON_GET(P3_ANA_READ) & BIT(1)) ? 1: 0 )

#define LDO5V_DET_GET()			    ((P33_CON_GET(P3_ANA_READ) & BIT(2)) ? 1: 0 )

/*
 *-------------------P3_CHG_CON0
 */
#define BTDCDC_DELAYTIME_S0(sel)  P33_CON_SET(P3_CHG_CON0, 2, 1, sel)

#define CHARGE_EN(en)           P33_CON_SET(P3_CHG_CON0, 0, 1, en)

#define CHGGO_EN(en)            P33_CON_SET(P3_CHG_CON0, 1, 1, en)

#define IS_CHARGE_EN()			((P33_CON_GET(P3_CHG_CON0) & BIT(0)) ? 1: 0 )

#define CHG_HV_MODE(mode)       P33_CON_SET(P3_CHG_CON0, 2, 1, mode)

#define IS_CHG_HV_MODE()       ((P33_CON_GET(P3_CHG_CON0) & BIT(2)) ? 1: 0 )

/*
 *-------------------P3_CHG_CON1
 */
#define CHARGE_FULL_V_SEL(a)	P33_CON_SET(P3_CHG_CON1, 0, 4, a)

#define CHARGE_mA_SEL(a)		P33_CON_SET(P3_CHG_CON1, 4, 4, a)

#define GET_CHARGE_mA_SEL()     ((P33_CON_GET(P3_CHG_CON1)&0xf0)>>4)

#define GET_CHARGE_FULL_SET()   (P33_CON_GET(P3_CHG_CON1) & 0x0F)

/*
 *-------------------P3_CHG_CON2
 */
#define CHARGE_FULL_mA_SEL(a)	P33_CON_SET(P3_CHG_CON2, 4, 3, a)

/*
 *-------------------P3_L5V_CON0
 */
#define L5V_LOAD_EN(a)		    P33_CON_SET(P3_L5V_CON0, 0, 1, a)
#define IS_L5V_LOAD_EN()        ((P33_CON_GET(P3_L5V_CON0) & BIT(0)) ? 1: 0 )

/*
 *-------------------P3_L5V_CON1
 */
#define L5V_RES_DET_S_SEL(a)	P33_CON_SET(P3_L5V_CON1, 0, 2, a)
#define GET_L5V_RES_DET_S_SEL() (P33_CON_GET(P3_L5V_CON1) & 0x03)

/*
 *-------------------P3_PMU_CON0
 */
#define GET_P33_SYS_POWER_FLAG() ((P33_CON_GET(P3_PMU_CON0) & BIT(7)) ? 1 : 0)

#define P33_SYS_POWERUP_CLEAR()  P33_CON_SET(P3_PMU_CON0, 6, 1, 1);

/*
 *-------------------P3_RST_SRC
 */
#define GET_P33_SYS_RST_SRC()	P33_CON_GET(P3_RST_SRC)

//===============================================================================//
//
//      				p33 rtcvdd
//
//===============================================================================//

/*
 *-------------------R3_ALM_CON
 */
#define ALM_CLK_SEL(a)	P33_CON_SET(R3_ALM_CON, 2, 3, a)

#define ALM_ALMEN(a)	P33_CON_SET(R3_ALM_CON, 0, 1, a)


//Macro for CLK_SEL
enum RTC_CLK {
    CLK_SEL_32K = 1,
    CLK_SEL_12M,
    CLK_SEL_24M,
    CLK_SEL_LRC,
};



/*
 *-------------------R3_RTC_CON0
 */
#define RTC_RTC_KICK(a)		P33_CON_SET(R3_RTC_CON0, 4, 1, a)


/*
 *-------------------R3_OSL_CON
 */
#define OSL_X32XS(a)	P33_CON_SET(R3_OSL_CON, 4, 2, a)

#define OSL_X32TS(a)	P33_CON_SET(R3_OSL_CON, 2, 1, a)

#define OSL_X32OS(a)	P33_CON_SET(R3_OSL_CON, 1, 1, a)

#define OSL_X32ES(a)	P33_CON_SET(R3_OSL_CON, 0, 1, a)


/*
 *-------------------R3_TIME_CPND
 */
#define TIME_256HZ_CPND(a)	P33_CON_SET(R3_TIME_CPND, 0, 1, a)

#define TIME_64HZ_CPND(a)	P33_CON_SET(R3_TIME_CPND, 1, 1, a)

#define TIME_2HZ_CPND(a)	P33_CON_SET(R3_TIME_CPND, 2, 1, a)

#define TIME_1HZ_CPND(a)	P33_CON_SET(R3_TIME_CPND, 3, 1, a)


/*
 *-------------------R3_TIME_CPND
 */
#define TIME_256HZ_CON(a)	P33_CON_SET(R3_TIME_CON, 0, 1, a)

#define TIME_64HZ_CON(a)	P33_CON_SET(R3_TIME_CON, 1, 1, a)

#define TIME_2HZ_CON(a)		P33_CON_SET(R3_TIME_CON, 2, 1, a)

#define TIME_1HZ_CON(a)		P33_CON_SET(R3_TIME_CON, 3, 1, a)

#endif



