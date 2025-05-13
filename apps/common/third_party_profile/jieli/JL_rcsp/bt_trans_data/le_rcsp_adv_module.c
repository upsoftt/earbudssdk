/*********************************************************************************************
    *   Filename        : le_server_module.c

    *   Description     :

    *   Author          :

    *   Email           : zh-jieli.com

    *   Last modifiled  : 2017-01-17 11:14

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

// *****************************************************************************
/* EXAMPLE_START(le_counter): LE Peripheral - Heartbeat Counter over GATT
 *
 * @text All newer operating systems provide GATT Client functionality.
 * The LE Counter examples demonstrates how to specify a minimal GATT Database
 * with a custom GATT Service and a custom Characteristic that sends periodic
 * notifications.
 */
// *****************************************************************************
#include "system/app_core.h"
#include "system/includes.h"

#include "app_config.h"
#include "app_action.h"

#include "btstack/btstack_task.h"
#include "btstack/bluetooth.h"
#include "user_cfg.h"
#include "vm.h"
#include "app_power_manage.h"
#include "btcontroller_modules.h"
#include "JL_rcsp_api.h"
#include "rcsp_adv_bluetooth.h"
#include "bt_common.h"
#include "3th_profile_api.h"
#include "btstack/avctp_user.h"
#include "app_chargestore.h"

#include "le_common.h"
#include "le_rcsp_adv_module.h"
#include "btcrypt.h"
#include "custom_cfg.h"
#include "adv_music_info_setting.h"
#include "classic/tws_api.h"
#include "rcsp_adv_user_update.h"
#include "bt_tws.h"
#include "adv_hearing_aid_setting.h"

#include "adv_key_setting.h"
#include "tone_player.h"

#if (TCFG_BLE_DEMO_SELECT == DEF_BLE_DEMO_ADV_RCSP)

#if TCFG_CHARGE_BOX_ENABLE
#include "chgbox_box.h"
#endif

#define TEST_SEND_DATA_RATE         0  //测试
#define PRINT_DMA_DATA_EN           0


#if 1
extern void printf_buf(u8 *buf, u32 len);
#define log_info          printf
#define log_info_hexdump  printf_buf
#else
#define log_info(...)
#define log_info_hexdump(...)
#endif

#define EIR_TAG_STRING   0xd6, 0x05, 0x08, 0x00, 'J', 'L', 'A', 'I', 'S', 'D','K'
static const char user_tag_string[] = {EIR_TAG_STRING};

//用户可配对的，这是样机跟客户开发的app配对的秘钥
/* const u8 link_key_data[16] = {0x06, 0x77, 0x5f, 0x87, 0x91, 0x8d, 0xd4, 0x23, 0x00, 0x5d, 0xf1, 0xd8, 0xcf, 0x0c, 0x14, 0x2b}; */


/* #define LOG_TAG_CONST       BT_BLE */
/* #define LOG_TAG             "[LE_S_DEMO]" */
/* #define LOG_ERROR_ENABLE */
/* #define LOG_DEBUG_ENABLE */
/* #define LOG_INFO_ENABLE */
/* #define LOG_DUMP_ENABLE */
/* #define LOG_CLI_ENABLE */
/* #include "debug.h" */

//------
#define ATT_LOCAL_PAYLOAD_SIZE    (200)                   //note: need >= 20
#define ATT_SEND_CBUF_SIZE        (512)                   //note: need >= 20,缓存大小，可修改
#define ATT_RAM_BUFSIZE           (ATT_CTRL_BLOCK_SIZE + ATT_LOCAL_PAYLOAD_SIZE + ATT_SEND_CBUF_SIZE)                   //note:
static u8 att_ram_buffer[ATT_RAM_BUFSIZE] __attribute__((aligned(4)));
//---------------

#if TEST_SEND_DATA_RATE
static u32 test_data_count;
static u32 server_timer_handle = 0;
#endif

//---------------
#define ADV_INTERVAL_MIN          (160)

static hci_con_handle_t con_handle;

//加密设置
static const uint8_t sm_min_key_size = 7;

//连接参数设置
static const uint8_t connection_update_enable = 1; ///0--disable, 1--enable
static uint8_t connection_update_cnt = 0; //
static const struct conn_update_param_t connection_param_table[] = {
    {16, 24, 16, 600},
    {12, 28, 14, 600},//11
    {8,  20, 20, 600},//3.7
    /* {12, 28, 4, 600},//3.7 */
    /* {12, 24, 30, 600},//3.05 */
};
#define CONN_PARAM_TABLE_CNT      (sizeof(connection_param_table)/sizeof(struct conn_update_param_t))

#if (ATT_RAM_BUFSIZE < 64)
#error "adv_data & rsp_data buffer error!!!!!!!!!!!!"
#endif

static u8 adv_data_len;
static u8 adv_data[ADV_RSP_PACKET_MAX];//max is 31
static u8 scan_rsp_data_len;
static u8 scan_rsp_data[ADV_RSP_PACKET_MAX];//max is 31


/* #define adv_data       &att_ram_buffer[0] */
/* #define scan_rsp_data  &att_ram_buffer[32] */

static u32 ble_timer_handle = 0;
static char *gap_device_name = "br22_ble_test";
static u8 gap_device_name_len = 0;
volatile static u8 ble_work_state = 0;
static u8 test_read_write_buf[4];
static u8 adv_ctrl_en;

extern const char *bt_get_local_name();
extern const u8 *bt_get_mac_addr();
extern const u8 *ble_get_mac_addr(void);
extern void swapX(const uint8_t *src, uint8_t *dst, int len);
extern int get_bt_tws_connect_status();
void rcsp_set_update_flag(u8 flag);
void rcsp_update_set_role_switch(u8 sw);
void sys_auto_shut_down_disable(void);

#define BLE_TIMER_SET (1500)

struct _bt_adv_handle {

    u8 seq_rand;
    u8 connect_flag;

    u8 bat_charge_L;
    u8 bat_charge_R;
    u8 bat_charge_C;
    u8 bat_percent_L;
    u8 bat_percent_R;
    u8 bat_percent_C;

    u8 modify_flag;
    u8 adv_disable_cnt;

    u8 ble_adv_notify;
} bt_adv_handle = {0};

#define __this (&bt_adv_handle)

static void (*app_recieve_callback)(void *priv, void *buf, u16 len) = NULL;
static void (*app_ble_state_callback)(void *priv, ble_state_e state) = NULL;
static void (*ble_resume_send_wakeup)(void) = NULL;
static u32 channel_priv;
u16 sibling_ver_info = 0;
//------------------------------------------------------

static int app_send_user_data_check(u16 len);
static int app_send_user_data_do(void *priv, u8 *data, u16 len);
static int app_send_user_data(u16 handle, u8 *data, u16 len, u8 handle_type);
//------------------------------------------------------
//广播参数设置
static void advertisements_setup_init();
extern const char *bt_get_local_name();
static int set_adv_enable(void *priv, u32 en);
static int get_buffer_vaild_len(void *priv);
//------------------------------------------------------
static void send_request_connect_parameter(u8 table_index)
{
    struct conn_update_param_t *param = (void *)&connection_param_table[table_index];//static ram

    log_info("update_request:-%d-%d-%d-%d-\n", param->interval_min, param->interval_max, param->latency, param->timeout);
    if (con_handle) {
        ble_user_cmd_prepare(BLE_CMD_REQ_CONN_PARAM_UPDATE, 2, con_handle, param);
    }
}

static void check_connetion_updata_deal(void)
{
    if (connection_update_enable) {
        if (connection_update_cnt < CONN_PARAM_TABLE_CNT) {
            send_request_connect_parameter(connection_update_cnt);
        }
    }
}

static void connection_update_complete_success(u8 *packet)
{
    int con_handle, conn_interval, conn_latency, conn_timeout;

    con_handle = hci_subevent_le_connection_update_complete_get_connection_handle(packet);
    conn_interval = hci_subevent_le_connection_update_complete_get_conn_interval(packet);
    conn_latency = hci_subevent_le_connection_update_complete_get_conn_latency(packet);
    conn_timeout = hci_subevent_le_connection_update_complete_get_supervision_timeout(packet);

    log_info("conn_interval = %d\n", conn_interval);
    log_info("conn_latency = %d\n", conn_latency);
    log_info("conn_timeout = %d\n", conn_timeout);
}


static void set_ble_work_state(ble_state_e state)
{
    if (state != ble_work_state) {
        log_info("ble_work_st:%x->%x\n", ble_work_state, state);
        ble_work_state = state;
        if (app_ble_state_callback) {
            app_ble_state_callback((void *)channel_priv, state);
        }
    }
}

static ble_state_e get_ble_work_state(void)
{
    return ble_work_state;
}

static void cbk_sm_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    sm_just_event_t *event = (void *)packet;
    u32 tmp32;
    switch (packet_type) {
    case HCI_EVENT_PACKET:
        switch (hci_event_packet_get_type(packet)) {
        case SM_EVENT_JUST_WORKS_REQUEST:
            sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
            log_info("Just Works Confirmed.\n");
            break;
        case SM_EVENT_PASSKEY_DISPLAY_NUMBER:
            log_info_hexdump(packet, size);
            memcpy(&tmp32, event->data, 4);
            log_info("Passkey display: %06u.\n", tmp32);
            break;
        }
        break;
    }
}


#if TEST_SEND_DATA_RATE
static void server_timer_handler(void)
{
    if (!con_handle) {
        test_data_count = 0;
        return;
    }

    if (test_data_count) {
        log_info("\n-data_rate: %d bps-\n", test_data_count * 8);
        test_data_count = 0;
    }
}

static void server_timer_start(void)
{
    server_timer_handle  = sys_timer_add(NULL, server_timer_handler, 1000);
}

static void server_timer_stop(void)
{
    if (server_timer_handle) {
        sys_timeout_del(server_timer_handle);
        server_timer_handle = 0;
    }
}

void test_data_send_packet(void)
{
    u32 vaild_len = get_buffer_vaild_len(0);
    if (vaild_len) {
        /* printf("\n---test_data_len = %d---\n",vaild_len); */
        app_send_user_data(ATT_CHARACTERISTIC_ae02_01_VALUE_HANDLE, &test_data_send_packet, vaild_len, ATT_OP_AUTO_READ_CCC);
        /* app_send_user_data(ATT_CHARACTERISTIC_ae05_01_VALUE_HANDLE, &test_data_send_packet, vaild_len,ATT_OP_AUTO_READ_CCC); */
        test_data_count += vaild_len;
    }
}
#endif


