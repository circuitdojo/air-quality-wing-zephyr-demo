/**
 * @author Jared Wolff (hello@jaredwolff.com)
 * @copyright Copyright Circuit Dojo LLC 2022
 */

#ifndef _AQW_CLIENT_H
#define _AQW_CLIENT_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/bluetooth/gatt.h>

#include <aqw.h>
#include <app_ble.h>

typedef void (*aqw_client_cb)(struct aqw_sensor_data *payload);

struct aqw_client_handles
{
    uint16_t temperature;
    uint16_t temperature_ccc;
    uint16_t humidity;
    uint16_t humidity_ccc;
    uint16_t voc;
    uint16_t voc_ccc;
    uint16_t pm25;
    uint16_t pm25_ccc;
};

struct aqw_ble_client
{
    /** Connection object. */
    struct bt_conn *conn;

    /** Handles for subscribing to data */
    struct aqw_client_handles handles;

    /** GATT subscribe parameters for characteristics. */
    struct bt_gatt_subscribe_params temperature_notif_params, humidity_notif_params, voc_notif_params, pm25_notif_params;

    /** Application callbacks. */
    aqw_client_cb cb;
};

int aqw_ble_client_init(struct aqw_ble_client *p_client, aqw_client_cb _cb);

int aqw_ble_client_handles_assign(struct aqw_ble_client *p_client, struct bt_gatt_dm *dm);

int aqw_ble_client_subscribe(struct aqw_ble_client *p_client);

#endif