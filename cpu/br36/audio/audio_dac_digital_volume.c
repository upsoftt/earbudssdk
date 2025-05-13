/*
 ******************************************************************
 * 			AUDIO DAC DIGITAL VOLUME MODULE ONCHIP
 * 芯片内部数字音量模块，默认淡入淡出
 * 范围：0~16384
 * 实现原理和audio_digital_vol类似，实现方式有硬件和软件的区别
 ******************************************************************
 */

#include "media/includes.h"
extern struct audio_dac_hdl dac_hdl;

//#define DAC_DIGITAL_VOL_LOG_ENABLE
#ifdef DAC_DIGITAL_VOL_LOG_ENABLE
#define DV_LOG	y_printf
#else
#define DV_LOG(...)
#endif/*DAC_DIGITAL_VOL_LOG_ENABLE*/

enum {
    DAC_DV_STA_CLOSE = 0,
    DAC_DV_STA_OPEN,
};

/*
 *dac数字音量级数 DIGITAL_VOL_MAX
 *数组长度 DIGITAL_VOL_MAX + 1
 */
#define DIGITAL_VOL_MAX		31
static const u16 dac_dig_vol_tab[DIGITAL_VOL_MAX + 1] = {
	0 , //0
	  291, // 1:-50.00 db
	  320, // 2:-45.00 db
	  380, // 3:-40.00 db
	  440, // 4:-35.00 db
	  518, // 5:-30.00 db
	  732, // 6:-27.00 db
	  1034, // 7:-24.00 db
	  1301, // 8:-22.00 db
	  1460, // 9:-21.00 db
	  1638, // 10:-20.00 db    
	  1838, // 11:-19.00 db    
	  2063, // 12:-18.00 db    
	  2314, // 13:-17.00 db    
	  2597, // 14:-16.00 db    
	  2914, // 15:-15.00 db    
	  3269, // 16:-14.00 db    
	  3668, // 17:-13.00 db    
	  4115, // 18:-12.00 db    
	  4618, // 19:-11.00 db   
	  5181, // 20:-10.00 db   
	  5813, // 21:-9.00 db	  
	  6523, // 22:-8.00 db	  
	  7318, // 23:-7.00 db	  
	  8211, // 24:-6.00 db	  
	  9213, // 25:-5.00 db	  
	  10338, // 26:-4.00 db   
	  11599, // 27:-3.00 db   
	  13014, // 28:-2.00 db   
	  14602, // 29:-1.00 db
	  16000,//30
	  16384 //31
};


typedef struct {
    u8 max_level;
    u8 state;
    u8 fade;
    u16 *vol_tab;
} dac_digital_volume_t;
dac_digital_volume_t dac_dv;

void dac_digital_vol_open()
{
    dac_dv.vol_tab = dac_dig_vol_tab;
    dac_dv.max_level = ARRAY_SIZE(dac_dig_vol_tab) - 1;
    DV_LOG("dac_digital_vol_open,max_level:%d", dac_dv.max_level);
    dac_dv.state = DAC_DV_STA_OPEN;
}

void dac_digital_vol_tab_register(u16 *tab, u8 max_level)
{
    if (max_level <= 1) {
        DV_LOG("dac_digital_vol_tab_reg err,max_level:%d", max_level);
        return ;
    }
    dac_dv.vol_tab = tab;
    dac_dv.max_level = max_level - 1;
    DV_LOG("dac_digital_vol_tab_reg,max_level:%d", dac_dv.max_level);
}

void dac_digital_vol_set(u16 vol_l, u16 vol_r, u8 fade)
{
    if (dac_dv.state == DAC_DV_STA_OPEN) {
        DV_LOG("dac_digital_vol_set:%d,%d,fade:%d", vol_l, vol_r, fade);
        u16 dvol_l = (vol_l > dac_dv.max_level) ? dac_dv.max_level : vol_l;
        u16 dvol_r = (vol_r > dac_dv.max_level) ? dac_dv.max_level : vol_r;
        if (dac_dv.vol_tab) {
            dvol_l = dac_dv.vol_tab[dvol_l];
            dvol_r = dac_dv.vol_tab[dvol_r];
            DV_LOG("Dvol:%d,%d", JL_AUDIO->DAC_VL0 & 0xFFFF, (JL_AUDIO->DAC_VL0) >> 16 & 0xFFFF);
            DV_LOG("Avol:%d,%d", JL_ADDA->DAA_CON1 & 0xF, (JL_ADDA->DAA_CON1 >> 4) & 0xF);
            DV_LOG("Dvol_new:%d,%d", dvol_l, dvol_r);
            audio_dac_set_L_digital_vol(&dac_hdl, dvol_l);
            audio_dac_set_R_digital_vol(&dac_hdl, dvol_r);
        }
    }
}

void dac_digital_vol_close()
{
    DV_LOG("dac_digital_vol_close\n");
    dac_dv.state = DAC_DV_STA_CLOSE;
}