static void can_send_now_wakeup(void)
{
    putchar('E');
    if (ble_resume_send_wakeup) {
        ble_resume_send_wakeup();
    }

#if TEST_SEND_DATA_RATE
    test_data_send_packet();
#endif
}

_WEAK_
u8 ble_update_get_ready_jump_flag(void)
{
    return 0;
}

/*
 * @section Packet Handler
 *
 * @text The packet handler is used to:
 *        - stop the counter after a disconnect
 *        - send a notification when the requested ATT_EVENT_CAN_SEND_NOW is received
 */

#if (MUTIl_CHARGING_BOX_EN)
static int ble_disconnect(void *priv);
#endif

/* LISTING_START(packetHandler): Packet Handler */
static void cbk_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    int mtu;
    u32 tmp;

    switch (packet_type) {
    case HCI_EVENT_PACKET:
        switch (hci_event_packet_get_type(packet)) {

        /* case DAEMON_EVENT_HCI_PACKET_SENT: */
        /* break; */
        case ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE:
            log_info("ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE\n");
        case ATT_EVENT_CAN_SEND_NOW:
            can_send_now_wakeup();
            break;

        case HCI_EVENT_LE_META:
            switch (hci_event_le_meta_get_subevent_code(packet)) {
            case HCI_SUBEVENT_LE_CONNECTION_COMPLETE: {
                set_app_connect_type(TYPE_BLE);
                con_handle = little_endian_read_16(packet, 4);
                log_info("HCI_SUBEVENT_LE_CONNECTION_COMPLETE: %0x\n", con_handle);
                connection_update_complete_success(packet + 8);
                ble_user_cmd_prepare(BLE_CMD_ATT_SEND_INIT, 4, con_handle, att_ram_buffer, ATT_RAM_BUFSIZE, ATT_LOCAL_PAYLOAD_SIZE);
                set_ble_work_state(BLE_ST_CONNECT);
#if (0 == BT_CONNECTION_VERIFY)
                JL_rcsp_auth_reset();
#endif

                rcsp_dev_select(RCSP_BLE);
                __this->ble_adv_notify = 1;
#if (MUTIl_CHARGING_BOX_EN)
                ble_disconnect(NULL);
#endif
            }
            break;

            case HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE:
                connection_update_complete_success(packet);
                break;
            }
            break;

        case HCI_EVENT_DISCONNECTION_COMPLETE:
            log_info("HCI_EVENT_DISCONNECTION_COMPLETE: %0x\n", packet[5]);
            con_handle = 0;
#if RCSP_ADV_MUSIC_INFO_ENABLE
            stop_get_music_timer(1);
#endif
            ble_user_cmd_prepare(BLE_CMD_ATT_SEND_INIT, 4, con_handle, 0, 0, 0);
            set_ble_work_state(BLE_ST_DISCONN);
            set_app_connect_type(TYPE_NULL);
            if (!ble_update_get_ready_jump_flag()) {
#if TCFG_USER_TWS_ENABLE
                if (!(tws_api_get_tws_state() & TWS_STA_SIBLING_CONNECTED)) {
                    printf(")))))))) 1\n");
                    bt_ble_adv_enable(1);
                } else {
                    printf("))))))))>>>> %d\n", tws_api_get_role());
                    if (tws_api_get_role() == TWS_ROLE_MASTER) {
                        bt_ble_adv_enable(1);
                    }
                }
#else
                bt_ble_adv_enable(1);
#endif
            }
            connection_update_cnt = 0;
            /*
                        JL_protocol_reset();
            #if (RCSP_FILEBROWSE_EN)
                        extern void rcsp_file_browse_close(void);
                        rcsp_file_browse_close();
            #endif
            */
            break;

        case ATT_EVENT_MTU_EXCHANGE_COMPLETE:
            mtu = att_event_mtu_exchange_complete_get_MTU(packet) - 3;
            log_info("ATT MTU = %u\n", mtu);
            ble_user_cmd_prepare(BLE_CMD_ATT_MTU_SIZE, 1, mtu);
            break;

        case HCI_EVENT_VENDOR_REMOTE_TEST:
            log_info("--- HCI_EVENT_VENDOR_REMOTE_TEST\n");
            break;

        case L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_RESPONSE:
            tmp = little_endian_read_16(packet, 4);
            log_info("-update_rsp: %02x\n", tmp);
            if (tmp) {
                connection_update_cnt++;
                log_info("remoter reject!!!\n");
                check_connetion_updata_deal();
            } else {
                connection_update_cnt = CONN_PARAM_TABLE_CNT;
            }
            break;

        case HCI_EVENT_ENCRYPTION_CHANGE:
            log_info("HCI_EVENT_ENCRYPTION_CHANGE= %d\n", packet[2]);
            break;
        }
        break;
    }
}


/* LISTING_END */

/*
 * @section ATT Read
 *
 * @text The ATT Server handles all reads to constant data. For dynamic data like the custom characteristic, the registered
 * att_read_callback is called. To handle long characteristics and long reads, the att_read_callback is first called
 * with buffer == NULL, to request the total value length. Then it will be called again requesting a chunk of the value.
 * See Listing attRead.
 */

 
static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{

    uint16_t  att_value_len = 0;
    uint16_t handle = att_handle;

    /* printf("READ Callback, handle %04x, offset %u, buffer size %u\n", handle, offset, buffer_size); */

    log_info("read_callback, handle= 0x%04x,buffer= %08x\n", handle, (u32)buffer);

    switch (handle) {
    case ATT_CHARACTERISTIC_2a00_01_VALUE_HANDLE:
        att_value_len = strlen(gap_device_name);

        if ((offset >= att_value_len) || (offset + buffer_size) > att_value_len) {
            break;
        }

        if (buffer) {
            memcpy(buffer, &gap_device_name[offset], buffer_size);
            att_value_len = buffer_size;
            log_info("\n------read gap_name: %s \n", gap_device_name);
        }
        break;

    
    case ATT_CHARACTERISTIC_ae02_01_CLIENT_CONFIGURATION_HANDLE:
        if (buffer) {
            buffer[0] = att_get_ccc_config(handle);
            buffer[1] = 0;

        }
        att_value_len = 2;
        break;
    case ATT_CHARACTERISTIC_18AC9920_BCE7_11E5_AE01_0002A5D5C51B_01_CLIENT_CONFIGURATION_HANDLE:
        if (buffer) {
            buffer[0] = att_get_ccc_config(handle);
            buffer[1] = 0;

        }
        att_value_len = 2;
        break;

    default:
        break;
    }

    log_info("att_value_len= %d\n", att_value_len);
    return att_value_len;

}
/* LISTING_END */
/*
 * @section ATT Write
 *
 * @text The only valid ATT write in this example is to the Client Characteristic Configuration, which configures notification
 * and indication. If the ATT handle matches the client configuration handle, the new configuration value is stored and used
 * in the heartbeat handler to decide if a new value should be sent. See Listing attWrite.
 */

void sum_check(u8 *buffer, u8 buffer_size)
{
    u8 sum_i = 0;
	u8 sum_data = 0;
    buffer_size = 9;
    buffer[8] = 0;
	printf_buf(buffer, 9);	
	for(sum_i = 0; sum_i < buffer_size; sum_i++){
        sum_data = sum_data ^ buffer[sum_i];
	}
	log_info("sum_data= %x\n", sum_data);
	buffer[8] = sum_data;
	log_info("send buf: \n");
    put_buf(buffer,16);
}
extern int user_spp_data_send(u8 *buf, u16 len);
void user_send_data(u8 *data,u16 len)
{
    sum_check(data,len);
    if(0 == data[1]){
        log_info("sylon debug: ble send data checksum = %x len = %d ",data[8],len);
        log_info("sylon debug:send to app data /n");  
        put_buf(data,len);      
        app_send_user_data(ATT_CHARACTERISTIC_18AC9920_BCE7_11E5_AE01_0002A5D5C51B_01_VALUE_HANDLE, data, len, ATT_OP_AUTO_READ_CCC);
    }else{
        log_info("sylon debug: spp send data checksum = %x len = %d ",data[8],len);
        user_spp_data_send(data,len);
    }
}
/*30秒内向APP更新电量*/
extern u8 get_rcsp_connect_status(void);
void user_to_app_percent(void)
{
return;
    u8 buf[ ] ={0x01, 0x00, 0x00, 0x4E, 0x00, 0x0A, 0x00, 0x01, 0x5E, 0x01,   SDK_version, 0x64,   SDK_version, 0x32,  0x50};
	if (tws_api_get_role() == TWS_ROLE_SLAVE){
        return;
	}
	if((con_handle || get_rcsp_connect_status())){

	} else {
        return;
	}
    log_info("bat_percent_L:%d  bat_percent_R:%d   bat_percent_C:%d\n",__this->bat_percent_L,__this->bat_percent_R,__this->bat_percent_C);
    buf[13] = __this->bat_percent_L;
    buf[17] = __this->bat_percent_R;
    buf[18] = __this->bat_percent_C;
	log_info("user_prcent_to_app:\n");
	printf_buf(buf,sizeof(buf)/sizeof(buf[0]));
	user_send_data(buf,sizeof(buf)/sizeof(buf[0]));	//电量返回给ble
    buf[1] = 1;
	user_spp_data_send(buf,sizeof(buf)/sizeof(buf[0]));	//电量返回给spp
}

/*0x4d  握手包*/
u8 local_read_licens[12];

