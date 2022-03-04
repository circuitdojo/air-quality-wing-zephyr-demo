/**
 * @author Jared Wolff (hello@jaredwolff.com)
 * @copyright Copyright Circuit Dojo LLC 2022
 */

#include <stdlib.h>
#include <zephyr.h>
#include <device.h>
#include <devicetree.h>

#include <date_time.h>
#include <net/golioth/system_client.h>

#include <codec/aqw.h>

#include <app_ble.h>
#include <app_event_manager.h>
#include <app_network.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(aqw_ethernet_hub_ble);

static struct golioth_client *client = GOLIOTH_SYSTEM_CLIENT_GET();

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

void date_time_evt(const struct date_time_evt *evt)
{

    /*Setup and connect to Golioth once we have the time*/
    client->on_message = golioth_on_message;
    golioth_system_client_start();
}

void publish(struct aqw_sensor_data *data)
{
    int err = 0;
    uint8_t buf[256];
    size_t size = 0;

    char *name = aqw_sensor_type_to_string(data->type);
    char *unit = aqw_sensor_unit_to_string(data->type);
    LOG_INF("%s: %i.%i%s", name, data->val.val1, abs(data->val.val2), unit);

    /* Get the current time */
    err = date_time_now(&data->ts);
    if (err)
    {
        LOG_WRN("Unable to get timestamp!");
    }

    struct app_codec_payload payload = {
        .data = data,
        .sensor_count = 1,
    };

    /* Encode CBOR data */
    err = app_codec_aqw_data_encode(&payload, buf, sizeof(buf), &size);
    if (err < 0)
    {
        LOG_ERR("Unable to encode data. Err: %i", err);
        return;
    }

    LOG_DBG("Data size: %i", size);

    /* Publish gps data */
    err = golioth_lightdb_set(client,
                              GOLIOTH_LIGHTDB_STREAM_PATH("env"),
                              COAP_CONTENT_FORMAT_APP_CBOR,
                              buf, size);
    if (err)
    {
        LOG_WRN("Failed to send data: %d", err);
    }
}

void main(void)
{
    int err;

    LOG_INF("Air Quality Wing Ethernet Hub");

    /* Init network */
    app_net_init();

    /* Init Bluetooth Central */
    app_ble_init();

    while (true)
    {

        struct app_event evt = {0};
        app_event_manager_get(&evt);

        LOG_DBG("Incoming %s", app_event_type_to_string(evt.type));

        switch (evt.type)
        {
        case APP_EVENT_BLE_DATA:
            publish(&evt.data);

            break;
        case APP_EVENT_ETHERNET_READY:

            /* Force time update */
            err = date_time_update_async(date_time_evt);
            if (err)
            {
                LOG_ERR("Unable to update time with date_time_update_async. Err: %i", err);
            }

            break;
        default:
            break;
        }
    }
}