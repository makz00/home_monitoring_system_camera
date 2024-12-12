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

#define UDPS_SENDER_PRIORITY 5
#define UDPS_CONTROL_PRIORITY 5
#define MAX_UDPS_RECEIERS 1

#include "esp_err.h"
#include "esp_log.h"
#include "sys/time.h"
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "udp_streamer_handler.h"
#include "udp_streamer_handler_controler.h"
#include "udp_streamer_handler_sender.h"
#include "udp_streamer_handler_types.h"

static const char *TAG = "UDP_STREAMER_CAMERA_COMPONENT";

static udps_camera_state_t *s_state = NULL;

QueueHandle_t xQueue;

esp_err_t udp_streamer_handler_handler_camera_queue_init()
{
    xQueue = xQueueCreate(MAX_UDPS_RECEIERS, sizeof(stream_receiver_addr_t));

    if (xQueue == NULL)
    {
        ESP_LOGE(TAG, "Cannot initialize queue");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t udp_streamer_handler_sender_start()
{
    BaseType_t xStatus;

    xStatus = xTaskCreate(udp_streamer_camera_sender_task, "udp_streamer_camera_sender_task", 4096, NULL, UDPS_SENDER_PRIORITY, &s_state->sender_task_handle);
    if (xStatus != pdPASS)
    {
        ESP_LOGE(TAG, "Could not start sender task!");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t udp_streamer_handler_server_start(int control_port)
{
    BaseType_t xStatus;

    xStatus = xTaskCreate(udp_streamer_camera_server_task, "udp_streamer_camera_server_task", 4096, (void *)control_port, UDPS_CONTROL_PRIORITY, &s_state->control_task_handle);
    if (xStatus != pdPASS)
    {
        ESP_LOGE(TAG, "Could not start control task!");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t udp_streamer_handler_camera_init(const udps_camera_config_t *config)
{
    esp_err_t ret;

    s_state = malloc(sizeof(udps_camera_state_t));
    if (!s_state)
    {
        ESP_LOGE(TAG, "State allocation failed");
        return ESP_FAIL;
    }

    ret = udp_streamer_handler_handler_camera_queue_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Queue has not been created successfully");
        return ESP_FAIL;
    }

    ret = udp_streamer_handler_sender_start();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Sender has not been created successfully");
        return ESP_FAIL;
    }

    ret = udp_streamer_handler_server_start(config->control_port);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Server has not been created successfully");
        return ESP_FAIL;
    }

    return ret;
}

esp_err_t udp_streamer_handler_camera_deinit(void)
{
    ESP_LOGE(TAG, "NOT IMPLEMENTED!");

    return ESP_FAIL;
}