void app_connect_ack(u8 *buffer, u8 buffer_size)
{
    u8 buf[22] ={0x01, 0x00, 0x00, 0x4D, 0x00, 0x01, 0x00, 0x01, 0x4C,  0x01};
    buf[1] = buffer[1]; /*spp和ble的区分返回给客户*/
    buf[7] = buffer[7];
    buf[6] = buffer[6];
	memcpy(&buf[10],local_read_licens,12);    
    if((con_handle || get_rcsp_connect_status())){
        log_info("\n app_connect_ack :send buf[3] = 0x%x ----len:%d  \n",buf[3],sizeof(buf)/sizeof(buf[0]));
        user_send_data(buf,sizeof(buf)/sizeof(buf[0]));
	} else {
        return;
	}
}
/*0x4e  获取TWS耳机信息               */
void app_get_tws_status(u8 *buffer, u8 buffer_size)
{
    u8 buf[ ] ={0x01, 0x00, 0x00, 0x4E, 0x00, 0x0A, 0x00, 0x01, 0x5E, 0x01,  SDK_version, 0x64,   SDK_version, 0x32,  0x50};
    buf[1] = buffer[1]; /*spp和ble的区分返回给客户*/
    buf[7] = buffer[7];
    buf[6] = buffer[6];
    log_info("bat_percent_L:%d  bat_percent_R:%d   bat_percent_C:%d\n",__this->bat_percent_L,__this->bat_percent_R,__this->bat_percent_C);
    buf[13] = __this->bat_percent_L;//获取左耳的电量  
    buf[17] = __this->bat_percent_R;//获取右耳的电量   
    buf[18] = __this->bat_percent_C;//获取充电仓的电量
    put_buf(buf, sizeof(buf)/sizeof(buf[0]));
    user_send_data(buf,sizeof(buf)/sizeof(buf[0]));
    log_info("\n client :send buf[3] = 0x%x ----len:%d  \n",buf[3],sizeof(buf)/sizeof(buf[0]));
}
/*0x53  设置耳机自定义EQ参数          */
void app_set_earphone_eq(u8 *buffer, u8 buffer_size)
{
    u8 buf[ ] ={0x11,0x00,0x00,0x4C,0x56};
    buf[1] = buffer[1]; /*spp和ble的区分返回给客户*/
    buf[7] = buffer[7];
    buf[6] = buffer[6];
    put_buf(buf, 32);
    user_send_data(buf,sizeof(buf)/sizeof(buf[0]));
    log_info("\n client :send buf[3] = 0x%x ----len:%d  \n",buf[3],sizeof(buf)/sizeof(buf[0]));
}

/*0x62  设置入耳自动播放/自动暂停状态     */
void app_set_autoplay_state(u8 *buffer, u8 buffer_size)
{
    u8 buf[ ] ={0x01, 0x00, 0x00, 0x62, 0x00, 0x01, 0x00, 0x01, 0x63,  0x01};
    buf[1] = buffer[1]; /*spp和ble的区分返回给客户*/
    buf[7] = buffer[7];
    buf[6] = buffer[6];
    extern void user_to_key_event(u8 key_event);
    if(0 == buffer[9]){
        user_send_cmd_prepare(USER_CTRL_AVCTP_OPID_PAUSE, 0, NULL);
    }
    else if(1 == buffer[9]){
        if (a2dp_get_status() != BT_MUSIC_STATUS_STARTING) {
            user_send_cmd_prepare(USER_CTRL_AVCTP_OPID_PLAY, 0, NULL);
        } else {
            user_send_cmd_prepare(USER_CTRL_AVCTP_OPID_PAUSE, 0, NULL);
        }
    }
    else if(2 == buffer[9]){

    }
    put_buf(buf, 32);
    user_send_data(buf,sizeof(buf)/sizeof(buf[0]));
    log_info("\n client :send buf[3] = 0x%x ----len:%d  \n",buf[3],sizeof(buf)/sizeof(buf[0]));
}
/*0x63  获取自动播放/自动暂停状态     */
void app_get_autoplay_state(u8 *buffer, u8 buffer_size)
{
    u8 buf[ ] ={0x11,0x00,0x00,0x4C,0x56};
    buf[1] = buffer[1]; /*spp和ble的区分返回给客户*/
    buf[7] = buffer[7];
    buf[6] = buffer[6];
    put_buf(buf, 32);
    user_send_data(buf,sizeof(buf)/sizeof(buf[0]));
    log_info("\n client :send buf[3] = 0x%x ----len:%d  \n",buf[3],sizeof(buf)/sizeof(buf[0]));
}
/*0x69  设置耳机预设EQ     0: Default, 1: Bass reducer, 2: Bass booster, 3: Acoustic, 4: Classical, 5: Podcast, 6: Deep, 7: Electronic, 8: Hip-Hop, 9: Lounge, 10: Pop, 11: Vocal booster           */
extern void eq_file_set_by_index(u8 index);
extern void bt_tws_sync_eq(u8 eq);
void app_set_eq_mode(u8 *buffer, u8 buffer_size)
{
    u8 app_eq_mode;
	u8 buf[ ] ={0x01, 0x00, 0x00, 0x69, 0x00, 0x01, 0x00, 0x01, 0x68,  0x01};
    buf[1] = buffer[1]; /*spp和ble的区分返回给客户*/
    buf[7] = buffer[7];
    buf[6] = buffer[6];
	log_info("app_set_eq_mode\n");
	app_eq_mode = buffer[9];
	//eq_file_set_by_index(buffer[3]);
    if(app_eq_mode == 0){
        tws_api_sync_call_by_uuid('T', SYNC_CMD_APP_EQ1, 300);
    } else if(app_eq_mode == 1){
        tws_api_sync_call_by_uuid('T', SYNC_CMD_APP_EQ2, 300);
    } else if(app_eq_mode == 2){
        tws_api_sync_call_by_uuid('T', SYNC_CMD_APP_EQ3, 300);
    } else if(app_eq_mode == 3){
        tws_api_sync_call_by_uuid('T', SYNC_CMD_APP_EQ4, 300);
    } else if(app_eq_mode == 4){
        tws_api_sync_call_by_uuid('T', SYNC_CMD_APP_EQ5, 300);
    } else if(app_eq_mode == 5){
        tws_api_sync_call_by_uuid('T', SYNC_CMD_APP_EQ6, 300);
    }
	log_info("S12 : eq_to_app:\n");
    user_send_data(buf,sizeof(buf)/sizeof(buf[0]));
    log_info("\n client :send buf[3] = 0x%x ----len:%d  \n",buf[3],sizeof(buf)/sizeof(buf[0]));
}

/*0x6a  获取耳机预设EQ                */
extern u8 get_eq_mode();
void app_get_eq_mode(u8 *buffer, u8 buffer_size)
{
	u8 buf[ ] ={0x01, 0x00, 0x00, 0x6A, 0x00, 0x01, 0x00, 0x01,  0x6B,  0x01};
    buf[1] = buffer[1]; /*spp和ble的区分返回给客户*/
    buf[7] = buffer[7];
    buf[6] = buffer[6];
	log_info("s12 app_get_eq_mode:%d len %d \n",get_eq_mode(),sizeof(buf)/sizeof(buf[0]));
    buf[9] = get_eq_mode();
    user_send_data(buf,sizeof(buf)/sizeof(buf[0]));

}
/*0x6B 设置娱乐模式开关        */
extern void bt_set_low_latency_mode(int enable);
void app_set_low_latency_mode(u8 *buffer, u8 buffer_size)
{
    u8 buf[ ] ={0x01, 0x00, 0x00, 0x6B, 0x00, 0x01, 0x00, 0x01, 0x6A,  0x01};
    buf[1] = buffer[1]; /*spp和ble的区分返回给客户*/
    buf[7] = buffer[7];
    buf[6] = buffer[6];
    // if((1 == buffer[9] && buffer[1] == 0) || (0 == buffer[9] && buffer[1] == 1)){/*安卓和ios设置的值是反的*/
    //     bt_set_low_latency_mode(ENABLE);
    //     log_info("s12 app_set_low_latency_mode on \n");
    // }
    // else{
    //     bt_set_low_latency_mode(DISABLE);
    //     log_info("s12 app_set_low_latency_mode off \n");
    // }
    if(1 == buffer[9]) /*万魔要求改为直接设置低延迟模式*/
    {
        bt_set_low_latency_mode(ENABLE);
        log_info("s12 app_set_low_latency_mode on \n");
    }
    else{
        bt_set_low_latency_mode(DISABLE);
        log_info("s12 app_set_low_latency_mode off \n");
    }
	log_info("s12 app_set_low_latency_mode : set value %d len %d \n",buffer[9],sizeof(buf)/sizeof(buf[0]));
    user_send_data(buf,sizeof(buf)/sizeof(buf[0]));
}
/*0x6c 获取娱乐模式开关设置          */
extern int bt_get_low_latency_mode();
void app_get_low_latency_mode(u8 *buffer, u8 buffer_size)
{
    u8 buf[ ] ={0x01, 0x00, 0x00, 0x6C, 0x00, 0x01, 0x00, 0x01, 0x6D,  0x00};
    buf[1] = buffer[1]; /*spp和ble的区分返回给客户*/
    buf[7] = buffer[7];
    buf[6] = buffer[6];
    // if(1 == buffer[1]) {/*安卓和ios设置的值是反的*/
    //     buf[9] = !bt_get_low_latency_mode();
    //     log_info("s12 spp return value buf[9] = %d \n",buf[9]);
    // }else if(0 == buffer[1]){
    //     buf[9] = bt_get_low_latency_mode();
    //     log_info("s12 ble return value buf[9] = %d \n",buf[9]);
    // }
    buf[9] = bt_get_low_latency_mode();
    log_info("s12 ble return value buf[9] = %d \n",buf[9]);
	log_info("s12 bt_get_low_latency_mode %d len %d \n",buffer[9],sizeof(buf)/sizeof(buf[0]));
    user_send_data(buf,sizeof(buf)/sizeof(buf[0]));
}
/*0x84 设置操作自定义(单个)         	*/
void app_set_key_func(u8 *buffer, u8 buffer_size)
{  
    u8 buf[ ] ={0x01, 0x00, 0x00, 0x84, 0x00, 0x01, 0x00, 0x01, 0x85,  0x01};
    buf[1] = buffer[1]; /*spp和ble的区分返回给客户*/
    buf[7] = buffer[7];
    buf[6] = buffer[6];
	u8 key_data[8];
    get_key_setting(key_data);
	log_info("get_key:\n");
	printf_buf(key_data,8);
	if(1 == buffer[9]){           //设置左耳机双击功能
	    key_data[2] = buffer[10];
	} else if(2 == buffer[9]){    //设置右耳机双击功能
	    key_data[3] = buffer[10];
	} else if(3 == buffer[9]){    //设置左耳机三击功能
	    key_data[4] = buffer[10];
	} else if(4 == buffer[9]){    //设置右耳机三击功能
	    key_data[5] = buffer[10];
	}else if(5 == buffer[9]){     //设置长按左耳功能
	    key_data[6] = buffer[10];
	}else if(6 == buffer[9]){     //设置长按右耳功能
	    key_data[7] = buffer[10];
	}
	set_key_setting(key_data);
    deal_key_setting(NULL, 1, 1);
	log_info("s12 app_set_key_func func: %d len %d \n",buffer[10],sizeof(buf)/sizeof(buf[0]));
    user_send_data(buf,sizeof(buf)/sizeof(buf[0]));
}
/*0x85 获取操作自定义设置(单个)     	*/
void app_get_key_func(u8 *buffer, u8 buffer_size)
{  
    u8 buf[ ] ={0x01, 0x00, 0x00, 0x85, 0x00, 0x01, 0x00, 0x01, 0x84,  0x01};
    buf[1] = buffer[1]; /*spp和ble的区分返回给客户*/
    buf[7] = buffer[7];
    buf[6] = buffer[6];
	u8 key_data[8];
    get_key_setting(key_data);
	log_info("get_key:\n");
	printf_buf(key_data,8);
    switch (buffer[9]) {
        case 1: //获取左耳机双击功能
            buf[9]= key_data[2];
            break;
        case 2: //获取右耳机双击功能
            buf[9]= key_data[3];
            break;
        case 3: //获取左耳机三击功能
            buf[9]= key_data[4];
            break;
        case 4: //获取右耳机三击功能
            buf[9]= key_data[5];
            break;
        case 5: //获取长按左耳功能
            buf[9]= key_data[6];
            break;
        case 6: //获取长按右耳功能
            buf[9]= key_data[7];
            break;
        default :
            printf("app set value error!");
    }
	set_key_setting(key_data);
    deal_key_setting(NULL, 1, 1);
	log_info("s12 app_get_key_func func:%d len %d \n",buf[9],sizeof(buf)/sizeof(buf[0]));
    user_send_data(buf,sizeof(buf)/sizeof(buf[0]));
}

