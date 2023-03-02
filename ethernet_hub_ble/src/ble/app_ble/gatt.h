/**
 * @author Jared Wolff (hello@jaredwolff.com)
 * @copyright Copyright Circuit Dojo LLC 2022
 */

#ifndef GATT_H
#define GATT_H

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>

#include <bluetooth/gatt_dm.h>

typedef void (*gatt_discovery_complete_cb)(struct bt_gatt_dm *dm,
                                           void *context);

void gatt_discover(struct bt_conn *conn, gatt_discovery_complete_cb cb);

#endif
