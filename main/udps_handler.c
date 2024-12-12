/*
 * Home monitoring system
 * Author: Maksymilian Komarnicki
 */

/*
 * ASSUMPTIONS BEG --------------------------------------------------------------------------------
 * ASSUMPTIONS END --------------------------------------------------------------------------------
 *
 * TODO BEG ---------------------------------------------------------------------------------------
 * - Configuration options to add in 'menuconfig'/Kconfig file
 * TODO END ---------------------------------------------------------------------------------------
 */

#include "driver/gpio.h"
#include "esp_log.h"
#include "udp_streamer_handler.h"
#include "udps_handler.h"

#define CONFIG_UDP_STREAMER_HANDLER_PORT_CONTROL 5001

static const char *TAG = "UDPS_HANDLER";

static udps_camera_config_t udps_camera_config = {
    .control_port = CONFIG_UDP_STREAMER_HANDLER_PORT_CONTROL,
};

esp_err_t udps_camera_init(){
    esp_err_t err = udp_streamer_handler_camera_init(&udps_camera_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "UDP streamer handler camera Init Failed");
        return err;
    }

    return ESP_OK;
}