void reset_allkey_func()
{
    u8 key_data[8];
	key_data[2] = ADV_KEY_TYPE_PP;
	key_data[3] = ADV_KEY_TYPE_PP;
	key_data[4] = ADV_KEY_TYPE_NULL;
	key_data[5] = ADV_KEY_TYPE_NULL;
	key_data[6] = ADV_KEY_TYPE_OPEN_SIRI;
	key_data[7] = ADV_KEY_TYPE_OPEN_SIRI;
	set_key_setting(key_data); 
    deal_key_setting(key_data, 1, 1); 

}

/*0x86 操作自定义恢复默认           	*/
void app_reset_allkey_func(u8 *buffer, u8 buffer_size)
{  
    u8 buf[ ] ={0x01, 0x00, 0x00, 0x86, 0x00, 0x01, 0x00, 0x01, 0x87,  0x01};
    buf[1] = buffer[1]; /*spp和ble的区分返回给客户*/
    buf[7] = buffer[7];
    buf[6] = buffer[6];
	reset_allkey_func();
	log_info("s12 app_reset_allkey_func ");
    user_send_data(buf,sizeof(buf)/sizeof(buf[0]));
}
/* 0x87 获取操作自定义设置(全部)     	*/
void app_get_allkey_func(u8 *buffer, u8 buffer_size)
{
    u8 buf[ ] ={0x01, 0x00, 0x00, 0x87, 0x00, 0x04, 0x00, 0x01, 0x83,  0x01, 0x02, 0x03, 0x04};
    buf[1] = buffer[1]; /*spp和ble的区分返回给客户*/
    buf[7] = buffer[7];
	u8 key_data[8];
    get_key_setting(key_data);
	log_info("get_key:\n");
	printf_buf(key_data,8);
    buf[9] = key_data[2];
    buf[10] = key_data[3];
    buf[11] = key_data[4];
    buf[12] = key_data[5];
	log_info("s12 app_get_allkey_func ");
    user_send_data(buf,sizeof(buf)/sizeof(buf[0]));
}

void app_data_handle(u8 *buffer, u8 buffer_size)
{
    log_info("\n sylon add :app msg handle ---- \n");
    printf_buf(buffer, buffer_size);
    if((buffer[0] == 0x11) && (buffer[2] == 0x00)){ //协议头校验
        if(buffer[3] == 0x4d){         /*app下发的握手包               */ 
            app_connect_ack(buffer, buffer_size);
        }else if(buffer[3] == 0x4e){	/*获取TWS耳机信息              */
            app_get_tws_status(buffer, buffer_size);
        }else if(buffer[3] == 0x69){	/*设置耳机预设EQ               */
            app_set_eq_mode(buffer,buffer_size);
        }else if(buffer[3] == 0x6a){	/*获取耳机预设EQ               */
            app_get_eq_mode(buffer,buffer_size);
        }else if(buffer[3] == 0x6b){	/*设置娱乐模式开关              */
            app_set_low_latency_mode(buffer,buffer_size);
        }else if(buffer[3] == 0x6c){	/*获取娱乐模式开关设置          */
            app_get_low_latency_mode(buffer,buffer_size);
        }else if(buffer[3] == 0x84){	/*设置操作自定义(单个)         	*/
            app_set_key_func(buffer,buffer_size);
        }else if(buffer[3] == 0x85){	/*获取操作自定义设置(单个)     	*/
            app_get_key_func(buffer,buffer_size);
        }else if(buffer[3] == 0x86){	/*操作自定义恢复默认           	*/
            app_reset_allkey_func(buffer,buffer_size);
        }else if(buffer[3] == 0x87){	/*获取操作自定义设置(全部)     	*/
            app_get_allkey_func(buffer,buffer_size);
        }   
        
    }
}
/* LISTING_START(attWrite): ATT Write */
static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    int result = 0;
    u16 tmp16;

    u16 handle = att_handle;

    /* log_info("write_callback, handle= 0x%04x\n", handle); */

    switch (handle) {
        case ATT_CHARACTERISTIC_2a00_01_VALUE_HANDLE:
            tmp16 = BT_NAME_LEN_MAX;
            if ((offset >= tmp16) || (offset + buffer_size) > tmp16) {
                break;
            }

            if (offset == 0) {
                memset(gap_device_name, 0x00, BT_NAME_LEN_MAX);
            }
            memcpy(&gap_device_name[offset], buffer, buffer_size);
            log_info("\n------write gap_name:");
            break;

        case ATT_CHARACTERISTIC_ae02_01_CLIENT_CONFIGURATION_HANDLE:
            set_ble_work_state(BLE_ST_NOTIFY_IDICATE);
            check_connetion_updata_deal();
            log_info("\n------write ccc:%04x, %02x\n", handle, buffer[0]);
            att_set_ccc_config(handle, buffer[0]);
            can_send_now_wakeup();
            break;
        case ATT_CHARACTERISTIC_09412640_BCE7_11E5_9E04_0002A5D5C51B_01_VALUE_HANDLE:
            app_data_handle(buffer,buffer_size);
            break;
        case ATT_CHARACTERISTIC_18AC9920_BCE7_11E5_AE01_0002A5D5C51B_01_CLIENT_CONFIGURATION_HANDLE:
            set_ble_work_state(BLE_ST_NOTIFY_IDICATE);
            check_connetion_updata_deal();
            log_info("\n sylon add : ------write ccc:%04x, %02x\n", handle, buffer[0]);
            att_set_ccc_config(handle, buffer[0]);
            can_send_now_wakeup();
            break;


        case ATT_CHARACTERISTIC_ae01_01_VALUE_HANDLE:
            /* if (!JL_rcsp_get_auth_flag()) { */
            /*     JL_rcsp_auth_recieve(buffer, buffer_size); */
            /*     break; */
            /* } */
            if (app_recieve_callback) {
                app_recieve_callback(0, buffer, buffer_size);
            }
#if ASSISTED_HEARING_CUSTOM_TRASNDATA
    app_third_party_hearing_aid_handle(buffer, buffer_size);
#endif
            //		JL_rcsp_auth_recieve(data, len);
            break;


        default:
            break;
    }

    return 0;
}




static int app_send_user_data(u16 handle, u8 *data, u16 len, u8 handle_type)
{
    u32 ret = APP_BLE_NO_ERROR;

    if (!con_handle) {
        return APP_BLE_OPERATION_ERROR;
    }

    ret = ble_user_cmd_prepare(BLE_CMD_ATT_SEND_DATA, 4, handle, data, len, handle_type);
    if (ret == BLE_BUFFER_FULL) {
        ret = APP_BLE_BUFF_FULL;
    }

    if (ret) {
        log_info("app_send_fail:%d !!!!!!\n", ret);
    }
    return ret;
}


//------------------------------------------------------
extern void swapX(const uint8_t *src, uint8_t *dst, int len);
extern const u8 *bt_get_mac_addr();
static const u8 dueros_dma_uuid_16bit[] = {0x04, 0xFE};
//------------------------------------------------------
#if TCFG_CHARGE_BOX_ENABLE
extern u8 *get_chargebox_adv_addr(void);
#endif

static void rcsp_adv_fill_mac_addr(u8 *mac_addr_buf)
{
#if (MUTIl_CHARGING_BOX_EN)
    u8 *mac_addr = get_chargebox_adv_addr();
    if (mac_addr) {
        swapX(mac_addr, mac_addr_buf, 6);
    }

    /* printf("mac_addr:"); */
    /* printf_buf(mac_addr_buf, 6); */

#else
    swapX(bt_get_mac_addr(), mac_addr_buf, 6);
#endif
}

typedef struct __flash_of_lic_para_head {
    s16 crc;
    u16 string_len;
    const u8 para_string[];
} __attribute__((packed)) _flash_of_lic_para_head;

//u8 local_read_licens[12];

