/*
 * Copyright Circuit Dojo (c) 2021
 *
 * SPDX-License-Identifier: LicenseRef-Circuit-Dojo-5-Clause
 */

#include <zephyr.h>

#include <app_event_manager.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(app_event_manager);

/* Define message queue */
K_MSGQ_DEFINE(app_event_msq, sizeof(struct app_event), APP_EVENT_QUEUE_SIZE, 4);

int app_event_manager_push(struct app_event *p_evt)
{
    return k_msgq_put(&app_event_msq, p_evt, K_NO_WAIT);
}

int app_event_manager_get(struct app_event *p_evt)
{
    return k_msgq_get(&app_event_msq, p_evt, K_FOREVER);
}

char *app_event_type_to_string(enum app_event_type type)
{
    switch (type)
    {
    case APP_EVENT_ETHERNET_DISCONNECT:
        return "APP_EVENT_ETHERNET_DISCONNECT";
    case APP_EVENT_ETHERNET_CONNECTED:
        return "APP_EVENT_ETHERNET_CONNECTED";
    case APP_EVENT_ETHERNET_READY:
        return "APP_EVENT_ETHERNET_READY";
    case APP_EVENT_BACKEND_ERROR:
        return "APP_EVENT_BACKEND_ERROR";
    case APP_EVENT_BACKEND_CONNECTED:
        return "APP_EVENT_BACKEND_CONNECTED";
    case APP_EVENT_BACKEND_DISCONNECTED:
        return "APP_EVENT_BACKEND_DISCONNECTED";
    case APP_EVENT_BLE_DATA:
        return "APP_EVENT_BLE_DATA";
    default:
        return "UNKNOWN";
    }
}