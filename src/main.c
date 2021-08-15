#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>
#include <devicetree.h>
#include <aqw.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(demo);

/* Device name defintions*/
#define SHTC3 DT_INST(0, sensirion_shtc3)
#define CONFIG_SHTC3_DEV_NAME DT_LABEL(SHTC3)

#define SGP40 DT_INST(0, sensirion_sgp40)

//TODO: PM2.5 sensor

static struct aqw_sensor temperature_sensor =
    {
        .type = AQW_TEMPERATURE_SENSOR,
        .chan = SENSOR_CHAN_AMBIENT_TEMP,
        .dev_name = CONFIG_SHTC3_DEV_NAME,
};

static struct aqw_sensor humidity_sensor =
    {
        .type = AQW_HUMIDITY_SENSOR,
        .chan = SENSOR_CHAN_HUMIDITY,
        .dev_name = CONFIG_SHTC3_DEV_NAME,
};

static struct aqw_sensor *sensors[] = {
    &temperature_sensor,
    &humidity_sensor,
};

void sensor_cb(struct aqw_sensor_data *data, size_t len)
{
    for (int i = 0; i < len; i++)
    {
        LOG_INF("%s: %i.%i", aqw_sensor_type_to_string(data[i].type), data[i].val.val1, data[i].val.val2);
    }
}

void main(void)
{
    int err = 0;

    LOG_INF("Air Quality Wing Demo");

    /* Init Air Quality Wing */
    err = aqw_init(sensors, ARRAY_SIZE(sensors), sensor_cb);
    if (err)
    {
        __ASSERT_MSG_INFO("Unable to init Air Quality Wing library. Err: %i", err);
    }

    for (;;)
    {
        err = aqw_sensor_start_fetch();
        if (err)
            LOG_WRN("Unable to start fetch. Err: %i", err);

        k_sleep(K_MSEC(10000));
    }
}