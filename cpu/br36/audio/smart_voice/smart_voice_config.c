/*****************************************************************
>file name : smart_voice_config.c
>author : lichao
>create time : Mon 01 Nov 2021 11:18:03 AM CST
*****************************************************************/
#include "typedef.h"
#include "app_config.h"

#if (TCFG_SMART_VOICE_ENABLE)
const u8 config_audio_kws_enable = 1; /*KWS 使能*/
#else
const u8 config_audio_kws_enable = 0;
#endif

const int config_audio_nn_vad_enable = 0;
