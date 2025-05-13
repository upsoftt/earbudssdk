#include "typedef.h"
#include "asm/clock.h"
#include "asm/adc_api.h"
#include "timer.h"
#include "init.h"
#include "asm/efuse.h"
#include "irq.h"
#include "asm/power/p33.h"
#include "asm/power/p11.h"
#include "asm/power_interface.h"
#include "jiffies.h"

u32 adc_sample(u32 ch);
static volatile u16 _adc_res;
static volatile u16 cur_ch_value;
static u8 cur_ch = 0;
struct adc_info_t {
    u32 ch;
    u16 value;
    u32 jiffies;
    u32 sample_period;
};

#define     ENABLE_OCCUPY_MODE 1

static struct adc_info_t adc_queue[ADC_MAX_CH + ENABLE_OCCUPY_MODE];


static u8 mvbg_trim_lev;

static u8 adc_sample_flag = 0;

static u16 dtemp_voltage = 0;


#define     ADC_SRC_CLK clk_get("adc")

/*config adc clk according to sys_clk*/
static const u32 sys2adc_clk_info[] = {
    128000000L,
    96000000L,
    72000000L,
    48000000L,
    24000000L,
    12000000L,
    6000000L,
    1000000L,
};

u32 adc_add_sample_ch(u32 ch)
{
    u32 i = 0;
    for (i = 0; i < ADC_MAX_CH; i++) {
        /* printf("%s() %d %x %x\n", __func__, i, ch, adc_queue[i].ch); */
        if (adc_queue[i].ch == ch) {
            break;
        } else if (adc_queue[i].ch == -1) {
            adc_queue[i].ch = ch;
            adc_queue[i].value = 1;
            adc_queue[i].jiffies = 0;
            adc_queue[i].sample_period = msecs_to_jiffies(0);
            printf("add sample ch %x\n", ch);
            break;
        }
    }
    return i;
}

u32 adc_set_sample_freq(u32 ch, u32 ms)
{
    u32 i;
    for (i = 0; i < ADC_MAX_CH; i++) {
        if (adc_queue[i].ch == ch) {
            adc_queue[i].sample_period = msecs_to_jiffies(ms);
            adc_queue[i].jiffies = msecs_to_jiffies(ms) + jiffies;
            break;
        }
    }
    return i;
}

u32 adc_remove_sample_ch(u32 ch)
{
    u32 i = 0;
    for (i = 0; i < ADC_MAX_CH; i++) {
        if (adc_queue[i].ch == ch) {
            adc_queue[i].ch = -1;
            break;
        }
    }
    return i;
}
static u32 adc_get_next_ch(u32 cur_ch)
{
    for (int i = cur_ch + 1; i < ADC_MAX_CH; i++) {
        if (adc_queue[i].ch != -1) {
            return i;
        }
    }
    return 0;
}



#define PMU_CH_SAMPLE_FREQ    500 //ms

#define PMU_CH_VALUE_ARRAY_SIZE   (16 + 1)

static u16 vbat_value_array[PMU_CH_VALUE_ARRAY_SIZE];
static u16 vbg_value_array[PMU_CH_VALUE_ARRAY_SIZE];

static void adc_value_push(u16 *array, u16 adc_value)
{
    u16 pos = array[0];
    pos++;
    if (pos >= PMU_CH_VALUE_ARRAY_SIZE) {
        pos = 1;
    }
    array[pos] = adc_value;
    array[0] = pos;
}

static u16 adc_value_avg(u16 *array)
{
    u32 i, j, sum = 0;
    for (i = 1, j = 0; i < PMU_CH_VALUE_ARRAY_SIZE; i++) {
        if (array[i]) {
            sum += array[i];
            j += 1;
        }
    }
    if (sum) {
        return (sum / j);
    }
    return 1;
}

static void adc_value_array_reset(u16 *array)
{
    for (int i = 0; i < PMU_CH_VALUE_ARRAY_SIZE; i++) {
        array[i] = 0;
    }
}

