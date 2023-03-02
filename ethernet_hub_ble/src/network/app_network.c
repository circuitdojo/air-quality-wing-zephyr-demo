
/**
 * @author Jared Wolff (hello@jaredwolff.com)
 * @copyright Copyright Circuit Dojo LLC 2022
 */

#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_context.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app_network);

#include <app_event_manager.h>

/* Network mgmt */
static struct net_mgmt_event_callback mgmt_cb;

/* Network interface */
struct net_if *iface;

/**
 * @brief Network management handler
 *
 * @param cb
 * @param mgmt_event
 * @param iface
 */
static void net_mgmt_handler(struct net_mgmt_event_callback *cb,
                             uint32_t mgmt_event,
                             struct net_if *iface)
{

    int i = 0;

    switch (mgmt_event)
    {
    case NET_EVENT_IPV4_ADDR_ADD:

        for (i = 0; i < NET_IF_MAX_IPV4_ADDR; i++)
        {
            char buf[NET_IPV4_ADDR_LEN];

            if (iface->config.ip.ipv4->unicast[i].addr_type !=
                NET_ADDR_DHCP)
            {
                continue;
            }

            LOG_INF("Your address: %s",
                    (char *)(net_addr_ntop(AF_INET,
                                           &iface->config.ip.ipv4->unicast[i].address.in_addr,
                                           buf, sizeof(buf))));
            LOG_INF("Lease time: %u seconds",
                    iface->config.dhcpv4.lease_time);
            LOG_INF("Subnet: %s",
                    (char *)(net_addr_ntop(AF_INET,
                                           &iface->config.ip.ipv4->netmask,
                                           buf, sizeof(buf))));
            LOG_INF("Router: %s",
                    (char *)(net_addr_ntop(AF_INET,
                                           &iface->config.ip.ipv4->gw,
                                           buf, sizeof(buf))));

            /* Ready event */
            APP_EVENT_MANAGER_PUSH(APP_EVENT_ETHERNET_READY);
        }

        break;
    default:
        LOG_INF("Net management event: %i", mgmt_event);
        break;
    }
}

/**
 * @brief Initialize ethernet network
 *
 */
void app_net_init()
{

    LOG_INF("Run DHCPv4 client");

    /* Setup callback */
    net_mgmt_init_event_callback(&mgmt_cb, net_mgmt_handler,
                                 NET_EVENT_IPV4_ADDR_ADD);
    net_mgmt_add_event_callback(&mgmt_cb);

    /* Get interface */
    iface = net_if_get_default();

    /* Start dhcp discovery */
    net_dhcpv4_start(iface);
}