/**
 * @author Jared Wolff (hello@jaredwolff.com)
 * @copyright Copyright Circuit Dojo LLC 2021
 */

#include <stdio.h>

#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>
#include <devicetree.h>
#include <aqw.h>

#include <modem/lte_lc.h>
#include <modem/nrf_modem_lib.h>
#include <modem/at_cmd.h>
#include <modem/at_notif.h>
#include <modem/modem_info.h>
#include <nrf_modem.h>
#include <date_time.h>
#include <power/reboot.h>

#include <net/coap.h>
#include <net/golioth/system_client.h>

#include <app_codec.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(aqw_golioth_demo);

/* Used to prevent the app from running before connecting */
K_SEM_DEFINE(lte_connected, 0, 1);

static struct golioth_client *client = GOLIOTH_SYSTEM_CLIENT_GET();

/* Device name defintions*/
#define SHTC3 DT_INST(0, sensirion_shtc3cd)
#define CONFIG_SHTC3_DEV_NAME DT_LABEL(SHTC3)

#define SGP40 DT_INST(0, sensirion_sgp40cd)
#define CONFIG_SGP40_DEV_NAME DT_LABEL(SGP40)

#define HPMA115S0 DT_INST(0, honeywell_hpma115s0)
#define CONFIG_HPMA115S0_DEV_NAME DT_LABEL(HPMA115S0)

/* Inverval in seconds */
#define STANDARD_SENSOR_INTERVAL 600

static struct aqw_sensor temperature_sensor =
    {
        .type = AQW_TEMPERATURE_SENSOR,
        .chan = SENSOR_CHAN_AMBIENT_TEMP,
        .dev_name = CONFIG_SHTC3_DEV_NAME,
        .interval = STANDARD_SENSOR_INTERVAL,
};

static struct aqw_sensor humidity_sensor =
    {
        .type = AQW_HUMIDITY_SENSOR,
        .chan = SENSOR_CHAN_HUMIDITY,
        .dev_name = CONFIG_SHTC3_DEV_NAME,
        .interval = STANDARD_SENSOR_INTERVAL,
};

static struct aqw_sensor voc_sensor =
    {
        .type = AQW_VOC_SENSOR,
        .chan = SENSOR_CHAN_VOC,
        .dev_name = CONFIG_SGP40_DEV_NAME,
        .interval = STANDARD_SENSOR_INTERVAL,
};

static struct aqw_sensor hpma_sensor =
    {
        .type = AQW_PM25_SENSOR,
        .chan = SENSOR_CHAN_PM_2_5,
        .dev_name = CONFIG_HPMA115S0_DEV_NAME,
        .interval = STANDARD_SENSOR_INTERVAL,
};

static struct aqw_sensor *sensors[] = {
    &temperature_sensor,
    &humidity_sensor,
    &voc_sensor,
    &hpma_sensor,
};

void sensor_cb(struct aqw_sensor_data *data, size_t len)
{
    int err = 0;
    uint8_t buf[256];
    size_t size = 0;

    for (int i = 0; i < len; i++)
    {

        /* Skip if not valid */
        if (data[i].type == AQW_INVALID_SENSOR)
            continue;

        LOG_INF("%s: %i.%i%s", aqw_sensor_type_to_string(data[i].type), data[i].val.val1, data[i].val.val2, aqw_sensor_unit_to_string(data[i].type));

        /* Get the current time */
        err = date_time_now(&data[i].ts);
        if (err)
        {
            LOG_WRN("Unable to get timestamp!");
        }
    }

    struct app_codec_payload payload = {
        .data = data,
        .sensor_count = len,
    };

    /* Encode CBOR data */
    err = app_codec_aqw_data_encode(&payload, buf, sizeof(buf), &size);
    if (err < 0)
    {
        LOG_ERR("Unable to encode data. Err: %i", err);
        return;
    }

    LOG_INF("Data size: %i", size);

    /* Publish gps data */
    err = golioth_lightdb_set(client,
                              GOLIOTH_LIGHTDB_STREAM_PATH("env"),
                              COAP_CONTENT_FORMAT_APP_CBOR,
                              buf, size);
    if (err)
    {
        LOG_WRN("Failed to gps data: %d", err);
    }
}

static void golioth_on_message(struct golioth_client *client,
                               struct coap_packet *rx)
{
    uint16_t payload_len;
    const uint8_t *payload;
    uint8_t type;

    type = coap_header_get_type(rx);
    payload = coap_packet_get_payload(rx, &payload_len);

    if (!IS_ENABLED(CONFIG_LOG_BACKEND_GOLIOTH) && payload)
    {
        LOG_HEXDUMP_DBG(payload, payload_len, "Payload");
    }
}

