/**
 * @author Jared Wolff (hello@jaredwolff.com)
 * @copyright Copyright Circuit Dojo LLC 2021
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/devicetree.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(aqw_ble_demo);

#include <aqw.h>
#include <app_ble.h>

/* Inverval in seconds */
#define STANDARD_SENSOR_INTERVAL 60

static struct aqw_sensor temperature_sensor =
    {
        .type = AQW_TEMPERATURE_SENSOR,
        .chan = SENSOR_CHAN_AMBIENT_TEMP,
        .dev = DEVICE_DT_GET_ONE(sensirion_shtc3cd),
        .interval = STANDARD_SENSOR_INTERVAL,
};

static struct aqw_sensor humidity_sensor =
    {
        .type = AQW_HUMIDITY_SENSOR,
        .chan = SENSOR_CHAN_HUMIDITY,
        .dev = DEVICE_DT_GET_ONE(sensirion_shtc3cd),
        .interval = STANDARD_SENSOR_INTERVAL,
};

static struct aqw_sensor voc_sensor =
    {
        .type = AQW_VOC_SENSOR,
        .chan = SENSOR_CHAN_VOC,
        .dev = DEVICE_DT_GET_ONE(sensirion_sgp40cd),
        .interval = STANDARD_SENSOR_INTERVAL,
};

static struct aqw_sensor hpma_sensor =
    {
        .type = AQW_PM25_SENSOR,
        .chan = SENSOR_CHAN_PM_2_5,
        .dev = DEVICE_DT_GET_ONE(honeywell_hpma115s0),
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
    for (int i = 0; i < len; i++)
    {

        /* Skip if not valid */
        if (data[i].type == AQW_INVALID_SENSOR)
            continue;

        LOG_INF("%s: %i.%i%s", aqw_sensor_type_to_string(data[i].type), data[i].val.val1, data[i].val.val2, aqw_sensor_unit_to_string(data[i].type));

        /* Data */
        struct app_ble_payload payload = {
            .value = data[i].val,
            .ts = k_uptime_ticks(),
            .type = data[i].type,
        };

        /* Attempt to publish no matter what */
        app_ble_publish_sensor_data(&payload);

        /* TODO: storing previous values to storage? */
    }
}

void main(void)
{
    int err = 0;

    LOG_INF("Air Quality Wing Demo");

    /* Setup BLE */
    err = app_ble_init();
    if (err)
        __ASSERT_MSG_INFO("Unable to init bluetooth library. Err: %i", err);

    /* Init Air Quality Wing */
    err = aqw_init(sensors, ARRAY_SIZE(sensors), sensor_cb);
    if (err)
        __ASSERT_MSG_INFO("Unable to init Air Quality Wing library. Err: %i", err);

    err = aqw_sensor_start_fetch();
    if (err)
        __ASSERT_MSG_INFO("Unable to start fetch. Err: %i", err);
}