static bool license_para_head_check(u8 *para)
{
    _flash_of_lic_para_head *head;

    //fill head
    head = (_flash_of_lic_para_head *)para;

    ///crc check
    u8 *crc_data = (u8 *)(para + sizeof(((_flash_of_lic_para_head *)0)->crc));
    u32 crc_len = sizeof(_flash_of_lic_para_head) - sizeof(((_flash_of_lic_para_head *)0)->crc)/*head crc*/ + (head->string_len)/*content crc,include end character '\0'*/;
    s16 crc_sum = 0;

    crc_sum = CRC16(crc_data, crc_len);

    if (crc_sum != head->crc) {
        printf("license crc error !!! %x %x \n", (u32)crc_sum, (u32)head->crc);
        return false;
    }

    return true;
}

u8 *app_protocal_get_license_ptr(void)
{
    u32 flash_capacity = sdfile_get_disk_capacity();
    u32 flash_addr = flash_capacity - 256 + 80;
    u8 *lic_ptr = NULL;
    _flash_of_lic_para_head *head;

    printf("flash capacity:%x \n", flash_capacity);
    lic_ptr = (u8 *)sdfile_flash_addr2cpu_addr(flash_addr);

    //head length check
    head = (_flash_of_lic_para_head *)lic_ptr;
    if (head->string_len >= 0xff) {
        printf("license length error !!! \n");
        return NULL;
    }

    ////crc check
    if (license_para_head_check(lic_ptr) == (false)) {
        printf("license head check fail\n");
        return NULL;
    }
    printf("app_protocal_get_license_ptr:\n");

    put_buf(lic_ptr, 128);

    lic_ptr += sizeof(_flash_of_lic_para_head);
    return lic_ptr;


}
extern bool sfc_erase(int cmd, u32 addr);
extern u32 sfc_write(const u8 *buf, u32 addr, u32 len);
int app_protocol_license2flash(const u8 *data, u16 len)
{
    _flash_of_lic_para_head header;
    u8 buffer[256];
    u32 flash_capacity = sdfile_get_disk_capacity();
    u32 sector = flash_capacity - 4 * 1024;
    u32 page_addr = flash_capacity - 256;
    u8 *auif_ptr = (u8 *)sdfile_flash_addr2cpu_addr(page_addr);
    ///save last 256 byte
    /* memset(buffer, 0xff, sizeof(buffer)); */
    memcpy(buffer, auif_ptr, sizeof(buffer));
    auif_ptr += 80;

    header.string_len = len;
    ///length
    memcpy(&buffer[80 + sizeof(header.crc)], &(header.string_len), sizeof(header.string_len));
    ///context
    memcpy(&buffer[80 + sizeof(header)], data, len);
    ///crc
    header.crc = CRC16(&buffer[80 + sizeof(header.crc)], sizeof(header.string_len) + header.string_len);
    memcpy(&buffer[80], &(header.crc), sizeof(header.crc));

    ///check if need update data to flash,erease license erea if there are some informations in license erea
    if (!memcmp(auif_ptr, buffer + 80, len + sizeof(_flash_of_lic_para_head))) {
        printf("flash information the same with license\n");
        return 0;
    }
    printf("erase license sector \n");
    sfc_erase(2, sector);

    printf("write license to flash \n");
    sfc_write(buffer, page_addr, 256);

    return 0;
}



#define APP_RCSP_ENABLE         0
//static char le_adv_license[] = "957f4eab0302f5ee";
#define LE_ADV_LICENSE_SIZE     17

static char le_adv_license[LE_ADV_LICENSE_SIZE] = {0};

static unsigned char le_adv_data[] = {
    // CID
    0xD6, 0x05,
    // PID
    0x41,
    // byte4 MAC
    0x12, 0x34, 0x56, 0xC2, 0x9A ,0xCB,
    // byte13 LICENSE
    0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00,
//0x00, 0x00, 0x00, 0x00
    // 0X64, 0X64, 0X33, 0X63, 0X38, 0X30, 0X39, 0X33, 0X62, 0X38, 0X32 ,0X36, 0X64, 0X61, 0X33, 0X38
};
extern u8 get_defalut_bt_channel_sel(void);
static int make_set_adv_data(void)
{
#if APP_RCSP_ENABLE

    u8 i;
    u8 *buf = adv_data;
    buf[0] = 0x1E;
    buf[1] = 0xFF;

    buf[2] = 0xD6;	// JL ID
    buf[3] = 0x05;

#if 1
    u16 vid = get_vid_pid_ver_from_cfg_file(GET_VID_FROM_EX_CFG);
    buf[4] = vid & 0xFF;
    buf[5] = vid >> 8;

    u16 pid = get_vid_pid_ver_from_cfg_file(GET_PID_FROM_EX_CFG);
    buf[6] = pid & 0xFF;
    buf[7] = pid >> 8;
#else
    buf[4] = 0x02;	// VID
    buf[5] = 0x00;

    buf[6] = 0x0F;	// PID
    buf[7] = 0x00;
#endif

#if (MUTIl_CHARGING_BOX_EN)
    buf[8] = 0x21;	//   2:TWS耳机类型   |  protocol verson
#else
    buf[8] = 0x20;	//   2:TWS耳机类型   |  protocol verson
    if (RCSP_SPP == get_defalut_bt_channel_sel()) {
        buf[8] |= 2;
    }
#endif

#if (RCSP_ADV_HALTER_ENABLE)
    buf[8] = 0x23;
#endif

    rcsp_adv_fill_mac_addr(&buf[9]);
    buf[15] = __this->connect_flag;

    /* buf[16] = 0xC1; */
    /* buf[17] = 0x40; */
    /* buf[18] = 0x30; */
    buf[16] = __this->bat_percent_L ? (((!!__this->bat_charge_L) << 7) | (__this->bat_percent_L & 0x7F)) : 0;
    buf[17] = __this->bat_percent_R ? (((!!__this->bat_charge_R) << 7) | (__this->bat_percent_R & 0x7F)) : 0;
    buf[18] = __this->bat_percent_C ? (((!!__this->bat_charge_C) << 7) | (__this->bat_percent_C & 0x7F)) : 0;

    memset(&buf[19], 0x00, 4);							// reserve

    buf[19] = __this->seq_rand;

#if (MUTIl_CHARGING_BOX_EN)
    buf[20] = 1;
#else
    if (RCSP_SPP == get_defalut_bt_channel_sel()) {
        buf[20] = 1;
    }
#endif

    u8 t_buf[16];
    btcon_hash(&buf[2], 16, &buf[15], 4, t_buf);		// hash

    for (i = 0; i < 8; i++) {								// single byte
        buf[23 + i] = t_buf[2 * i + 1];
    }
#else
		u8 offset = 0;
		u8 *buf = adv_data;

		// if(con_handle) {
        //     le_adv_data[4] |= BIT(0);
        // } else {
        //     le_adv_data[4] &= ~BIT(0);
        // }

        // // MAC ADDR
		// for(unsigned char i = 0; i < 6; i++) {
		// 	le_adv_data[7 + i] ^= 0xad;
		// }
		// // step 1 read flash license
		// // step 2 read vm license
		// // step 3 sync tws license
		// // write license to vm
		//strcpy(&le_adv_data[13], le_adv_license);
		swapX(bt_get_mac_addr(), &le_adv_data[3], 6);
        #include "log.h"
        log_info("sylon debug: ");
        put_buf(bt_get_mac_addr(),6);
        log_info("sylon debug:111 ");
		put_buf(le_adv_data,  sizeof(le_adv_data));
        offset += make_eir_packet_val(&buf[offset], offset, HCI_EIR_DATATYPE_FLAGS, 0x18, 1);
		offset += make_eir_packet_data(&buf[offset], offset, HCI_EIR_DATATYPE_MANUFACTURER_SPECIFIC_DATA, le_adv_data, sizeof(le_adv_data));

        memcpy(&le_adv_data[9],local_read_licens,12);    


		if (offset > ADV_RSP_PACKET_MAX) {
			return -1;
		}
#endif

    __this->modify_flag = 0;
    adv_data_len = 31;
    log_info("ADV data() send:");
    ble_user_cmd_prepare(BLE_CMD_ADV_DATA, 2, 31, buf);
    log_info("ADV data():");
    put_buf(buf, 31);
    return 0;
}

static int make_set_rsp_data(void)
{
    u8 offset = 0;
    u8 *buf = scan_rsp_data;
    u8 *edr_name = bt_get_local_name();

    offset += make_eir_packet_val(&buf[offset], offset, HCI_EIR_DATATYPE_FLAGS, 0x06, 1);
    offset += make_eir_packet_data(&buf[offset], offset, HCI_EIR_DATATYPE_COMPLETE_LOCAL_NAME, (void *)edr_name, strlen(edr_name));
    printf("RSP data(%d):,offset");
    put_buf(buf, offset);
    printf("edr name:");
    put_buf(edr_name, strlen(edr_name));
    scan_rsp_data_len = 31;
    ble_user_cmd_prepare(BLE_CMD_RSP_DATA, 2, 31, buf);
    return 0;
}


static int update_adv_data(u8 *buf)
{
    u8 i;
    buf[1] = 0xD6;	// JL ID
    buf[0] = 0x05;

#if 1
    u16 vid = get_vid_pid_ver_from_cfg_file(GET_VID_FROM_EX_CFG);
    buf[3] = vid & 0xFF;
    buf[2] = vid >> 8;

    u16 pid = get_vid_pid_ver_from_cfg_file(GET_PID_FROM_EX_CFG);
    buf[5] = pid & 0xFF;
    buf[4] = pid >> 8;
#else
    buf[3] = 0x02;	// VID
    buf[2] = 0x00;

    buf[5] = 0x0F;	// PID
    buf[4] = 0x00;
#endif

    buf[6] = 0x20;	//   2:TWS耳机类型   |  protocol verson
    if (RCSP_SPP == get_defalut_bt_channel_sel()) {
        buf[6] |= 2;
    }
#if (RCSP_ADV_HALTER_ENABLE)
    buf[6] = 0x23;
#endif

    swapX(bt_get_mac_addr(), &buf[7], 6);

    buf[13] = __this->connect_flag;

    buf[14] = __this->bat_percent_L ? (((!!__this->bat_charge_L) << 7) | (__this->bat_percent_L & 0x7F)) : 0;
    buf[15] = __this->bat_percent_R ? (((!!__this->bat_charge_R) << 7) | (__this->bat_percent_R & 0x7F)) : 0;
    buf[16] = __this->bat_percent_C ? (((!!__this->bat_charge_C) << 7) | (__this->bat_percent_C & 0x7F)) : 0;

    buf[17] = __this->seq_rand;

    return 0;
}

