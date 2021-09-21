/**
 * @author Jared Wolff (hello@jaredwolff.com)
 * @copyright Copyright Circuit Dojo LLC 2021
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr.h>
#include <init.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include <app_ble.h>
#include <services/aqw.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(app_ble_aqw_service);

static void app_ble_aqw_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    ARG_UNUSED(attr);

    bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

    LOG_INF("Air Quality Wing service notifications %s", notif_enabled ? "enabled" : "disabled");
}

/* Make sure this is updated to match BT_GATT_SERVICE_DEFINE below */
enum app_ble_aqw_char_position
{
    AQW_TEMP_ATTR_POS = 2,
    AQW_HUMIDITY_ATTR_POS = 5,
    AQW_VOC_ATTR_POS = 8,
    AQW_PM25_ATTR_POS = 11,
};

/* Air Quality Wing Service Declaration */
BT_GATT_SERVICE_DEFINE(aqw_service,
                       BT_GATT_PRIMARY_SERVICE(BT_UUID_AQW_SERVICE),
                       BT_GATT_CHARACTERISTIC(BT_UUID_AQW_TEMPERATURE, BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_NONE, NULL, NULL, NULL),
                       BT_GATT_CCC(app_ble_aqw_ccc_cfg_changed,
                                   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
                       BT_GATT_CHARACTERISTIC(BT_UUID_AQW_HUMIDITY, BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_NONE, NULL, NULL, NULL),
                       BT_GATT_CCC(app_ble_aqw_ccc_cfg_changed,
                                   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
                       BT_GATT_CHARACTERISTIC(BT_UUID_AQW_VOC, BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_NONE, NULL, NULL, NULL),
                       BT_GATT_CCC(app_ble_aqw_ccc_cfg_changed,
                                   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
                       BT_GATT_CHARACTERISTIC(BT_UUID_AQW_PM25, BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_NONE, NULL, NULL, NULL),
                       BT_GATT_CCC(app_ble_aqw_ccc_cfg_changed,
                                   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE), );

int app_ble_aqw_publish(struct bt_conn *conn, const struct app_ble_payload *data)
{
    struct bt_gatt_notify_params params = {0};
    const struct bt_gatt_attr *attr = NULL;

    switch (data->type)
    {
    case AQW_TEMPERATURE_SENSOR:
        attr = &aqw_service.attrs[AQW_TEMP_ATTR_POS];
        break;
    case AQW_HUMIDITY_SENSOR:
        attr = &aqw_service.attrs[AQW_HUMIDITY_ATTR_POS];
        break;
    case AQW_VOC_SENSOR:
        attr = &aqw_service.attrs[AQW_VOC_ATTR_POS];
        break;
    case AQW_PM25_SENSOR:
        attr = &aqw_service.attrs[AQW_PM25_ATTR_POS];
        break;
    default:
        return -EINVAL;
        break;
    }

    params.attr = attr;
    params.data = &data->value;
    params.len = sizeof(data->value);
    params.func = NULL;

    if (!conn)
    {
        LOG_DBG("Notification send to all connected peers");
        return bt_gatt_notify_cb(NULL, &params);
    }
    else if (bt_gatt_is_subscribed(conn, attr, BT_GATT_CCC_NOTIFY))
    {
        return bt_gatt_notify_cb(conn, &params);
    }
    else
    {
        return -EINVAL;
    }
}