u32 adc_get_value(u32 ch)
{
    if (ch == AD_CH_VBAT) {
        return adc_value_avg(vbat_value_array);
    } else if (ch == AD_CH_LDOREF) {
        return adc_value_avg(vbg_value_array);
    }

    for (int i = 0; i < ADC_MAX_CH; i++) {
        if (adc_queue[i].ch == ch) {
            return adc_queue[i].value;
        }
    }
    return 1;
}
#define     VBG_CENTER  801
#define     VBG_RES     3
u32 adc_value_to_voltage(u32 adc_vbg, u32 adc_ch_val)
{
    u32 adc_res = adc_ch_val;
    u32 adc_trim = get_vbg_trim();
    u32 tmp, tmp1;
    int now_mvbg;

    tmp1 = adc_trim & 0x0f;
    tmp = (adc_trim & BIT(4)) ? VBG_CENTER - tmp1 * VBG_RES : VBG_CENTER + tmp1 * VBG_RES;
    now_mvbg = MVBG_GET();
    if (now_mvbg != (int)mvbg_trim_lev) {
        tmp = (u32)((int)tmp + (now_mvbg - (int)mvbg_trim_lev) * 15);//根据trim的档位和当前档位纠正VBG电压值
    }
    if (adc_vbg == 0) {
        adc_vbg = 1;
    }
    adc_res = adc_res * tmp / adc_vbg;
    /* printf("adc_res %d mv vbg:%d adc:%d adc_trim:%x\n", adc_res, adc_vbg, adc_ch_val, adc_trim); */
    return adc_res;
}

u32 adc_get_voltage(u32 ch)
{
#ifdef CONFIG_FPGA_ENABLE
    return 1000;
#endif

    if (ch == AD_CH_DTEMP) {
        return dtemp_voltage;
    }

    u32 adc_vbg = adc_get_value(AD_CH_LDOREF);
    u32 adc_res = adc_get_value(ch);
    return adc_value_to_voltage(adc_vbg, adc_res);
}
u32 adc_check_vbat_lowpower()
{
    return 0;
}
void adc_audio_ch_select(u32 ch)
{
    /* SFR(JL_ANA->DAA_CON0, 12, 4, ch); */
}

void adc_close()
{
    JL_ADC->CON = 0;
    JL_ADC->CON = 0;
}
void adc_suspend()
{
    JL_ADC->CON &= ~BIT(4);
}
void adc_resume()
{
    JL_ADC->CON |= BIT(4);
}

void adc_enter_occupy_mode(u32 ch)
{
    if (JL_ADC->CON & BIT(4)) {
        return;
    }
    adc_queue[ADC_MAX_CH].ch = ch;
    cur_ch_value = adc_sample(ch);
}
void adc_exit_occupy_mode()
{
    adc_queue[ADC_MAX_CH].ch = -1;
}
u32 adc_occupy_run()
{
    if (adc_queue[ADC_MAX_CH].ch != -1) {
        while (1) {
            asm volatile("idle");//wait isr
            if (_adc_res != (u16) - 1) {
                break;
            }
        }
        if (_adc_res == 0) {
            _adc_res ++;
        }
        adc_queue[ADC_MAX_CH].value = _adc_res;
        _adc_res = cur_ch_value;
        return adc_queue[ADC_MAX_CH].value;
    }
    return 0;
}
u32 adc_get_occupy_value()
{
    if (adc_queue[ADC_MAX_CH].ch != -1) {
        return adc_queue[ADC_MAX_CH].value;
    }
    return 0;
}
u32 get_adc_div(u32 src_clk)
{
    u32 adc_clk;
    u32 adc_clk_idx;
    u32 cnt;
    adc_clk = src_clk;
    cnt = ARRAY_SIZE(sys2adc_clk_info);
    for (adc_clk_idx = 0; adc_clk_idx < cnt; adc_clk_idx ++) {
        if (adc_clk > sys2adc_clk_info[adc_clk_idx]) {
            break;
        }
    }

    if (adc_clk_idx < cnt) {
        adc_clk_idx = cnt - adc_clk_idx;
    } else {
        adc_clk_idx = cnt - 1;
    }
    return adc_clk_idx;
}

