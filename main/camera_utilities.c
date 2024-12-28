/*
 * Home monitoring system
 * Author: Maksymilian Komarnicki
 */

#include <string.h>

#include "esp_log.h"
#include "esp_err.h"

#include "esp_camera.h"

#include "camera_utilities.h"
#include "streamer_camera.h"

// ===================
// Select camera model
// ===================
// #define CAMERA_MODEL_WROVER_KIT                   // Has PSRAM
// #define CAMERA_MODEL_ESP_EYE                      // Has PSRAM
// #define CAMERA_MODEL_ESP32S3_EYE                  // Has PSRAM
// #define CAMERA_MODEL_M5STACK_PSRAM                // Has PSRAM
// #define CAMERA_MODEL_M5STACK_V2_PSRAM             // M5Camera version B Has PSRAM
// #define CAMERA_MODEL_M5STACK_WIDE                 // Has PSRAM
// #define CAMERA_MODEL_M5STACK_ESP32CAM             // No PSRAM
// #define CAMERA_MODEL_M5STACK_UNITCAM              // No PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
// #define CAMERA_MODEL_TTGO_T_JOURNAL               // No PSRAM
// #define CAMERA_MODEL_XIAO_ESP32S3                 // Has PSRAM
//  ** Espressif Internal Boards **
// #define CAMERA_MODEL_ESP32_CAM_BOARD
// #define CAMERA_MODEL_ESP32S2_CAM_BOARD
// #define CAMERA_MODEL_ESP32S3_CAM_LCD
// #define CAMERA_MODEL_DFRobot_FireBeetle2_ESP32S3  // Has PSRAM
// #define CAMERA_MODEL_DFRobot_Romeo_ESP32S3        // Has PSRAM

#include "camera_pins.h"

static const char *TAG = "CAMERA_UTILITIES";

streamer_pixformat_t transform_to_stream_pixel_format(pixformat_t pixformat)
{
    switch (pixformat)
    {
    case PIXFORMAT_RGB565:
        return STREAMER_PIXFORMAT_RGB565;
    case PIXFORMAT_YUV422:
        return STREAMER_PIXFORMAT_YUV422;
    case PIXFORMAT_YUV420:
        return STREAMER_PIXFORMAT_YUV420;
    case PIXFORMAT_GRAYSCALE:
        return STREAMER_PIXFORMAT_GRAYSCALE;
    case PIXFORMAT_JPEG:
        return STREAMER_PIXFORMAT_JPEG;
    case PIXFORMAT_RGB888:
        return STREAMER_PIXFORMAT_RGB888;
    case PIXFORMAT_RAW:
        return STREAMER_PIXFORMAT_RAW;
    case PIXFORMAT_RGB444:
        return STREAMER_PIXFORMAT_RGB444;
    case PIXFORMAT_RGB555:
        return STREAMER_PIXFORMAT_RGB555;
    }

    // How to handle default (???)
    return STREAMER_PIXFORMAT_JPEG;
}

pixformat_t transform_to_camera_pixel_format(streamer_pixformat_t streamer_pixformat)
{
    switch (streamer_pixformat)
    {
    case STREAMER_PIXFORMAT_RGB565:
        return PIXFORMAT_RGB565;
    case STREAMER_PIXFORMAT_YUV422:
        return PIXFORMAT_YUV422;
    case STREAMER_PIXFORMAT_YUV420:
        return PIXFORMAT_YUV420;
    case STREAMER_PIXFORMAT_GRAYSCALE:
        return PIXFORMAT_GRAYSCALE;
    case STREAMER_PIXFORMAT_JPEG:
        return PIXFORMAT_JPEG;
    case STREAMER_PIXFORMAT_RGB888:
        return PIXFORMAT_RGB888;
    case STREAMER_PIXFORMAT_RAW:
        return PIXFORMAT_RAW;
    case STREAMER_PIXFORMAT_RGB444:
        return PIXFORMAT_RGB444;
    case STREAMER_PIXFORMAT_RGB555:
        return PIXFORMAT_RGB555;
    }

    // How to handle default (???)
    return PIXFORMAT_JPEG;
}

