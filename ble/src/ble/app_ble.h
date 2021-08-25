/**
 * @author Jared Wolff (hello@jaredwolff.com)
 * @copyright Copyright Circuit Dojo LLC 2021
 */

#ifndef _APP_BLE_H
#define _APP_BLE_H

/* Air Quality Wing Dependencies */
#include <aqw.h>
#include <drivers/sensor.h>

struct app_ble_payload
{
    enum aqw_sensor_type type;
    struct sensor_value value;
    uint64_t ts;
};

int app_ble_init(void);
int app_ble_publish_sensor_data(const struct app_ble_payload *data);

#endif