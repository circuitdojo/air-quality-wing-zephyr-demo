/**
 * @author Jared Wolff (hello@jaredwolff.com)
 * @copyright Copyright Circuit Dojo LLC 2022
 */

#include <zephyr/kernel.h>

#include <app_ble/scan.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app_ble_scan);

static int init_scan_filters(void)
{
    int err;

    /* Remove filters first */
    bt_scan_filter_remove_all();

    // Add a filter
    err = bt_scan_filter_add(BT_SCAN_FILTER_TYPE_NAME, "Air Quality Wing");
    if (err)
    {
        LOG_WRN("Scanning filters cannot be set. Err: %d", err);
        return err;
    }

    /* Enable name filter */
    err = bt_scan_filter_enable(BT_SCAN_NAME_FILTER, true);
    if (err)
    {
        LOG_WRN("Filters cannot be turned on. Err: %d", err);
        return err;
    }

    return 0;
}

static void scan_connecting_error(struct bt_scan_device_info *device_info)
{
    LOG_ERR("Connecting failed");
}

static void scan_connecting(struct bt_scan_device_info *device_info,
                            struct bt_conn *conn)
{
    LOG_INF("Scan connecting..");
}

static void scan_filter_match(struct bt_scan_device_info *device_info,
                              struct bt_scan_filter_match *filter_match,
                              bool connectable)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(device_info->recv_info->addr, addr, sizeof(addr));

    LOG_INF("Scan match: [addr: %s]", (char*)addr);
}

BT_SCAN_CB_INIT(scan_cb, scan_filter_match, NULL,
                scan_connecting_error, scan_connecting);

int init_scan()
{
    int err = 0;

    // Scan parameters setup
    struct bt_scan_init_param scan_init = {
        .connect_if_match = true,
        .scan_param = NULL,
        .conn_param = BT_LE_CONN_PARAM_DEFAULT,
    };

    /* Init scan */
    bt_scan_init(&scan_init);
    bt_scan_cb_register(&scan_cb);

    /* Init filters */
    err = init_scan_filters();
    if (err)
        return err;

    return 0;
}

void start_scan()
{
    int err;

    /* This demo doesn't require active scan */
    err = bt_scan_start(BT_LE_SCAN_TYPE_PASSIVE);
    if (err)
    {
        LOG_INF("Scanning failed to start (err %d)", err);
        return;
    }

    LOG_INF("Scanning successfully started");
}