/**
 * @author Jared Wolff (hello@jaredwolff.com)
 * @copyright Copyright Circuit Dojo LLC 2021
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <devicetree.h>
#include <aqw.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(aqw_basic_demo);

/* Device name defintions*/
#define SHTC3 DT_INST(0, sensirion_shtc3cd)
#define CONFIG_SHTC3_DEV_NAME DT_LABEL(SHTC3)

#define SGP40 DT_INST(0, sensirion_sgp40cd)
#define CONFIG_SGP40_DEV_NAME DT_LABEL(SGP40)

#define HPMA115S0 DT_INST(0, honeywell_hpma115s0)
#define CONFIG_HPMA115S0_DEV_NAME DT_LABEL(HPMA115S0)

/* Inverval in seconds */
#define STANDARD_SENSOR_INTERVAL 60

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

static const struct device *display;
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

    display = device_get_binding(DT_LABEL(DT_INST(0, solomon_ssd1306fb)));
    if (display == NULL)
    {
        LOG_ERR("SSD16XX device not found");
        return -ENODEV;
    }

    temp_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(temp_label, "T: (C)");
    lv_obj_align(temp_label, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

    temp_value_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(temp_value_label, "*");
    lv_obj_align(temp_value_label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 10);

    hum_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(hum_label, "H: (%)");
    lv_obj_align(hum_label, NULL, LV_ALIGN_IN_TOP_MID, 0, 36);

    hum_value_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(hum_value_label, "*");
    lv_obj_align(hum_value_label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 46);

    voc_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(voc_label, "VOC:");
    lv_obj_align(voc_label, NULL, LV_ALIGN_IN_TOP_MID, 0, 72);

    voc_value_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(voc_value_label, "*");
    lv_obj_align(voc_value_label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 82);

    pm25_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(pm25_label, "PM25:");
    lv_obj_align(pm25_label, NULL, LV_ALIGN_IN_TOP_MID, 0, 108);

    pm25_value_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(pm25_value_label, "*");
    lv_obj_align(pm25_value_label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 118);

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
    display_init();

    /* Init Air Quality Wing */
    err = aqw_init(sensors, ARRAY_SIZE(sensors), sensor_cb);
    if (err)
        __ASSERT_MSG_INFO("Unable to init Air Quality Wing library. Err: %i", err);

    err = aqw_sensor_start_fetch();
    if (err)
        __ASSERT_MSG_INFO("Unable to start fetch. Err: %i", err);
}