static void lte_handler(const struct lte_lc_evt *const evt)
{
    switch (evt->type)
    {
    case LTE_LC_EVT_NW_REG_STATUS:
        if ((evt->nw_reg_status != LTE_LC_NW_REG_REGISTERED_HOME) &&
            (evt->nw_reg_status != LTE_LC_NW_REG_REGISTERED_ROAMING))
        {
            /*  TODO: Send disconnected event */

            break;
        }

        /* TODO: Otherwise send connected event */

        LOG_INF("Network registration status: %s",
                evt->nw_reg_status == LTE_LC_NW_REG_REGISTERED_HOME ? "Connected - home network" : "Connected - roaming");

        k_sem_give(&lte_connected);
        break;
    case LTE_LC_EVT_PSM_UPDATE:
        LOG_INF("PSM parameter update: TAU: %d, Active time: %d",
                evt->psm_cfg.tau, evt->psm_cfg.active_time);
        break;
    case LTE_LC_EVT_EDRX_UPDATE:
    {
        char log_buf[60];
        ssize_t len;

        len = snprintf(log_buf, sizeof(log_buf),
                       "eDRX parameter update: eDRX: %f, PTW: %f",
                       evt->edrx_cfg.edrx, evt->edrx_cfg.ptw);
        if (len > 0)
        {
            LOG_INF("%s", log_buf);
        }
        break;
    }
    case LTE_LC_EVT_RRC_UPDATE:
        LOG_INF("RRC mode: %s",
                evt->rrc_mode == LTE_LC_RRC_MODE_CONNECTED ? "Connected" : "Idle");
        break;
    case LTE_LC_EVT_CELL_UPDATE:
        LOG_INF("LTE cell changed: Cell ID: %d, Tracking area: %d",
                evt->cell.id, evt->cell.tac);
        break;
    default:
        break;
    }
}

static void nrf_modem_lib_dfu_handler(void)
{
    int err;

    err = nrf_modem_lib_init(NORMAL_MODE);

    switch (err)
    {
    case MODEM_DFU_RESULT_OK:
        LOG_INF("Modem update suceeded, reboot");
        sys_reboot(SYS_REBOOT_COLD);
        break;
    case MODEM_DFU_RESULT_UUID_ERROR:
    case MODEM_DFU_RESULT_AUTH_ERROR:
        LOG_ERR("Modem update failed, error: %d", err);
        LOG_ERR("Modem will use old firmware");
        sys_reboot(SYS_REBOOT_COLD);
        break;
    case MODEM_DFU_RESULT_HARDWARE_ERROR:
    case MODEM_DFU_RESULT_INTERNAL_ERROR:
        LOG_ERR("Modem update malfunction, error: %d, reboot", err);
        sys_reboot(SYS_REBOOT_COLD);
        break;
    default:
        break;
    }
}

void date_time_evt(const struct date_time_evt *evt)
{

    /*Setup and connect to Golioth once we have the time*/
    client->on_message = golioth_on_message;
    golioth_system_client_start();
}

static int modem_init()
{

    int err = 0;

    nrf_modem_lib_dfu_handler();

    if (IS_ENABLED(CONFIG_LTE_AUTO_INIT_AND_CONNECT))
    {
        /* Do nothing, modem is already configured and LTE connected. */
    }
    else
    {
        LOG_INF("Connecting..");

        err = lte_lc_init_and_connect_async(lte_handler);
        if (err)
        {
            LOG_ERR("Modem could not be configured, error: %d",
                    err);
            return err;
        }
    }

    err = modem_info_init();
    if (err)
    {
        LOG_ERR("Failed initializing modem info module, error: %d",
                err);
        return err;
    }

    k_sem_take(&lte_connected, K_FOREVER);

    /* Force time update */
    err = date_time_update_async(date_time_evt);
    if (err)
    {
        LOG_ERR("Unable to update time with date_time_update_async. Err: %i", err);
    }

    return 0;
}

void main(void)
{
    int err = 0;

    /* Init modem first */
    err = modem_init();
    if (err)
        __ASSERT_MSG_INFO("Error initializing Golioth. Err: %i", err);

    LOG_INF("Air Quality Wing Golioth Demo");

    /* Init Air Quality Wing */
    err = aqw_init(sensors, ARRAY_SIZE(sensors), sensor_cb);
    if (err)
        __ASSERT_MSG_INFO("Unable to init Air Quality Wing library. Err: %i", err);

    err = aqw_sensor_start_fetch();
    if (err)
        __ASSERT_MSG_INFO("Unable to start fetch. Err: %i", err);
}