/**
 * @author Jared Wolff (hello@jaredwolff.com)
 * @copyright Copyright Circuit Dojo LLC 2021
 */

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/settings/settings.h>
#include <zephyr/mgmt/mcumgr/smp_bt.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app_ble);

#include <os_mgmt/os_mgmt.h>
#include <img_mgmt/img_mgmt.h>

/* Services */
#include <services/aqw.h>

/* Local */
#include <app_ble.h>

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

int app_ble_init(void)
{

    int err = 0;

    /* Enable BLE peripheral */
    err = bt_enable(NULL);
    if (err)
    {
        LOG_ERR("Bluetooth init failed. Err: %i", err);
        return err;
    }

    if (IS_ENABLED(CONFIG_SETTINGS))
    {
        settings_load();
    }

    /* Register DFU */
    os_mgmt_register_group();
    img_mgmt_register_group();
    smp_bt_register();

    /* Start advertising */
    err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err)
    {
        LOG_ERR("Advertising failed to start (err %d)", err);
        return err;
    }

    LOG_INF("Bluetooth initialization complete!");
    return 0;
}

int app_ble_publish_sensor_data(const struct app_ble_payload *data)
{
    return app_ble_aqw_publish(NULL, data);
}
