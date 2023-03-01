/**
 * @author Jared Wolff (hello@jaredwolff.com)
 * @copyright Copyright Circuit Dojo LLC 2021
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/display.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(aqw_basic_with_display_demo);

#include <lvgl.h>
#include <aqw.h>

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

static lv_obj_t *temp_label, *temp_value_label;
static lv_obj_t *hum_label, *hum_value_label;
static lv_obj_t *pm25_label, *pm25_value_label;
static lv_obj_t *voc_label, *voc_value_label;

char temp_value[11] = {0};
char hum_value[11] = {0};
char pm25_value[11] = {0};
char voc_value[11] = {0};

static uint8_t get_two_digits(int32_t value)
{
    int32_t new_val = value;

    while (new_val > 100)
    {
        new_val /= 10;
    }

    return new_val;
}

static int display_update(struct aqw_sensor_data *data)
{

    switch (data->type)
    {
    case AQW_TEMPERATURE_SENSOR:

        snprintf(temp_value, sizeof(temp_value) - 1, "%d.%d", data->val.val1, get_two_digits(data->val.val2));
        lv_label_set_text(temp_value_label, temp_value);

        break;
    case AQW_HUMIDITY_SENSOR:

        snprintf(hum_value, sizeof(hum_value) - 1, "%d.%d%%", data->val.val1, get_two_digits(data->val.val2));
        lv_label_set_text(hum_value_label, hum_value);
        break;
    case AQW_PM25_SENSOR:

        snprintf(pm25_value, sizeof(pm25_value) - 1, "%d.%d", data->val.val1, get_two_digits(data->val.val2));
        lv_label_set_text(pm25_value_label, pm25_value);
        break;
    case AQW_VOC_SENSOR:

        snprintf(voc_value, sizeof(voc_value) - 1, "%d.%d", data->val.val1, get_two_digits(data->val.val2));
        lv_label_set_text(voc_value_label, voc_value);
        break;

    default:
        break;
    }

    return 0;
}

static int display_init(void)
{

    const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev))
    {
        LOG_ERR("Display not ready!");
        return -EIO;
    }

    temp_label = lv_label_create(lv_scr_act());
    lv_label_set_text(temp_label, "T: (C)");
    lv_obj_align(temp_label, LV_ALIGN_TOP_MID, 0, 0);

    temp_value_label = lv_label_create(lv_scr_act());
    lv_label_set_text(temp_value_label, "*");
    lv_obj_align(temp_value_label, LV_ALIGN_TOP_LEFT, 0, 14);

    hum_label = lv_label_create(lv_scr_act());
    lv_label_set_text(hum_label, "H: (%)");
    lv_obj_align(hum_label, LV_ALIGN_TOP_MID, 0, 32);

    hum_value_label = lv_label_create(lv_scr_act());
    lv_label_set_text(hum_value_label, "*");
    lv_obj_align(hum_value_label, LV_ALIGN_TOP_LEFT, 0, 46);

    voc_label = lv_label_create(lv_scr_act());
    lv_label_set_text(voc_label, "VOC:");
    lv_obj_align(voc_label, LV_ALIGN_TOP_MID, 0, 66);

    voc_value_label = lv_label_create(lv_scr_act());
    lv_label_set_text(voc_value_label, "*");
    lv_obj_align(voc_value_label, LV_ALIGN_TOP_LEFT, 0, 80);

    pm25_label = lv_label_create(lv_scr_act());
    lv_label_set_text(pm25_label, "PM25:");
    lv_obj_align(pm25_label, LV_ALIGN_TOP_MID, 0, 94);

    pm25_value_label = lv_label_create(lv_scr_act());
    lv_label_set_text(pm25_value_label, "*");
    lv_obj_align(pm25_value_label, LV_ALIGN_TOP_LEFT, 0, 108);

    lv_task_handler();

    return 0;
}

void sensor_cb(struct aqw_sensor_data *data, size_t len)
{

    for (int i = 0; i < len; i++)
    {

        /* Skip if not valid */
        if (data[i].type == AQW_INVALID_SENSOR)
            continue;

        LOG_INF("%s: %i.%i%s", aqw_sensor_type_to_string(data[i].type), data[i].val.val1, data[i].val.val2, aqw_sensor_unit_to_string(data[i].type));

        /* Update value in display */
        display_update(&data[i]);
    }

    lv_task_handler();
}

void main(void)
{
    int err = 0;

    LOG_INF("Air Quality Wing Demo");

    /* Get display */
    err = display_init();
    if (err)
        __ASSERT_MSG_INFO("Unable to init display library. Err: %i", err);

    /* Init Air Quality Wing */
    err = aqw_init(sensors, ARRAY_SIZE(sensors), sensor_cb);
    if (err)
        __ASSERT_MSG_INFO("Unable to init Air Quality Wing library. Err: %i", err);

    err = aqw_sensor_start_fetch();
    if (err)
        __ASSERT_MSG_INFO("Unable to start fetch. Err: %i", err);
}