framesize_t transform_to_camera_frame_size(streamer_framesize_t streamer_framesize)
{
    switch (streamer_framesize)
    {
    case STREAMER_FRAMESIZE_96X96:
        return FRAMESIZE_96X96;
    case STREAMER_FRAMESIZE_QQVGA:
        return FRAMESIZE_QQVGA;
    case STREAMER_FRAMESIZE_QCIF:
        return FRAMESIZE_QCIF;
    case STREAMER_FRAMESIZE_HQVGA:
        return FRAMESIZE_HQVGA;
    case STREAMER_FRAMESIZE_240X240:
        return FRAMESIZE_240X240;
    case STREAMER_FRAMESIZE_QVGA:
        return FRAMESIZE_QVGA;
    case STREAMER_FRAMESIZE_CIF:
        return FRAMESIZE_CIF;
    case STREAMER_FRAMESIZE_HVGA:
        return FRAMESIZE_HVGA;
    case STREAMER_FRAMESIZE_VGA:
        return FRAMESIZE_VGA;
    case STREAMER_FRAMESIZE_SVGA:
        return FRAMESIZE_SVGA;
    case STREAMER_FRAMESIZE_XGA:
        return FRAMESIZE_XGA;
    case STREAMER_FRAMESIZE_HD:
        return FRAMESIZE_HD;
    case STREAMER_FRAMESIZE_SXGA:
        return FRAMESIZE_SXGA;
    case STREAMER_FRAMESIZE_UXGA:
        return FRAMESIZE_UXGA;
    case STREAMER_FRAMESIZE_FHD:
        return FRAMESIZE_FHD;
    case STREAMER_FRAMESIZE_P_HD:
        return FRAMESIZE_P_HD;
    case STREAMER_FRAMESIZE_P_3MP:
        return FRAMESIZE_P_3MP;
    case STREAMER_FRAMESIZE_QXGA:
        return FRAMESIZE_QXGA;
    case STREAMER_FRAMESIZE_QHD:
        return FRAMESIZE_QHD;
    case STREAMER_FRAMESIZE_WQXGA:
        return FRAMESIZE_WQXGA;
    case STREAMER_FRAMESIZE_P_FHD:
        return FRAMESIZE_P_FHD;
    case STREAMER_FRAMESIZE_QSXGA:
        return FRAMESIZE_QSXGA;
    case STREAMER_FRAMESIZE_INVALID:
        return FRAMESIZE_INVALID;
    }

    // How to handle default (???)
    return FRAMESIZE_CIF;
}

camera_grab_mode_t transform_to_camera_grab_mode(streamer_grab_mode_t streamer_grabmode)
{
    switch (streamer_grabmode)
    {
    case STREAMER_GRAB_WHEN_EMPTY:
        return CAMERA_GRAB_WHEN_EMPTY;
    case STREAMER_GRAB_LATEST:
        return CAMERA_GRAB_LATEST;
    }

    // How to handle default (???)
    return CAMERA_GRAB_WHEN_EMPTY;
}

void prepare_camera_config(camera_config_t *camera_conf, const streamer_hal_config_t *stream_conf)
{
    camera_conf->pin_pwdn = PWDN_GPIO_NUM;
    camera_conf->pin_reset = RESET_GPIO_NUM;
    camera_conf->pin_xclk = XCLK_GPIO_NUM;
    camera_conf->pin_sccb_sda = SIOD_GPIO_NUM;
    camera_conf->pin_sccb_scl = SIOC_GPIO_NUM;

    camera_conf->pin_d7 = Y9_GPIO_NUM;
    camera_conf->pin_d6 = Y8_GPIO_NUM;
    camera_conf->pin_d5 = Y7_GPIO_NUM;
    camera_conf->pin_d4 = Y6_GPIO_NUM;
    camera_conf->pin_d3 = Y5_GPIO_NUM;
    camera_conf->pin_d2 = Y4_GPIO_NUM;
    camera_conf->pin_d1 = Y3_GPIO_NUM;
    camera_conf->pin_d0 = Y2_GPIO_NUM;
    camera_conf->pin_vsync = VSYNC_GPIO_NUM;
    camera_conf->pin_href = HREF_GPIO_NUM;
    camera_conf->pin_pclk = PCLK_GPIO_NUM;

    camera_conf->xclk_freq_hz = 20000000;
    camera_conf->ledc_timer = LEDC_TIMER_0;
    camera_conf->ledc_channel = LEDC_CHANNEL_0;

    camera_conf->pixel_format = transform_to_camera_pixel_format(stream_conf->pixel_format);
    camera_conf->frame_size = transform_to_camera_frame_size(stream_conf->frame_size);

    camera_conf->jpeg_quality = stream_conf->jpeg_quality;
    camera_conf->fb_count = (size_t)stream_conf->fb_count;
    camera_conf->grab_mode = transform_to_camera_grab_mode(stream_conf->grab_mode);
}

esp_err_t start_camera(const streamer_hal_config_t *stream_config)
{
    esp_err_t ret;
    camera_config_t camera_config;
    prepare_camera_config(&camera_config, stream_config);

    if (PWDN_GPIO_NUM != -1)
    {
        gpio_set_direction(PWDN_GPIO_NUM, GPIO_MODE_OUTPUT);
        gpio_set_level(PWDN_GPIO_NUM, 0);
    }

    ret = esp_camera_init(&camera_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera initialization failed");
        return ret;
    }

    return ESP_OK;
}

esp_err_t stop_camera()
{
    esp_err_t ret;

    ret = esp_camera_deinit();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera deinitialization failed");
        return ret;
    }

    return ESP_OK;
}

esp_err_t send_camera_frame(stream_fb_t *stream_fb)
{
    camera_fb_t *camera_fb = esp_camera_fb_get();
    if (camera_fb == NULL)
    {
        ESP_LOGE(TAG, "Camera Frame Buffer capture failed");
        return ESP_FAIL;
    }

    stream_fb->len = (int)camera_fb->len;
    stream_fb->width = (int)camera_fb->width;
    stream_fb->height = (int)camera_fb->height;
    stream_fb->format = transform_to_stream_pixel_format(camera_fb->format);
    stream_fb->timestamp.tv_sec = camera_fb->timestamp.tv_sec;
    stream_fb->timestamp.tv_usec = camera_fb->timestamp.tv_usec;

    memcpy(stream_fb->buf, camera_fb->buf, camera_fb->len);
    esp_camera_fb_return(camera_fb);

    return ESP_OK;
}
