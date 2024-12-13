/*
 * Home monitoring system
 * Author: Maksymilian Komarnicki
 */

#include "esp_err.h"
#include "esp_log.h"
#include "mdns.h"

#include "mdns_handler.h"

static const char *TAG = "MDNS_HANDLER";
static const char* MDNS_HOSTNAME = "home_monitoring_camera";

esp_err_t mdns_service_init()
{
    esp_err_t err = mdns_init();
    if (err) {
        ESP_LOGE(TAG, "MDNS Init failed: %d\n", err);
        return err;
    }

    err = mdns_hostname_set(MDNS_HOSTNAME);
    if (err) {
        ESP_LOGE(TAG, "MDNS hostname set failed: %d\n", err);
        return err;
    }

    return ESP_OK;
}
