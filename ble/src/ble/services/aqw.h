/**
 * @author Jared Wolff (hello@jaredwolff.com)
 * @copyright Copyright Circuit Dojo LLC 2021
 */

#ifndef _AQW_SERVICE_H
#define _AQW_SERVICE_H

#include <app_ble.h>

#define BT_UUID_AQW_VAL \
    BT_UUID_128_ENCODE(0x13370001, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)

/* Characterisitcs for sensors */
#define BT_UUID_AQW_TEMPERATURE_VAL \
    BT_UUID_128_ENCODE(0x13370002, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)
#define BT_UUID_AQW_HUMIDITY_VAL \
    BT_UUID_128_ENCODE(0x13370003, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)
#define BT_UUID_AQW_VOC_VAL \
    BT_UUID_128_ENCODE(0x13370004, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)
#define BT_UUID_AQW_PM25_VAL \
    BT_UUID_128_ENCODE(0x13370005, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)

#define BT_UUID_AQW_SERVICE BT_UUID_DECLARE_128(BT_UUID_AQW_VAL)
#define BT_UUID_AQW_TEMPERATURE BT_UUID_DECLARE_128(BT_UUID_AQW_TEMPERATURE_VAL)
#define BT_UUID_AQW_HUMIDITY BT_UUID_DECLARE_128(BT_UUID_AQW_HUMIDITY_VAL)
#define BT_UUID_AQW_VOC BT_UUID_DECLARE_128(BT_UUID_AQW_VOC_VAL)
#define BT_UUID_AQW_PM25 BT_UUID_DECLARE_128(BT_UUID_AQW_PM25_VAL)

int app_ble_aqw_publish(struct bt_conn *conn, const struct app_ble_payload *data);

#endif /*_AQW_SERVICE_H*/