___interrupt
static void adc_isr()
{
    _adc_res = JL_ADC->RES;

    local_irq_disable();
    JL_ADC->CON = BIT(6);
    local_irq_enable();

#if 1
    if (adc_sample_flag > 1) {
        extern void adc_scan(void *priv);
        adc_scan((void *)&adc_sample_flag);
    }
#endif
}

u32 adc_sample(u32 ch)
{
    const u32 tmp_adc_res = _adc_res;
    _adc_res = (u16) - 1;

    u32 adc_con = 0;
    SFR(adc_con, 0, 3, 0b110);//div 96

    adc_con |= (0xf << 12); //启动延时控制，实际启动延时为此数值*8个ADC时钟
    adc_con |= BIT(3); //ana en
    adc_con |= BIT(6); //en
    adc_con |= BIT(5);//ie
    adc_con |= BIT(17);//clk en
    /* adc_con |= BIT(18)|BIT(19);//clk en */

    SFR(adc_con, 8, 4, ch & 0xf);

    if ((ch & 0xffff) == AD_CH_PMU) {
        if (adc_sample_flag > 1) {
            SFR(adc_con, 0, 3, 0b011);//div 24
            adc_con &= ~(0xf << 12);
        }
        adc_pmu_ch_select(ch >> 16);
    }

    JL_ADC->CON = adc_con;
    JL_ADC->CON |= BIT(4);//en

    /* printf("ch %x %x %x %x",ch,ch >> 16,JL_ADC->CON,P3_ANA_CON4); */

    JL_ADC->CON |= BIT(6);//kistart

    return tmp_adc_res;
}

void adc_scan(void *priv)
{
    static u16 vbg_adc_res = 0;
    static u16 dtemp_adc_res = 0;

    if ((priv == NULL) && (adc_sample_flag > 1)) {
        return;
    }

    if (adc_queue[ADC_MAX_CH].ch != -1) {   //occupy mode
        return;
    }

    if (JL_ADC->CON & BIT(4)) {
        adc_sample_flag = 0;
        return ;
    }

    if (_adc_res == 0xffff) {
        adc_sample_flag = 0;
    }

    if (adc_sample_flag) {
        if (adc_sample_flag == 2) {
            dtemp_adc_res = _adc_res;
            adc_sample_flag = 3;
            adc_sample(AD_CH_LDOREF);
            return;
        } else if (adc_sample_flag == 3) {
            vbg_adc_res = _adc_res;
            dtemp_voltage = adc_value_to_voltage(vbg_adc_res, dtemp_adc_res);
            /* printf("vbg:%d dtemp:%d vol:%dmv\n", vbg_adc_res, dtemp_adc_res, dtemp_voltage); */
        } else {
            adc_queue[cur_ch].value = _adc_res;
            if (adc_queue[cur_ch].ch == AD_CH_VBAT) {
                adc_value_push(vbat_value_array, _adc_res);
                /* printf("vbat %d",_adc_res); */
            } else if (adc_queue[cur_ch].ch == AD_CH_LDOREF) {
                adc_value_push(vbg_value_array, _adc_res);
                /* printf("vbg %d",_adc_res); */
            }
        }
        adc_sample_flag = 0;
    }

    u8 next_ch = adc_get_next_ch(cur_ch);

    if (adc_queue[next_ch].sample_period) {
        if (time_before(adc_queue[next_ch].jiffies, jiffies)) {
            if (adc_queue[next_ch].ch == AD_CH_DTEMP) {
                adc_sample_flag = 2;
            } else {
                adc_sample_flag = 1;
            }
            adc_sample(adc_queue[next_ch].ch);
            adc_queue[next_ch].jiffies += adc_queue[next_ch].sample_period;
        }
    } else {
        adc_sample(adc_queue[next_ch].ch);
        adc_sample_flag = 1;
    }

    cur_ch = next_ch;
}

