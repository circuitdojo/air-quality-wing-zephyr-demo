/**
 * @author Jared Wolff (hello@jaredwolff.com)
 * @copyright Copyright Circuit Dojo LLC 2022
 */

#include <zephyr.h>

#include <bluetooth/gatt.h>
#include <bluetooth/gatt_dm.h>

#include <aqw.h>
#include <app_ble/services/aqw.h>
#include <app_ble/services/aqw_c.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(app_ble_aqw_c);

static uint8_t on_temperature_received(struct bt_conn *conn,
                                       struct bt_gatt_subscribe_params *params,
                                       const void *data, uint16_t length)
{
    struct aqw_ble_client *p_client;

    /* Retrieve client module context. */
    p_client = CONTAINER_OF(params, struct aqw_ble_client, temperature_notif_params);

    if (!data)
    {
        LOG_DBG("Temperature unsubscribed");
        params->value_handle = 0;
        return BT_GATT_ITER_STOP;
    }

    LOG_DBG("Temperature data %p length %u", data, length);
    if (p_client->cb && length == sizeof(struct sensor_value))
    {
        struct aqw_sensor_data payload = {
            .type = AQW_TEMPERATURE_SENSOR,
        };
        memcpy(&payload.val, data, sizeof(struct sensor_value));
        p_client->cb(&payload);
    }

    return BT_GATT_ITER_CONTINUE;
}

static uint8_t on_humidity_received(struct bt_conn *conn,
                                    struct bt_gatt_subscribe_params *params,
                                    const void *data, uint16_t length)
{
    struct aqw_ble_client *p_client;

    /* Retrieve client module context. */
    p_client = CONTAINER_OF(params, struct aqw_ble_client, humidity_notif_params);

    if (!data)
    {
        LOG_DBG("Humidity unsubscribed");
        params->value_handle = 0;
        return BT_GATT_ITER_STOP;
    }

    LOG_DBG("Humidity data %p length %u", data, length);
    if (p_client->cb && length == sizeof(struct sensor_value))
    {
        struct aqw_sensor_data payload = {
            .type = AQW_HUMIDITY_SENSOR,
        };
        memcpy(&payload.val, data, sizeof(struct sensor_value));
        p_client->cb(&payload);
    }

    return BT_GATT_ITER_CONTINUE;
}

static uint8_t on_voc_received(struct bt_conn *conn,
                               struct bt_gatt_subscribe_params *params,
                               const void *data, uint16_t length)
{
    struct aqw_ble_client *p_client;

    /* Retrieve client module context. */
    p_client = CONTAINER_OF(params, struct aqw_ble_client, voc_notif_params);

    if (!data)
    {
        LOG_DBG("VOC unsubscribed");
        params->value_handle = 0;
        return BT_GATT_ITER_STOP;
    }

    LOG_DBG("VOC data %p length %u", data, length);
    if (p_client->cb && length == sizeof(struct sensor_value))
    {
        struct aqw_sensor_data payload = {
            .type = AQW_VOC_SENSOR,
        };
        memcpy(&payload.val, data, sizeof(struct sensor_value));
        p_client->cb(&payload);
    }

    return BT_GATT_ITER_CONTINUE;
}

static uint8_t on_pm25_received(struct bt_conn *conn,
                                struct bt_gatt_subscribe_params *params,
                                const void *data, uint16_t length)
{
    struct aqw_ble_client *p_client;

    /* Retrieve client module context. */
    p_client = CONTAINER_OF(params, struct aqw_ble_client, pm25_notif_params);

    if (!data)
    {
        LOG_DBG("PM2.5 unsubscribed");
        params->value_handle = 0;
        return BT_GATT_ITER_STOP;
    }

    LOG_DBG("PM2.5 data %p length %u", data, length);
    if (p_client->cb && length == sizeof(struct sensor_value))
    {
        struct aqw_sensor_data payload = {
            .type = AQW_PM25_SENSOR,
        };
        memcpy(&payload.val, data, sizeof(struct sensor_value));
        p_client->cb(&payload);
    }

    return BT_GATT_ITER_CONTINUE;
}

int aqw_ble_client_init(struct aqw_ble_client *p_client, aqw_client_cb _cb)
{
    if (p_client == NULL || _cb == NULL)
    {
        return -EINVAL;
    }

    p_client->cb = _cb;

    return 0;
}

