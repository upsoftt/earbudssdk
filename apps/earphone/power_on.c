#include "system/includes.h"
#include "media/includes.h"
#include "app_config.h"
#include "app_action.h"
#include "tone_player.h"
#include "asm/charge.h"
#include "app_charge.h"
#include "app_main.h"
#include "ui_manage.h"
#include "vm.h"
#include "app_chargestore.h"
#include "user_cfg.h"
#include "default_event_handler.h"

#if TCFG_ANC_BOX_ENABLE
#include "app_ancbox.h"
#endif

#define LOG_TAG_CONST       APP_POWER_ON
#define LOG_TAG             "[APP_POWER_ON]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

#ifdef CONFIG_MEDIA_DEVELOP_ENABLE
#define AUDIO_PLAY_EVENT_END 		AUDIO_DEC_EVENT_END
#endif /* #ifdef CONFIG_MEDIA_DEVELOP_ENABLE */


#define USER_POWER_ON_TONE_EN  1
#if USER_POWER_ON_TONE_EN



static int app_power_on_tone_event_handler(struct device_event *dev)
{
    int ret = false;

    switch (dev->event) {
    case AUDIO_PLAY_EVENT_END:
        r_printf("------AUDIO_PLAY_EVENT_END----------\n");
        task_switch_to_bt();
        break;
    }

    return ret;
}

static int power_on_event_handler(struct application *app, struct sys_event *event)
{
    switch (event->type) {
    case SYS_KEY_EVENT:
        return 0;
    case SYS_BT_EVENT:
        return 0;
    case SYS_DEVICE_EVENT:
        if ((u32)event->arg == DEVICE_EVENT_FROM_CHARGE) {
#if TCFG_CHARGE_ENABLE
            return app_charge_event_handler(&event->u.dev);
#endif
        }
        if ((u32)event->arg == DEVICE_EVENT_FROM_TONE) {
            return app_power_on_tone_event_handler(&event->u.dev);
        }
#if TCFG_CHARGESTORE_ENABLE || TCFG_TEST_BOX_ENABLE
        if ((u32)event->arg == DEVICE_EVENT_CHARGE_STORE) {
            app_chargestore_event_handler(&event->u.chargestore);
        }
#endif
#if TCFG_ANC_BOX_ENABLE
        if ((u32)event->arg == DEVICE_EVENT_FROM_ANC) {
            return app_ancbox_event_handler(&event->u.ancbox);
        }
#endif
        break;
    default:
        return false;
    }

#if 1///CONFIG_BT_BACKGROUND_ENABLE
    default_event_handler(event);
#endif

    return false;
}

static int power_on_state_machine(struct application *app, enum app_state state,
                              struct intent *it)
{
    int ret;
    switch (state) {
    case APP_STA_CREATE:
        break;
    case APP_STA_START:
        if (!it) {
            break;
        }
        switch (it->action) {
        case ACTION_POWER_ON_MAIN:
            printf("ACTION_POWER_ON_MAIN\n");
            ui_update_status(STATUS_BT_INIT_OK);
			
            STATUS *p_tone = get_tone_config();
            ret = tone_play_index(p_tone->power_on, 1);
            printf("power_off tone play ret:%d", ret);

            break;
        case ACTION_POWER_ON:
            r_printf("-----ACTION_POWER_ON------\n");
            break;
        }
        break;
    case APP_STA_PAUSE:
        break;
    case APP_STA_RESUME:
        break;
    case APP_STA_STOP:
        break;
    case APP_STA_DESTROY:
        break;
    }

    return 0;
}


static const struct application_operation app_power_on_ops = {
    .state_machine  = power_on_state_machine,
    .event_handler 	= power_on_event_handler,
};

REGISTER_APPLICATION(app_app_power_on) = {
    .name 	= "poweron",
    .action	= ACTION_POWER_ON_MAIN,
    .ops 	= &app_power_on_ops,
    .state  = APP_STA_DESTROY,
};




#endif // USER_POWER_ON_TONE_EN
