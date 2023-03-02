
/**
 * @author Jared Wolff (hello@jaredwolff.com)
 * @copyright Copyright Circuit Dojo LLC 2022
 */

#include <zephyr/kernel.h>

#include <app_ble/gatt.h>
#include <app_ble/services/aqw.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app_ble_gatt);

static gatt_discovery_complete_cb cb = NULL;

static void discovery_completed(struct bt_gatt_dm *dm, void *context)
{

    LOG_INF("Discovery complete!");

    /* Notify that we've completed discovery */
    if (cb != NULL)
    {
        cb(dm, NULL);
    }

    /* Release data*/
    bt_gatt_dm_data_release(dm);
}

static void discovery_service_not_found(struct bt_conn *conn, void *ctx)
{
    LOG_WRN("Air Quality Wing service not found!");
}

static void discovery_error_found(struct bt_conn *conn, int err, void *ctx)
{
    LOG_WRN("The discovery procedure failed, err %d", err);

    // Disconnect from device
    bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
}

static struct bt_gatt_dm_cb discovery_cb = {
    .completed = discovery_completed,
    .service_not_found = discovery_service_not_found,
    .error_found = discovery_error_found,
};

void gatt_discover(struct bt_conn *conn, gatt_discovery_complete_cb _cb)
{
    int err;

    /* Set the callback */
    cb = _cb;

    err = bt_gatt_dm_start(conn,
                           BT_UUID_AQW_SERVICE,
                           &discovery_cb,
                           NULL);
    if (err)
    {
        LOG_ERR("could not start the discovery procedure, error "
                "code: %d",
                err);
    }
}