static int assign_characteristic_handles(struct aqw_ble_client *p_client, struct bt_gatt_dm *dm, struct bt_uuid *uuid)
{

    const struct bt_gatt_dm_attr *gatt_chrc;
    const struct bt_gatt_dm_attr *gatt_desc;

    /* Data Characteristic */
    gatt_chrc = bt_gatt_dm_char_by_uuid(dm, uuid);
    if (!gatt_chrc)
    {
        LOG_ERR("Missing data characteristic.");
        return -EINVAL;
    }

    /* Desc */
    gatt_desc = bt_gatt_dm_desc_by_uuid(dm, gatt_chrc, uuid);
    if (!gatt_desc)
    {
        LOG_ERR("Missing data value descriptor in characteristic.");
        return -EINVAL;
    }

    if (memcmp(uuid, BT_UUID_AQW_TEMPERATURE, sizeof(struct bt_uuid_128)) == 0)
    {
        p_client->handles.temperature = gatt_desc->handle;
    }
    else if (memcmp(uuid, BT_UUID_AQW_HUMIDITY, sizeof(struct bt_uuid_128)) == 0)
    {
        p_client->handles.humidity = gatt_desc->handle;
    }
    else if (memcmp(uuid, BT_UUID_AQW_VOC, sizeof(struct bt_uuid_128)) == 0)
    {
        p_client->handles.voc = gatt_desc->handle;
    }
    else if (memcmp(uuid, BT_UUID_AQW_PM25, sizeof(struct bt_uuid_128)) == 0)
    {
        p_client->handles.pm25 = gatt_desc->handle;
    }

    /* CCC */
    gatt_desc = bt_gatt_dm_desc_by_uuid(dm, gatt_chrc, BT_UUID_GATT_CCC);
    if (!gatt_desc)
    {
        LOG_ERR("Missing data CCC in characteristic.");
        return -EINVAL;
    }

    if (memcmp(uuid, BT_UUID_AQW_TEMPERATURE, sizeof(struct bt_uuid_128)) == 0)
    {
        p_client->handles.temperature_ccc = gatt_desc->handle;
    }
    else if (memcmp(uuid, BT_UUID_AQW_HUMIDITY, sizeof(struct bt_uuid_128)) == 0)
    {
        p_client->handles.humidity_ccc = gatt_desc->handle;
    }
    else if (memcmp(uuid, BT_UUID_AQW_VOC, sizeof(struct bt_uuid_128)) == 0)
    {
        p_client->handles.voc_ccc = gatt_desc->handle;
    }
    else if (memcmp(uuid, BT_UUID_AQW_PM25, sizeof(struct bt_uuid_128)) == 0)
    {
        p_client->handles.pm25_ccc = gatt_desc->handle;
    }

    return 0;
}

int aqw_ble_client_handles_assign(struct aqw_ble_client *p_client, struct bt_gatt_dm *dm)
{

    const struct bt_gatt_dm_attr *gatt_service_attr =
        bt_gatt_dm_service_get(dm);
    const struct bt_gatt_service_val *gatt_service =
        bt_gatt_dm_attr_service_val(gatt_service_attr);

    if (bt_uuid_cmp(gatt_service->uuid, BT_UUID_AQW_SERVICE))
    {
        return -ENOTSUP;
    }
    LOG_DBG("Getting handles from AQW service.");

    /* Assign handle for each */
    assign_characteristic_handles(p_client, dm, BT_UUID_AQW_TEMPERATURE);
    assign_characteristic_handles(p_client, dm, BT_UUID_AQW_HUMIDITY);
    assign_characteristic_handles(p_client, dm, BT_UUID_AQW_VOC);
    assign_characteristic_handles(p_client, dm, BT_UUID_AQW_PM25);

    /* Assign connection instance. */
    p_client->conn = bt_gatt_dm_conn_get(dm);
    return 0;
}

int aqw_ble_client_subscribe(struct aqw_ble_client *p_client)
{
    int err;

    p_client->temperature_notif_params.notify = on_temperature_received;
    p_client->temperature_notif_params.value = BT_GATT_CCC_NOTIFY;
    p_client->temperature_notif_params.value_handle = p_client->handles.temperature;
    p_client->temperature_notif_params.ccc_handle = p_client->handles.temperature_ccc;
    atomic_set_bit(p_client->temperature_notif_params.flags,
                   BT_GATT_SUBSCRIBE_FLAG_VOLATILE);

    p_client->humidity_notif_params.notify = on_humidity_received;
    p_client->humidity_notif_params.value = BT_GATT_CCC_NOTIFY;
    p_client->humidity_notif_params.value_handle = p_client->handles.humidity;
    p_client->humidity_notif_params.ccc_handle = p_client->handles.humidity_ccc;
    atomic_set_bit(p_client->humidity_notif_params.flags,
                   BT_GATT_SUBSCRIBE_FLAG_VOLATILE);

    p_client->voc_notif_params.notify = on_voc_received;
    p_client->voc_notif_params.value = BT_GATT_CCC_NOTIFY;
    p_client->voc_notif_params.value_handle = p_client->handles.voc;
    p_client->voc_notif_params.ccc_handle = p_client->handles.voc_ccc;
    atomic_set_bit(p_client->voc_notif_params.flags,
                   BT_GATT_SUBSCRIBE_FLAG_VOLATILE);

    p_client->pm25_notif_params.notify = on_pm25_received;
    p_client->pm25_notif_params.value = BT_GATT_CCC_NOTIFY;
    p_client->pm25_notif_params.value_handle = p_client->handles.pm25;
    p_client->pm25_notif_params.ccc_handle = p_client->handles.pm25_ccc;
    atomic_set_bit(p_client->pm25_notif_params.flags,
                   BT_GATT_SUBSCRIBE_FLAG_VOLATILE);

    err = bt_gatt_subscribe(p_client->conn, &p_client->temperature_notif_params);
    if (err)
    {
        LOG_ERR("Subscribe for temperature failed (err %d)", err);
    }

    err = bt_gatt_subscribe(p_client->conn, &p_client->humidity_notif_params);
    if (err)
    {
        LOG_ERR("Subscribe for humidity failed (err %d)", err);
    }

    err = bt_gatt_subscribe(p_client->conn, &p_client->voc_notif_params);
    if (err)
    {
        LOG_ERR("Subscribe for voc failed (err %d)", err);
    }

    err = bt_gatt_subscribe(p_client->conn, &p_client->pm25_notif_params);
    if (err)
    {
        LOG_ERR("Subscribe for pm2.5 failed (err %d)", err);
    }

    return err;
}