static u16 adc_wait_pnd()
{
    while (!(JL_ADC->CON & BIT(7)));
    u32 adc_res = JL_ADC->RES;
    asm("nop");
    JL_ADC->CON |= BIT(6);
    return adc_res;
}

void adc_reset(void)
{
    int i, j;
    local_irq_disable();
    JL_ADC->CON = 0;

    memset(vbg_value_array, 0x0, sizeof(vbg_value_array));
    memset(vbat_value_array, 0x0, sizeof(vbat_value_array));

    u32 sum_ad = 0;
    adc_sample(AD_CH_LDOREF);
    for (int i = 0; i < PMU_CH_VALUE_ARRAY_SIZE; i ++) {
        sum_ad += adc_wait_pnd();
    }
    sum_ad /= PMU_CH_VALUE_ARRAY_SIZE;
    adc_value_push(vbg_value_array, sum_ad);
    printf("vbg_adc_value = %d\n", adc_value_avg(vbg_value_array));

    sum_ad = 0;
    adc_sample(AD_CH_VBAT);
    for (int i = 0; i < PMU_CH_VALUE_ARRAY_SIZE; i ++) {
        sum_ad += adc_wait_pnd();
    }
    sum_ad /= PMU_CH_VALUE_ARRAY_SIZE;
    adc_value_push(vbat_value_array, sum_ad);
    printf("vbat_adc_value = %d\n", adc_value_avg(vbat_value_array));
    printf("vbat = %d mv\n", adc_get_voltage(AD_CH_VBAT) * 4);

    JL_ADC->CON = BIT(6);
    cur_ch = 0;
    adc_sample_flag = 0;
    local_irq_enable();
}

void _adc_init(u32 sys_lvd_en)
{
    memset(adc_queue, 0xff, sizeof(adc_queue));
    memset(vbg_value_array, 0x0, sizeof(vbg_value_array));
    memset(vbat_value_array, 0x0, sizeof(vbat_value_array));

    JL_ADC->CON = 0;
    JL_ADC->CON = 0;

    adc_add_sample_ch(AD_CH_LDOREF);
    adc_set_sample_freq(AD_CH_LDOREF, PMU_CH_SAMPLE_FREQ);

    adc_add_sample_ch(AD_CH_VBAT);
    adc_set_sample_freq(AD_CH_VBAT, PMU_CH_SAMPLE_FREQ);

    adc_add_sample_ch(AD_CH_DTEMP);
    adc_set_sample_freq(AD_CH_DTEMP, PMU_CH_SAMPLE_FREQ);

    u32 sum_ad = 0;
    adc_sample(AD_CH_LDOREF);
    for (int i = 0; i < PMU_CH_VALUE_ARRAY_SIZE; i ++) {
        sum_ad += adc_wait_pnd();
    }
    sum_ad /= PMU_CH_VALUE_ARRAY_SIZE;
    adc_value_push(vbg_value_array, sum_ad);
    printf("vbg_adc_value = %d\n", adc_value_avg(vbg_value_array));

    sum_ad = 0;
    adc_sample(AD_CH_VBAT);
    for (int i = 0; i < PMU_CH_VALUE_ARRAY_SIZE; i ++) {
        sum_ad += adc_wait_pnd();
    }
    sum_ad /= PMU_CH_VALUE_ARRAY_SIZE;
    adc_value_push(vbat_value_array, sum_ad);
    printf("vbat_adc_value = %d\n", adc_value_avg(vbat_value_array));
    printf("vbat = %d mv\n", adc_get_voltage(AD_CH_VBAT) * 4);

    sum_ad = 0;
    adc_sample(AD_CH_DTEMP);
    for (int i = 0; i < PMU_CH_VALUE_ARRAY_SIZE; i ++) {
        sum_ad += adc_wait_pnd();
    }
    sum_ad /= PMU_CH_VALUE_ARRAY_SIZE;
    dtemp_voltage = adc_value_to_voltage(adc_value_avg(vbg_value_array), sum_ad);
    printf("dtemp_adc_value = %d\n", sum_ad);
    printf("dtemp = %d mv\n", dtemp_voltage);

    request_irq(IRQ_SARADC_IDX, 0, adc_isr, 0);

    usr_timer_add(NULL, adc_scan, 5, 0); //2ms

    /* void btosc_fast_boot_test(void); */
    /* btosc_fast_boot_test(); */
    /* sys_timer_add(NULL, adc_scan, 10); //2ms */

    /* void adc_test(); */
    /* usr_timer_add(NULL, adc_test, 1000, 0); //2ms */

    /* extern void wdt_close(); */
    /* wdt_close(); */
    /*  */
    /* while(1); */
}
static u32 get_vdd_voltage(u32 ch)
{
    u32 vbg_value = 0;
    u32 wvdd_value = 0;

    adc_sample(AD_CH_LDOREF);
    for (int i = 0; i < 10; i++) {
        vbg_value += adc_wait_pnd();
    }

    adc_sample(ch);
    for (int i = 0; i < 10; i++) {
        wvdd_value += adc_wait_pnd();
    }

    u32 adc_vbg = vbg_value / 10;
    u32 adc_res = wvdd_value / 10;

    return adc_value_to_voltage(adc_vbg, adc_res);

}

