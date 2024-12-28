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

#include "esp_log.h"
#include "esp_err.h"

#include "camera_utilities.h"
#include "streamer_camera.h"
#include "udps_handler.h"

#define CONFIG_STREAMER_STACK_SIZE 4096
#define CONFIG_STREAMER_PRIORITY 5

#define CONFIG_STREAMER_PORT_CONTROL 5001
#define CONFIG_STREAMER_PORT_DATA 5002

#define CONFIG_STREAMER_MDNS_SERVER_NAME "central_server"

// Streamer-protocol settings
#define CONFIG_STREAMER_FPS 15
#define CONFIG_STREAMER_FRAME_MAX_LENGTH (100 * 1014)

// Streamer-camera settings
#define CONFIG_STREAMER_CAMERA_PIXFORMAT STREAMER_PIXFORMAT_JPEG
#define CONFIG_STREAMER_CAMERA_FRAMESIZE STREAMER_FRAMESIZE_CIF
#define CONFIG_STREAMER_CAMERA_GRAB_MODE STREAMER_GRAB_WHEN_EMPTY
#define CONFIG_STREAMER_CAMERA_JPEG_QUALITY 6
#define CONFIG_STREAMER_CAMERA_FB_COUNT 2

static const char *TAG = "STREAMER_HANDLER";

esp_err_t udps_camera_init(){
    streamer_config_t streamer_config = {
        .data_send_task_info = {
            .stack_size = CONFIG_STREAMER_STACK_SIZE,
            .task_prio = CONFIG_STREAMER_PRIORITY,
        },
        .control_task_info = {
            .stack_size = CONFIG_STREAMER_STACK_SIZE,
            .task_prio = CONFIG_STREAMER_PRIORITY,
        },
        .camera_local_ports = {
            .control_port = CONFIG_STREAMER_PORT_CONTROL,
            .data_port = CONFIG_STREAMER_PORT_DATA,
        },
        .camera_remote_ports = {
            .control_port = CONFIG_STREAMER_PORT_CONTROL,
            .data_port = CONFIG_STREAMER_PORT_DATA,
        },
        .trans = STREAMER_TRANSPORT_UDP,
        .cb = {
            .start_cam = start_camera,
            .stop_cam = stop_camera,
            .send_frame = send_camera_frame,
        },
        .hal = {
            .pixel_format = CONFIG_STREAMER_CAMERA_PIXFORMAT,
            .frame_size = CONFIG_STREAMER_CAMERA_FRAMESIZE,
            .jpeg_quality = CONFIG_STREAMER_CAMERA_JPEG_QUALITY,
            .grab_mode = CONFIG_STREAMER_CAMERA_GRAB_MODE,
            .fb_count = CONFIG_STREAMER_CAMERA_FB_COUNT,
        },
        .frame_max_len = CONFIG_STREAMER_FRAME_MAX_LENGTH,
        .fps = CONFIG_STREAMER_FPS,
        .server_mdns_name = CONFIG_STREAMER_MDNS_SERVER_NAME
    };

    esp_err_t err = streamer_camera_init(&streamer_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "UDP streamer handler camera Init Failed");
        return err;
    }

    return ESP_OK;
}
