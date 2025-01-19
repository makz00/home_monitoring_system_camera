/*
 * Home monitoring system
 * Author: Maksymilian Komarnicki
 */

#include "esp_log.h"
#include "esp_err.h"

#include "mdns.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "camera_utilities.h"
#include "espfsp_client_push.h"
#include "udps_handler.h"

#define CONFIG_STREAMER_STACK_SIZE 4096
#define CONFIG_STREAMER_PRIORITY 5

#define CONFIG_STREAMER_PORT_CONTROL 5001
#define CONFIG_STREAMER_PORT_DATA 5002

#define CONFIG_STREAMER_CAMERA_PIXFORMAT ESPFSP_PIXFORMAT_JPEG
#define CONFIG_STREAMER_CAMERA_FRAMESIZE ESPFSP_FRAMESIZE_CIF
#define CONFIG_STREAMER_CAMERA_GRAB_MODE ESPFSP_GRAB_WHEN_EMPTY
#define CONFIG_STREAMER_CAMERA_JPEG_QUALITY 6
#define CONFIG_STREAMER_CAMERA_FB_COUNT 2

#define CONFIG_STREAMER_FRAME_MAX_LENGTH (100 * 1014)
#define CONFIG_STREAMER_FPS 15

#define CONFIG_STREAMER_MDNS_SERVER_NAME "espfsp_server"

static const char *TAG = "STREAMER_HANDLER";

static espfsp_client_push_handler_t client_push_handler = NULL;

static esp_err_t resolve_mdns_host(const char * host_name, struct esp_ip4_addr *addr)
{
    esp_err_t err = mdns_init();
    if (err) {
        ESP_LOGI(TAG, "MDNS Init failed: %d\n", err);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Query A: %s.local", host_name);

    addr->addr = 0;

    err = mdns_query_a(host_name, 2000,  addr);
    if(err){
        if(err == ESP_ERR_NOT_FOUND){
            ESP_LOGI(TAG, "Host was not found!");
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "Query Failed");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, IPSTR, IP2STR(addr));
    return ESP_OK;
}

esp_err_t udps_camera_init(){
    esp_err_t ret = ESP_OK;

    struct esp_ip4_addr server_addr;
    while (resolve_mdns_host(CONFIG_STREAMER_MDNS_SERVER_NAME, &server_addr) != ESP_OK)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }

    espfsp_client_push_config_t streamer_config = {
        .data_task_info = {
            .stack_size = CONFIG_STREAMER_STACK_SIZE,
            .task_prio = CONFIG_STREAMER_PRIORITY,
        },
        .session_and_control_task_info = {
            .stack_size = CONFIG_STREAMER_STACK_SIZE,
            .task_prio = CONFIG_STREAMER_PRIORITY,
        },
        .local = {
            .control_port = CONFIG_STREAMER_PORT_CONTROL,
            .data_port = CONFIG_STREAMER_PORT_DATA,
        },
        .remote = {
            .control_port = CONFIG_STREAMER_PORT_CONTROL,
            .data_port = CONFIG_STREAMER_PORT_DATA,
        },
        .data_transport = ESPFSP_TRANSPORT_UDP,
        .remote_addr.addr = server_addr.addr,
        .cb = {
            .start_cam = start_camera,
            .stop_cam = stop_camera,
            .send_frame = send_camera_frame,
            .reconf_cam = reconf_camera,
        },
        .frame_config = {
            .frame_max_len = CONFIG_STREAMER_FRAME_MAX_LENGTH,
            .fps = CONFIG_STREAMER_FPS,
            .buffered_fbs = 1,              // Not needed as there is no buffering
            .fb_in_buffer_before_get = 1,   // Not needed as there is no buffering
        },
        .cam_config = {
            .cam_grab_mode = CONFIG_STREAMER_CAMERA_GRAB_MODE,
            .cam_jpeg_quality = CONFIG_STREAMER_CAMERA_JPEG_QUALITY,
            .cam_fb_count = CONFIG_STREAMER_CAMERA_FB_COUNT,
            .cam_pixel_format = CONFIG_STREAMER_CAMERA_PIXFORMAT,
            .cam_frame_size = CONFIG_STREAMER_CAMERA_FRAMESIZE,
        }
    };

    ret = init_config(&streamer_config.cam_config, &streamer_config.frame_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera init failed");
        return ret;
    }

    client_push_handler = espfsp_client_push_init(&streamer_config);
    if (client_push_handler == NULL) {
        ESP_LOGE(TAG, "Client push ESPFSP init failed");
        return ESP_FAIL;
    }

    return ESP_OK;
}
