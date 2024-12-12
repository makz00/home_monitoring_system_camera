/*
 * Home monitoring system
 * Author: Maksymilian Komarnicki
 */

#pragma once

#include "sys/time.h"

typedef int esp_err_t;

/**
 * @brief UDP streamer server side configuration
 */
typedef struct
{
    int control_port;
} udps_camera_config_t;

/**
 * @brief Initialize the UDP streamer handler camera side application protocol
 *
 * @param config  UDP streamer handler configuration parameters
 *
 * @return ESP_OK on success
 */
esp_err_t udp_streamer_handler_camera_init(const udps_camera_config_t *config);

/**
 * @brief Deinitialize the UDP streamer handler camera
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if the application protocol hasn't been initialized yet
 */
esp_err_t udp_streamer_handler_camera_deinit(void);