u8 *ble_get_scan_rsp_ptr(u16 *len)
{
    if (len) {
        *len = scan_rsp_data_len;
    }
    return scan_rsp_data;
}

u8 *ble_get_adv_data_ptr(u16 *len)
{
    if (len) {
        *len = adv_data_len;
    }
    return adv_data;
}

u8 *ble_get_gatt_profile_data(u16 *len)
{
    *len = sizeof(profile_data);
    return (u8 *)profile_data;
}

//广播参数设置
static void advertisements_setup_init()
{
    uint8_t adv_type = 0;
    uint8_t adv_channel = 7;
    int   ret = 0;
    u16 adv_interval = 0x30;

    //ble_user_cmd_prepare(BLE_CMD_ADV_PARAM, 3, adv_interval, adv_type, adv_channel);
      ble_user_cmd_prepare(BLE_CMD_ADV_PARAM, 3, ADV_INTERVAL_MIN, adv_type, adv_channel);

    ret |= make_set_adv_data();
    ret |= make_set_rsp_data();

    if (ret) {
        puts("advertisements_setup_init fail !!!!!!\n");
        return;
    }

}


void ble_sm_setup_init(io_capability_t io_type, u8 auth_req, uint8_t min_key_size, u8 security_en)
{
    //setup SM: Display only
    sm_init();
    sm_set_io_capabilities(io_type);
    sm_set_authentication_requirements(auth_req);
    sm_set_encryption_key_size_range(min_key_size, 16);
    sm_set_request_security(0);
    sm_event_callback_set(&cbk_sm_packet_handler);
}


extern void le_device_db_init(void);
void ble_profile_init(void)
{
    printf("ble profile init\n");
    le_device_db_init();
    ble_sm_setup_init(IO_CAPABILITY_NO_INPUT_NO_OUTPUT, SM_AUTHREQ_BONDING, 7, TCFG_BLE_SECURITY_EN);
    /* setup ATT server */
    att_server_init(profile_data, att_read_callback, att_write_callback);
    att_server_register_packet_handler(cbk_packet_handler);
    /* gatt_client_register_packet_handler(packet_cbk); */

    // register for HCI events
    hci_event_callback_set(&cbk_packet_handler);
    /* ble_l2cap_register_packet_handler(packet_cbk); */
    /* sm_event_packet_handler_register(packet_cbk); */
    le_l2cap_register_packet_handler(&cbk_packet_handler);
}


static int set_adv_enable(void *priv, u32 en)
{
    ble_state_e next_state, cur_state;

    if (!adv_ctrl_en) {
        return APP_BLE_OPERATION_ERROR;
    }

    printf("con_handle %d\n", con_handle);
    if (con_handle) {
        return APP_BLE_OPERATION_ERROR;
    }

    if (en) {
        next_state = BLE_ST_ADV;
    } else {
        next_state = BLE_ST_IDLE;
    }

    cur_state =  get_ble_work_state();
    printf("get_ble_work_state  %d\n", cur_state);
    switch (cur_state) {
    case BLE_ST_ADV:
    case BLE_ST_IDLE:
    case BLE_ST_INIT_OK:
    case BLE_ST_NULL:
    case BLE_ST_DISCONN:
        break;
    default:
        return APP_BLE_OPERATION_ERROR;
        break;
    }

    if (cur_state == next_state) {
        printf("cur_state == next_state \n");
        return APP_BLE_NO_ERROR;
    }
    log_info("adv_en:%d\n", en);
    set_ble_work_state(next_state);
    if (en) {
        advertisements_setup_init();
    }
    ble_user_cmd_prepare(BLE_CMD_ADV_ENABLE, 1, en);
    printf(">  set_adv_enable  %d\n", en);
    return APP_BLE_NO_ERROR;
}

static int ble_disconnect(void *priv)
{
    if (con_handle) {
        if (BLE_ST_SEND_DISCONN != get_ble_work_state()) {
            log_info(">>>ble send disconnect\n");
            set_ble_work_state(BLE_ST_SEND_DISCONN);
            ble_user_cmd_prepare(BLE_CMD_DISCONNECT, 1, con_handle);
        } else {
            log_info(">>>ble wait disconnect...\n");
        }
        return APP_BLE_NO_ERROR;
    } else {
        return APP_BLE_OPERATION_ERROR;
    }
}


static int get_buffer_vaild_len(void *priv)
{
    u32 vaild_len = 0;
    ble_user_cmd_prepare(BLE_CMD_ATT_VAILD_LEN, 1, &vaild_len);
    return vaild_len;
}

static int app_send_user_data_do(void *priv, u8 *data, u16 len)
{
#if PRINT_DMA_DATA_EN
    if (len < 128) {
        log_info("-dma_tx(%d):", len);
        log_info_hexdump(data, len);
    } else {
        putchar('L');
    }
#endif
    return app_send_user_data(ATT_CHARACTERISTIC_ae02_01_VALUE_HANDLE, data, len, ATT_OP_AUTO_READ_CCC);
}

static int app_send_user_data_check(u16 len)
{
    u32 buf_space = get_buffer_vaild_len(0);
    if (len <= buf_space) {
        return 1;
    }
    return 0;
}

static int regiest_wakeup_send(void *priv, void *cbk)
{
    ble_resume_send_wakeup = cbk;
    return APP_BLE_NO_ERROR;
}

static int regiest_recieve_cbk(void *priv, void *cbk)
{
    channel_priv = (u32)priv;
    app_recieve_callback = cbk;
    return APP_BLE_NO_ERROR;
}

static int regiest_state_cbk(void *priv, void *cbk)
{
    channel_priv = (u32)priv;
    app_ble_state_callback = cbk;
    return APP_BLE_NO_ERROR;
}

void bt_ble_adv_enable(u8 enable)
{
    set_adv_enable(0, enable);
}

extern u8 get_charge_online_flag(void);
static u8 update_dev_battery_level(void)
{

    u8 master_bat = 0;
    u8 master_charge = 0;
    u8 slave_bat = 0;
    u8 slave_charge = 0;
    u8 box_bat = 0;
    u8 box_charge = 0;

//	Master bat
#if CONFIG_DISPLAY_DETAIL_BAT
    master_bat = get_vbat_percent();
#else
    master_bat = get_self_battery_level() * 10 + 10;
#endif
    if (master_bat > 100) {
        master_bat = 100;
    }
    master_charge = get_charge_online_flag();


// Slave bat

#if	TCFG_USER_TWS_ENABLE
    slave_bat = get_tws_sibling_bat_persent();
#if TCFG_CHARGESTORE_ENABLE
    if (slave_bat == 0xff) {
        /* log_info("--update_bat_01\n"); */
        if (get_bt_tws_connect_status()) {
            slave_bat = chargestore_get_sibling_power_level();
        }
    }
#endif

    if (slave_bat == 0xff) {
        /*  log_info("--update_bat_02\n"); */
        slave_bat = 0x00;
    }

    slave_charge = !!(slave_bat & 0x80);
    slave_bat &= 0x7F;
#else
    slave_charge = 0;
    slave_bat = 0;
#endif

// Box bat
    if ((master_charge && (master_bat != 0))
        || (slave_charge && (slave_bat != 0))) {
        //earphone in charge
#if TCFG_CHARGESTORE_ENABLE
        box_bat = chargestore_get_power_level();
        if (box_bat == 0xFF) {
            box_bat = 0;
        }
#else
        box_bat = 0;
#endif
    } else {
        //all not in charge
        box_bat = 0;
    }
    box_charge = !!(box_bat & 0x80);
    box_bat &= 0x7F;
// check if master is L

    u8 tbat_charge_L = 0;
    u8 tbat_charge_R = 0;
    u8 tbat_percent_L = 0;
    u8 tbat_percent_R = 0;
    u8 tbat_percent_C = box_bat;
    u8 tbat_charge_C = box_charge;
#if TCFG_USER_TWS_ENABLE
    if (get_bt_tws_connect_status()) {
        if ('L' == tws_api_get_local_channel()) {
            tbat_charge_L = master_charge;
            tbat_charge_R = slave_charge;
            tbat_percent_L = master_bat;
            tbat_percent_R = slave_bat;
        } else if ('R' == tws_api_get_local_channel()) {
            tbat_charge_L = slave_charge;
            tbat_charge_R = master_charge;
            tbat_percent_L = slave_bat;
            tbat_percent_R = master_bat;
        }
    } else {
        switch (tws_api_get_local_channel()) {
        case 'R':
            tbat_charge_R = master_charge;
            tbat_percent_R = master_bat;
            break;
        case 'L':
        default:
            tbat_charge_L = master_charge;
            tbat_percent_L = master_bat;
            break;
        }
    }
#else
    tbat_charge_L = master_charge;
    tbat_percent_L = master_bat;
#endif

#if 0//TCFG_CHARGESTORE_ENABLE
    u8 tpercent = 0;
    u8 tcharge = 0;
    if (chargestore_get_earphone_pos() == 'L') {
        //switch position
        log_info("is L pos\n");
        tpercent = tbat_percent_R;
        tcharge = tbat_charge_R;
        tbat_percent_R = tbat_percent_L;
        tbat_charge_R = tbat_charge_L;
        tbat_percent_L = tpercent;
        tbat_charge_L = tcharge;
    } else {
        log_info("is R pos\n");
    }
#endif

    // 防止充电仓没电时，耳机进仓会只显示耳机充电但充电仓没有电量的情况
    if ((!!tbat_charge_L) || (!!tbat_charge_R) || (!!tbat_charge_C)) {
        if (((tbat_percent_C + 1) & 0x7F) <= 0x2) {
            tbat_percent_C = 0x2;
        }
    }

    // 防止充电仓电量跳变
    if (__this->bat_percent_C && ((!!tbat_charge_L) || (!!tbat_charge_R))) {
        // 左耳或右耳进仓
        if ((!!tbat_charge_C)) {
            // 充电仓充电, 新值比旧值大
            if ((__this->bat_percent_C > tbat_percent_C) && (__this->bat_percent_C == (tbat_percent_C + 1))) {
                tbat_percent_C = __this->bat_percent_C;
            }
        } else {
            // 充电仓没有充电, 新值比旧值小
            if ((__this->bat_percent_C < tbat_percent_C) && ((__this->bat_percent_C + 1) == tbat_percent_C)) {
                tbat_percent_C = __this->bat_percent_C;
            }
        }
    }
// check if change
    u8 tchange = 0;
    if ((tbat_charge_L  != __this->bat_charge_L)
        || (tbat_charge_R  != __this->bat_charge_R)
        || (tbat_charge_C  != __this->bat_charge_C)
        || (tbat_percent_L != __this->bat_percent_L)
        || (tbat_percent_R != __this->bat_percent_R)
        || (tbat_percent_C != __this->bat_percent_C)) {
        tchange = 1;
    }

    __this->bat_charge_L  = tbat_charge_L;
    __this->bat_charge_R  = tbat_charge_R;
    __this->bat_charge_C  = tbat_charge_C;
    __this->bat_percent_L = tbat_percent_L;
    __this->bat_percent_R = tbat_percent_R;
    __this->bat_percent_C = tbat_percent_C;

    /* u8 *buf = adv_data; */
    /* buf[16] = __this->bat_percent_L ? (((!!__this->bat_charge_L) << 7) | (__this->bat_percent_L & 0x7F)):0; */
    /* buf[17] = __this->bat_percent_R ? (((!!__this->bat_charge_R) << 7) | (__this->bat_percent_R & 0x7F)):0; */
    /* buf[18] = __this->bat_percent_C ? (((!!__this->bat_charge_C) << 7) | (__this->bat_percent_C & 0x7F)):0; */

    return tchange;
}