static u8 wvdd_trim(u8 trim)
{
    u8 wvdd_lev = 0;
    u8 err = 0;
    wvdd_lev = 0;
    if (trim) {
#if 1
        P33_CON_SET(P3_ANA_CON13, 0, 4, wvdd_lev);
        WVDD_LOAD_EN(1);
        WLDO06_EN(1);
        delay(2000);//1ms
        do {
            P33_CON_SET(P3_ANA_CON13, 0, 4, wvdd_lev);
            delay(2000);//1ms * n
            if (get_vdd_voltage(AD_CH_WVDD) > WVDD_VOL_TRIM) {
                break;
            }
            wvdd_lev ++;
        } while (wvdd_lev < WVDD_LEVEL_MAX);
        WVDD_LOAD_EN(0);
        WLDO06_EN(0);
#else
        wvdd_lev = 7;
#endif

        //update_wvdd_trim_level(wvdd_lev);
    } else {
        wvdd_lev = get_wvdd_trim_level();
    }

#if 0
    printf("wvdd min: %d, max: %d, def_lev: %d\n", (WVDD_VOL_TRIM - WVDD_VOL_MIN) / WVDD_VOL_STEP - 2, \
           (WVDD_VOL_TRIM - WVDD_VOL_MIN) / WVDD_VOL_STEP + 2, \
           WVDD_LEVEL_DEFAULT);
#endif

    printf("trim: %d, wvdd_lev: %d\n", trim, wvdd_lev);

    if (trim) {
        u8 min = (WVDD_VOL_TRIM - WVDD_VOL_MIN) / WVDD_VOL_STEP - 2;
        u8 max = (WVDD_VOL_TRIM - WVDD_VOL_MIN) / WVDD_VOL_STEP + 2;
        if (!(wvdd_lev >= min && wvdd_lev <= max)) {
            wvdd_lev = WVDD_LEVEL_DEFAULT;
            err = 1;
        }
    }

    /* power_set_wvdd(wvdd_lev); */
    M2P_WDVDD = wvdd_lev;

    if (err) {
        return WVDD_LEVEL_ERR;
    }

    return wvdd_lev;
}
static u8 pvdd_trim(u8 trim)
{
    u32 v = 0;
    u32 lev = 0xf;
    u8 err = 0;
    if (trim) {
#if 1
        for (; lev; lev--) {
            P33_CON_SET(P3_PVDD1_AUTO, 0, 8, (lev | lev << 4));
            delay(2000);//1ms
            v = get_vdd_voltage(AD_CH_PVDD);
            if (v < (PVDD_VOL_TRIM + PVDD_VOL_STEP)) {
                break;
            }
        }

        if (v < PVDD_VOL_TRIM) {
            if (lev < PVDD_LEVEL_MAX) {
                lev++;
            }
        }
#else
        lev = 10;
#endif

        //update_pvdd_trim_level(lev);
    } else {
        lev = get_pvdd_trim_level();
    }

#if 0
    printf("pvdd min: %d, max: %d, def_lev: %d\n", (PVDD_VOL_TRIM - PVDD_VOL_MIN) / PVDD_VOL_STEP - 2, \
           (PVDD_VOL_TRIM - PVDD_VOL_MIN) / PVDD_VOL_STEP + 2,  \
           PVDD_LEVEL_DEFAULT);
#endif

    g_printf("trim: %d, pvdd_lev: %d %d, pvdd_lev_l: %d\n", trim, lev, v, lev - PVDD_LEVEL_TRIM_LOW);

    if (trim) {
        u8 min = (PVDD_VOL_TRIM - PVDD_VOL_MIN) / PVDD_VOL_STEP - 2;
        u8 max = (PVDD_VOL_TRIM - PVDD_VOL_MIN) / PVDD_VOL_STEP + 2;
        if (!(lev >= min && lev <= max)) {
            lev = PVDD_LEVEL_DEFAULT;
            err = 1;
        }
    }

    P33_CON_SET(P3_PVDD1_AUTO, 0, 8, (lev | lev << 4));
    delay(2000);
    P33_CON_SET(P3_PVDD0_AUTO, 0, 8, (7 << 4) | (lev - PVDD_LEVEL_TRIM_LOW));

    if (err) {
        return PVDD_LEVEL_ERR;
    }

    return lev;
}

