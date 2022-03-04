/**
 * @author Jared Wolff (hello@jaredwolff.com)
 * @copyright Copyright Circuit Dojo LLC 2022
 */

#include <zephyr.h>
#include <stdlib.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>

#include <codec/aqw.h>

/* Local */
#include <app_ble.h>
#include <app_ble/scan.h>
#include <app_ble/gatt.h>
#include <app_ble/services/aqw_c.h>
#include <app_event_manager.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(app_ble);

static struct bt_conn *default_conn;
static struct aqw_ble_client client = {0};

static void client_data(struct aqw_sensor_data *payload)
{
    /* Evt */
    struct app_event evt = {
        .type = APP_EVENT_BLE_DATA,
        .data = *payload,
    };

    /* Send it further down the chain */
    app_event_manager_push(&evt);
}

static void discovery_completed(struct bt_gatt_dm *dm, void *context)
{

    /* Init */
    aqw_ble_client_init(&client, client_data);

    /* Set handles */
    aqw_ble_client_handles_assign(&client, dm);

    /* Subscribe to events */
    aqw_ble_client_subscribe(&client);
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    /* Get the ble addr */
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    LOG_INF("Connected: %s", log_strdup(addr));

    /* Ref incoming */
    default_conn = bt_conn_ref(conn);

    if (err)
    {
        LOG_INF("Failed to connect to %s (%u)", addr, err);

        bt_conn_unref(default_conn);
        default_conn = NULL;

        start_scan();
        return;
    }

    if (conn != default_conn)
    {
        LOG_WRN("Conn not valid.");
        return;
    }

    // Discover services/subscribe
    gatt_discover(conn, &discovery_completed);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    if (conn != default_conn)
    {
        return;
    }

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    LOG_INF("Disconnected: %s (reason 0x%02x)", log_strdup(addr), reason);

    bt_conn_unref(default_conn);
    default_conn = NULL;

    start_scan();
}

static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected};

int app_ble_init()
{

    int err;

    /* Enable bluetooth */
    err = bt_enable(NULL);
    if (err)
    {
        LOG_INF("Bluetooth init failed (err %d)", err);
        return err;
    }

    LOG_INF("Bluetooth initialized");

    /* Set callback*/
    bt_conn_cb_register(&conn_callbacks);

    /* Initialize scan */
    err = init_scan();
    if (err)
    {
        LOG_INF("Bluetooth scan init failed (err %d)", err);
        return err;
    }

    /* Start scan */
    start_scan();

    return 0;
}