static u8 update_dev_connect_flag(void)
{
    u8 old_flag = __this->connect_flag;


    extern u8 get_bt_connect_status(void);
    if (get_bt_connect_status() ==  BT_STATUS_WAITINT_CONN) {
        __this->connect_flag = 1;
    } else {
        __this->connect_flag = 2;
    }

    if (old_flag != __this->connect_flag) {
        return 1;
    }
    return 0;
}

#if(TCFG_CHARGE_BOX_ENABLE)
extern u8 get_tws_ear_status(void);
#endif
extern u8 get_rcsp_connect_status(void);
extern u8 JL_get_cur_bt_channel_sel(void);
static void bt_ble_rcsp_adv_enable_do(void)
{
#if(TCFG_CHARGE_BOX_ENABLE)
    if (!get_tws_ear_status()) {
        return;
    }
#endif
    if (tws_api_get_role() == TWS_ROLE_SLAVE) {
        if (!(get_rcsp_connect_status() && RCSP_SPP == JL_get_cur_bt_channel_sel())) {
            return;
        }
    }


    ble_state_e cur_state =  get_ble_work_state();
    // battery
    if (update_dev_battery_level()) {
        __this->modify_flag = 1;
    }

    // connect flag
    /* if (update_dev_connect_flag()) { */
    /* __this->modify_flag = 1; */
    /* } */

    extern u8 adv_info_notify(u8 * buf, u8 len);
    extern u8 JL_rcsp_get_auth_flag(void);
#if (!MUTIl_CHARGING_BOX_EN) && (RCSP_ADV_EN) && (TCFG_USER_TWS_ENABLE)
    if ((con_handle || get_rcsp_connect_status()) && JL_rcsp_get_auth_flag()) {
        extern void deal_adv_setting_gain_time_stamp(void);
        deal_adv_setting_gain_time_stamp();
    }
#endif
    if ((con_handle || get_rcsp_connect_status()) && JL_rcsp_get_auth_flag() && (__this->ble_adv_notify || __this->modify_flag)) {
        printf("adv con_handle %d\n", con_handle);
        /* u8 data[5]; */
        /* data[0] = __this->seq_rand; */
        /* data[1] = __this->connect_flag; */
        /* data[2] = __this->bat_percent_L ? (((!!__this->bat_charge_L) << 7) | (__this->bat_percent_L & 0x7F)):0; */
        /* data[3] = __this->bat_percent_R ? (((!!__this->bat_charge_R) << 7) | (__this->bat_percent_R & 0x7F)):0; */
        /* data[4] = __this->bat_percent_C ? (((!!__this->bat_charge_C) << 7) | (__this->bat_percent_C & 0x7F)):0; */
        /* adv_info_notify(data, 5); */
        u8 data[18];
        update_adv_data(data);
        adv_info_notify(data, 18);
        __this->modify_flag = 0;
        return;
    }

#if (!MUTIl_CHARGING_BOX_EN)
    if (cur_state != BLE_ST_ADV) {
        return;
    }
#endif

    if (__this->modify_flag == 0) {
        return;
    }

    printf("adv modify!!!!!!\n");
    bt_ble_adv_enable(0);
    make_set_adv_data();
    make_set_rsp_data();
    bt_ble_adv_enable(1);
}

void bt_ble_rcsp_adv_enable(void)
{
#if (MUTIl_CHARGING_BOX_EN)
    __this->modify_flag = 1;
    bt_ble_rcsp_adv_enable_do();
#endif
    ble_timer_handle = sys_timer_add(NULL, bt_ble_rcsp_adv_enable_do, BLE_TIMER_SET);
}


void bt_ble_rcsp_adv_disable(void)
{
    if (ble_timer_handle) {
        sys_timer_del(ble_timer_handle);
        ble_timer_handle = 0;
    }
    ble_app_disconnect();
    set_adv_enable(0, 0);
    bt_adv_seq_change();
}

// 同步seq到对端
static void deal_sibling_seq_rand_sync(void *data, u16 len, bool rx)
{
    u8 tws_need_update = 1;
    put_buf(data, len);
    if (rx) {
        switch (((u8 *)data)[0]) {
        case TWS_ADV_SEQ_CHANGE:
            __this->seq_rand = ((u8 *) data)[1];
            JL_rcsp_event_to_user(DEVICE_EVENT_FROM_RCSP, MSG_JL_UPDATE_SEQ, NULL, 0);
            break;
        case TWS_VERSON_INFO:
            sibling_ver_info  = ((u8 *)data)[1] | ((u8 *)data)[2] << 8;
            u16 cur_ver_info  = get_vid_pid_ver_from_cfg_file(GET_VER_FROM_EX_CFG);
            g_printf("get sibling version:0x%x cur_ver_info:0x%x\n", ((u8 *)data)[1] | ((u8 *)data)[2] << 8, cur_ver_info);
            if ((cur_ver_info > sibling_ver_info) && (tws_api_get_role() == TWS_ROLE_MASTER)) { //如果主机版本号比从机高需要进行role_switch
                /* if(bt_tws_phone_connected()) { */
                /*     rcsp_update_set_role_switch(1); */
                /* } else {             */
                /*     g_printf("rcsp_need role switch\n");                    //已经连接上手机在此处role_switch */
                /*     tws_conn_switch_role(); */
                /*     tws_api_auto_role_switch_disable(); */
                /* } */
            } else if (cur_ver_info == sibling_ver_info) {                  //如果tws连接同步版本号一致需要把标志清楚防止重复升级
                tws_need_update = 0;
                syscfg_write(VM_UPDATE_FLAG, &tws_need_update, sizeof(tws_need_update));
            }

            break;
        case TWS_UPDATE_INFO:                                               //单备份升级走BLE主机需要要通过该消息来通知从机进入升级
            bt_ble_rcsp_adv_disable();
            ble_module_enable(0);                                           //关闭广播防止从机被手机误回连
            g_printf("slave close adv...\n");
            sys_timeout_add(NULL,  update_slave_adv_reopen, 1000 * 60);     //延迟一分钟再开广播
            if (cur_ver_info <= sibling_ver_info) {
                syscfg_write(VM_UPDATE_FLAG, &tws_need_update, sizeof(tws_need_update));
                sys_auto_shut_down_disable();
            }
            break;

        }
    }
}
REGISTER_TWS_FUNC_STUB(adv_seq_rand_sync) = {
    .func_id = TWS_FUNC_ID_SEQ_RAND_SYNC,
    .func    = deal_sibling_seq_rand_sync,
};
void adv_seq_vaule_sync(void)
{
    syscfg_write(ADV_SEQ_RAND, &__this->seq_rand, sizeof(__this->seq_rand));
}
void bt_adv_seq_change(void)
{
    u8 trand;
    u8 data[2];
    syscfg_read(ADV_SEQ_RAND, &trand, 1);
    log_info("adv seq read: %x\n", trand);
    trand++;
    syscfg_write(ADV_SEQ_RAND, &trand, 1);
    log_info("adv seq write: %x\n", trand);
    __this->seq_rand = trand;
    data[0] = TWS_ADV_SEQ_CHANGE;
    data[1] = __this->seq_rand;
#if TCFG_USER_TWS_ENABLE
    if (get_bt_tws_connect_status() && tws_api_get_role() == TWS_ROLE_MASTER) {
        tws_api_send_data_to_sibling(data, sizeof(data), TWS_FUNC_ID_SEQ_RAND_SYNC);
    }
#endif
}

u8 get_ble_adv_modify(void)
{
    return __this->modify_flag;
}

u8 get_ble_adv_notify(void)
{
    return __this->ble_adv_notify;
}

void set_ble_adv_notify(u8 en)
{

    __this->ble_adv_notify = en;
}

u8 get_connect_flag(void)
{
    return __this->connect_flag;
}

void set_connect_flag(u8 value)
{
    __this->connect_flag = 	value;
}

void bt_ble_init_do(void)
{
    log_info("bt_ble_test_tag_do\n");
    //初始化三个电量值
    /* swapX(bt_get_mac_addr(), &user_tag_data[7], 6); */

    __this->connect_flag = 0;
    __this->bat_charge_L = 1;
    __this->bat_charge_R = 1;
    __this->bat_charge_C = 1;
    __this->bat_percent_L = 100;
    __this->bat_percent_R = 100;
    __this->bat_percent_C = 100;
    __this->modify_flag = 0;
    __this->ble_adv_notify = 0;
    update_dev_battery_level();
    bt_adv_seq_change();
    //update_dev_connect_flag();
}