void adc_init()
{
    adc_pmu_detect_en(1);

    //记住最初的VBG值
    mvbg_trim_lev = MVBG_GET();

    //trim wvdd
    u8 trim = check_wvdd_pvdd_trim(0);
    u8 wvdd_lev = wvdd_trim(trim);
    u8 pvdd_lev = pvdd_trim(trim);

    if (trim) {
        if ((wvdd_lev != WVDD_LEVEL_ERR) && (pvdd_lev != PVDD_LEVEL_ERR)) {
            update_wvdd_pvdd_trim_level(wvdd_lev, pvdd_lev);
        }
    }

    _adc_init(1);
}
//late_initcall(adc_init);

void adc_test()
{

    /* printf("\n\n%s() chip_id :%x\n", __func__, get_chip_id()); */
    /* printf("%s() vbg trim:%x\n", __func__, get_vbg_trim()); */
    /* printf("%s() vbat trim:%x\n", __func__, get_vbat_trim());  */

    /* printf("\n\nWLA_CON0 %x\n", JL_ANA->WLA_CON0); */
    /* printf("WLA_CON9 %x\n", JL_ANA->WLA_CON9); */
    /* printf("WLA_CON10 %x\n", JL_ANA->WLA_CON10); */
    /* printf("WLA_CON21 %x\n", JL_ANA->WLA_CON21); */

    /* printf("ADA_CON %x\n", JL_ANA->ADA_CON3); */
    /* printf("PLL_CON1 %x\n", JL_CLOCK->PLL_CON1); */

    printf("\n%s() VBAT:%d %d mv\n\n", __func__,
           adc_get_value(AD_CH_VBAT), adc_get_voltage(AD_CH_VBAT) * 4);

    /* printf("\n%s() pa3:%d %d mv\n\n", __func__,                   */
    /*        adc_get_value(AD_CH_PA3), adc_get_voltage(AD_CH_PA3)); */


}
void adc_vbg_init()
{
    return ;
}
//__initcall(adc_vbg_init);