int bt_ble_adv_ioctl(u32 cmd, u32 priv, u8 mode)
{
    printf(" bt_ble_adv_ioctl %d %d %d\n", cmd, priv, mode);
    ble_state_e cur_state =  get_ble_work_state();
    if (mode) {			// set
        switch (cmd) {
        case BT_ADV_ENABLE:
            break;
        case BT_ADV_DISABLE:
            if (cur_state == BLE_ST_ADV) {
                printf("ADV DISABLE !!!\n");
                bt_ble_adv_enable(0);
                return 0;
            }
            __this->connect_flag = SECNE_DISMISS;
            break;
        case BT_ADV_SET_EDR_CON_FLAG:
            __this->connect_flag = priv;
            if (priv == SECNE_UNCONNECTED) {
                bt_adv_seq_change();
            }
            break;
        case BT_ADV_SET_BAT_CHARGE_L:
            __this->bat_charge_L = priv;
            break;
        case BT_ADV_SET_BAT_CHARGE_R:
            __this->bat_charge_R = priv;
            break;
        case BT_ADV_SET_BAT_CHARGE_C:
            __this->bat_charge_C = priv;
            break;
        case BT_ADV_SET_BAT_PERCENT_L:
            __this->bat_percent_L = priv;
            break;
        case BT_ADV_SET_BAT_PERCENT_R:
            __this->bat_percent_R = priv;
            break;
        case BT_ADV_SET_BAT_PERCENT_C:
            __this->bat_percent_C = priv;
            break;
        case BT_ADV_SET_NOTIFY_EN:
            __this->ble_adv_notify = priv;
            break;
        default:
            return -1;
        }

        __this->modify_flag = 1;
    } else {			// get
        switch (cmd) {
        case BT_ADV_ENABLE:
        case BT_ADV_DISABLE:
            break;
        case BT_ADV_SET_EDR_CON_FLAG:
            break;
        case BT_ADV_SET_BAT_CHARGE_L:
            break;
        case BT_ADV_SET_BAT_CHARGE_R:
            break;
        case BT_ADV_SET_BAT_CHARGE_C:
            break;
        case BT_ADV_SET_BAT_PERCENT_L:
            break;
        case BT_ADV_SET_BAT_PERCENT_R:
            break;
        case BT_ADV_SET_BAT_PERCENT_C:
            break;
        default:
            break;
        }
    }
    return 0;
}

void ble_module_enable(u8 en)
{
    log_info("mode_en:%d\n", en);
    if (en) {
        adv_ctrl_en = 1;
#if (!MUTIl_CHARGING_BOX_EN)
        bt_ble_adv_enable(1);
#endif//MUTIl_CHARGING_BOX_EN

    } else {
        if (con_handle) {
            adv_ctrl_en = 0;
            ble_disconnect(NULL);
        } else {
            bt_ble_adv_enable(0);
            adv_ctrl_en = 0;
        }
    }
}

#if(TCFG_CHARGE_BOX_ENABLE)
extern u8 get_chgbox_lid_status(void);
#endif
void bt_ble_init(void)
{
    log_info("***** ble_init******\n");

    gap_device_name = bt_get_local_name();
    gap_device_name_len = strlen(gap_device_name);
    if (gap_device_name_len > BT_NAME_LEN_MAX) {
        gap_device_name_len = BT_NAME_LEN_MAX;
    }

    log_info("ble name(%d): %s \n", gap_device_name_len, gap_device_name);

    set_ble_work_state(BLE_ST_INIT_OK);
    bt_ble_init_do();

#if (!MUTIl_CHARGING_BOX_EN)
    bt_ble_rcsp_adv_enable();
    ble_module_enable(1);
#else
    ble_module_enable(1);
#if(TCFG_CHARGE_BOX_ENABLE)
    if (get_chgbox_lid_status()) {
        bt_ble_rcsp_adv_enable();
    }
#endif

#endif

#if TEST_SEND_DATA_RATE
    server_timer_start();
#endif
}

void bt_ble_exit(void)
{
    log_info("***** ble_exit******\n");
    ble_module_enable(0);

#if TEST_SEND_DATA_RATE
    server_timer_stop();
#endif

}


void ble_app_disconnect(void)
{
    ble_disconnect(NULL);
}

static const struct ble_server_operation_t mi_ble_operation = {
    .adv_enable = set_adv_enable,
    .disconnect = ble_disconnect,
    .get_buffer_vaild = get_buffer_vaild_len,
    .send_data = (void *)app_send_user_data_do,
    .regist_wakeup_send = regiest_wakeup_send,
    .regist_recieve_cbk = regiest_recieve_cbk,
    .regist_state_cbk = regiest_state_cbk,
};


void ble_get_server_operation_table(struct ble_server_operation_t **interface_pt)
{
    *interface_pt = (void *)&mi_ble_operation;
}

extern bool get_remote_test_flag();
void ble_server_send_test_key_num(u8 key_num)
{
    if (con_handle) {
        if (get_remote_test_flag()) {
            ble_user_cmd_prepare(BLE_CMD_SEND_TEST_KEY_NUM, 2, con_handle, key_num);
        } else {
            log_info("-not conn testbox\n");
        }
    }
}

bool ble_msg_deal(u32 param)
{
    struct ble_server_operation_t *test_opt;
    ble_get_server_operation_table(&test_opt);

#if 0//for test key
    switch (param) {
    case MSG_BT_PP:
        break;

    case MSG_BT_NEXT_FILE:
        break;

    case MSG_HALF_SECOND:
        /* putchar('H'); */
        break;

    default:
        break;
    }
#endif

    return FALSE;
}

void input_key_handler(u8 key_status, u8 key_number)
{
}

void bt_adv_get_bat(u8 *buf)
{
    buf[0] = __this->bat_percent_L ? (((!!__this->bat_charge_L) << 7) | (__this->bat_percent_L & 0x7F)) : 0;
    buf[1] = __this->bat_percent_R ? (((!!__this->bat_charge_R) << 7) | (__this->bat_percent_R & 0x7F)) : 0;
    buf[2] = __this->bat_percent_C ? (((!!__this->bat_charge_C) << 7) | (__this->bat_percent_C & 0x7F)) : 0;
}

#if (JL_EARPHONE_APP_EN)
extern u8 jl_app_init_setting(void);
u8 btstck_app_init_setting(void)
{
    jl_app_init_setting();
    return 1;
}
#endif

#define TWS_FUNC_ID_ADV_STATE_INFO_SYNC	(('A' << (3 * 8)) | ('S' << (2 * 8)) | ('S' << (1 * 8)) | ('Y'))
static void deal_sibling_adv_state_info_sync(void *data, u16 len, bool rx)
{
    if (rx) {
        JL_rcsp_event_to_user(DEVICE_EVENT_FROM_RCSP, MSG_JL_UPDAET_ADV_STATE_INFO, data, len);
    }
}
REGISTER_TWS_FUNC_STUB(adv_state_sync) = {
    .func_id = TWS_FUNC_ID_ADV_STATE_INFO_SYNC,
    .func    = deal_sibling_adv_state_info_sync,
};
static void adv_state_info_sync(void)
{
#if TCFG_USER_TWS_ENABLE
    if (get_bt_tws_connect_status()) {
        u8 param[2] = {0};
        param[0] = JL_rcsp_get_auth_flag();
        param[1] = __this->ble_adv_notify;
        tws_api_send_data_to_sibling(param, sizeof(param), TWS_FUNC_ID_ADV_STATE_INFO_SYNC);
    }
#endif
}

// type : 0 - 正常关机检查		// 1 - 低电关机检测
u8 adv_tws_both_in_charge_box(u8 type)
{
    if (0 == ((!!__this->bat_percent_L) & (!!__this->bat_percent_R))) {
        return 0;
    }
    static u8 ret = 0;
    if (ret) {
        goto check_changes;
    }

    if (type) {
#if TCFG_USER_TWS_ENABLE
        if (TWS_ROLE_MASTER == tws_api_get_role()) {
            ret = 1;
        }
#endif
        return ret;
    }

    ret = ((!!__this->bat_charge_L) ^ (!!__this->bat_charge_R));
check_changes:
    if (con_handle) {
        return ret;
    }
    if (get_rcsp_connect_status() && ret) {
        adv_state_info_sync();
    }
    return 0;
}

extern u8 check_le_pakcet_sent_finish_flag(void);
extern bool rcsp_send_list_is_empty(void);
static void adv_master_to_salve_disconn_ble(void *priv)
{
    if (rcsp_send_list_is_empty() && check_le_pakcet_sent_finish_flag()) {
        ble_module_enable(0);
    } else {
        sys_timeout_add(NULL, adv_master_to_salve_disconn_ble, 50);
    }
}

// 切换后触发
void adv_role_switch_handle(void)
{
    u8 adv_cmd = 0;
#if TCFG_USER_TWS_ENABLE
    if (tws_api_get_tws_state()) {
        if (RCSP_BLE == JL_get_cur_bt_channel_sel()) {
            if (tws_api_get_role() == TWS_ROLE_MASTER) {
                ble_module_enable(1);
            } else {
                adv_cmd = 0x3;
                extern u8 adv_info_device_request(u8 * buf, u16 len);
                adv_info_device_request(&adv_cmd, sizeof(adv_cmd));
                adv_master_to_salve_disconn_ble(NULL);
            }
        } else if (RCSP_SPP == JL_get_cur_bt_channel_sel()) {
            adv_cmd = 0x4;
            adv_info_device_request(&adv_cmd, sizeof(adv_cmd));             //让手机来请求固件信息
        }
    }
#endif
}

void send_version_to_sibling(void)
{
    u8 data[3] = {0};
    u16 ver = 0;
    ver =  get_vid_pid_ver_from_cfg_file(GET_VER_FROM_EX_CFG);
    printf("%s---verson:%x\n", ver);
    data[0] = TWS_VERSON_INFO;
    data[1] = ver;
    data[2] = ver >> 8;
    tws_api_send_data_to_sibling(data, sizeof(data), TWS_FUNC_ID_SEQ_RAND_SYNC);
}

// int custom_app_send_user_data(u8 *data, u16 len)
// {
//     if (!ATT_CHARACTERISTIC_ae02_01_VALUE_HANDLE) {
//         return -1;
//     }
//     return app_send_user_data(ATT_CHARACTERISTIC_ae02_01_VALUE_HANDLE, data, len, ATT_OP_AUTO_READ_CCC);
// }
#endif

