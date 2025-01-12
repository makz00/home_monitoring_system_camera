/*
 * Home monitoring system
 * Author: Maksymilian Komarnicki
 */

#include <string.h>

#include "esp_log.h"
#include "esp_err.h"

#include "esp_camera.h"

#include "camera_utilities.h"
#include "espfsp_client_push.h"

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

espfsp_pixformat_t transform_to_stream_pixel_format(pixformat_t pixformat)
{
    switch (pixformat)
    {
    case PIXFORMAT_RGB565:
        return ESPFSP_PIXFORMAT_RGB565;
    case PIXFORMAT_YUV422:
        return ESPFSP_PIXFORMAT_YUV422;
    case PIXFORMAT_YUV420:
        return ESPFSP_PIXFORMAT_YUV420;
    case PIXFORMAT_GRAYSCALE:
        return ESPFSP_PIXFORMAT_GRAYSCALE;
    case PIXFORMAT_JPEG:
        return ESPFSP_PIXFORMAT_JPEG;
    case PIXFORMAT_RGB888:
        return ESPFSP_PIXFORMAT_RGB888;
    case PIXFORMAT_RAW:
        return ESPFSP_PIXFORMAT_RAW;
    case PIXFORMAT_RGB444:
        return ESPFSP_PIXFORMAT_RGB444;
    case PIXFORMAT_RGB555:
        return ESPFSP_PIXFORMAT_RGB555;
    }

    ESP_LOGE(TAG, "Not handled camera pixformat. Return jpeg by default");
    return ESPFSP_PIXFORMAT_JPEG;
}

pixformat_t transform_to_camera_pixel_format(espfsp_pixformat_t streamer_pixformat)
{
    switch (streamer_pixformat)
    {
    case ESPFSP_PIXFORMAT_RGB565:
        return PIXFORMAT_RGB565;
    case ESPFSP_PIXFORMAT_YUV422:
        return PIXFORMAT_YUV422;
    case ESPFSP_PIXFORMAT_YUV420:
        return PIXFORMAT_YUV420;
    case ESPFSP_PIXFORMAT_GRAYSCALE:
        return PIXFORMAT_GRAYSCALE;
    case ESPFSP_PIXFORMAT_JPEG:
        return PIXFORMAT_JPEG;
    case ESPFSP_PIXFORMAT_RGB888:
        return PIXFORMAT_RGB888;
    case ESPFSP_PIXFORMAT_RAW:
        return PIXFORMAT_RAW;
    case ESPFSP_PIXFORMAT_RGB444:
        return PIXFORMAT_RGB444;
    case ESPFSP_PIXFORMAT_RGB555:
        return PIXFORMAT_RGB555;
    }

    ESP_LOGE(TAG, "Not handled espfsp pixformat. Return jpeg by default");
    return PIXFORMAT_JPEG;
}

framesize_t transform_to_camera_frame_size(espfsp_framesize_t streamer_framesize)
{
    switch (streamer_framesize)
    {
    case ESPFSP_FRAMESIZE_96X96:
        return FRAMESIZE_96X96;
    case ESPFSP_FRAMESIZE_QQVGA:
        return FRAMESIZE_QQVGA;
    case ESPFSP_FRAMESIZE_QCIF:
        return FRAMESIZE_QCIF;
    case ESPFSP_FRAMESIZE_HQVGA:
        return FRAMESIZE_HQVGA;
    case ESPFSP_FRAMESIZE_240X240:
        return FRAMESIZE_240X240;
    case ESPFSP_FRAMESIZE_QVGA:
        return FRAMESIZE_QVGA;
    case ESPFSP_FRAMESIZE_CIF:
        return FRAMESIZE_CIF;
    case ESPFSP_FRAMESIZE_HVGA:
        return FRAMESIZE_HVGA;
    case ESPFSP_FRAMESIZE_VGA:
        return FRAMESIZE_VGA;
    case ESPFSP_FRAMESIZE_SVGA:
        return FRAMESIZE_SVGA;
    case ESPFSP_FRAMESIZE_XGA:
        return FRAMESIZE_XGA;
    case ESPFSP_FRAMESIZE_HD:
        return FRAMESIZE_HD;
    case ESPFSP_FRAMESIZE_SXGA:
        return FRAMESIZE_SXGA;
    case ESPFSP_FRAMESIZE_UXGA:
        return FRAMESIZE_UXGA;
    case ESPFSP_FRAMESIZE_FHD:
        return FRAMESIZE_FHD;
    case ESPFSP_FRAMESIZE_P_HD:
        return FRAMESIZE_P_HD;
    case ESPFSP_FRAMESIZE_P_3MP:
        return FRAMESIZE_P_3MP;
    case ESPFSP_FRAMESIZE_QXGA:
        return FRAMESIZE_QXGA;
    case ESPFSP_FRAMESIZE_QHD:
        return FRAMESIZE_QHD;
    case ESPFSP_FRAMESIZE_WQXGA:
        return FRAMESIZE_WQXGA;
    case ESPFSP_FRAMESIZE_P_FHD:
        return FRAMESIZE_P_FHD;
    case ESPFSP_FRAMESIZE_QSXGA:
        return FRAMESIZE_QSXGA;
    case ESPFSP_FRAMESIZE_INVALID:
        return FRAMESIZE_INVALID;
    }

    ESP_LOGE(TAG, "Not handled espfsp framesize. Return cif by default");
    return FRAMESIZE_CIF;
}

camera_grab_mode_t transform_to_camera_grab_mode(espfsp_grab_mode_t streamer_grabmode)
{
    switch (streamer_grabmode)
    {
    case ESPFSP_GRAB_WHEN_EMPTY:
        return CAMERA_GRAB_WHEN_EMPTY;
    case ESPFSP_GRAB_LATEST:
        return CAMERA_GRAB_LATEST;
    }

    ESP_LOGE(TAG, "Not handled espfsp camera grab mode. Return 'grab when empty' by default");
    return CAMERA_GRAB_WHEN_EMPTY;
}

void prepare_camera_config(camera_config_t *camera_conf, const espfsp_cam_config_t *cam_config, const espfsp_frame_config_t *frame_config)
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

    camera_conf->pixel_format = transform_to_camera_pixel_format(frame_config->pixel_format);
    camera_conf->frame_size = transform_to_camera_frame_size(frame_config->frame_size);

    camera_conf->jpeg_quality = cam_config->cam_jpeg_quality;
    camera_conf->fb_count = (size_t) cam_config->cam_fb_count;
    camera_conf->grab_mode = transform_to_camera_grab_mode(cam_config->cam_grab_mode);
}

esp_err_t start_camera(const espfsp_cam_config_t *cam_config, const espfsp_frame_config_t *frame_config)
{
    esp_err_t ret = ESP_OK;

    camera_config_t camera_config;
    prepare_camera_config(&camera_config, cam_config, frame_config);

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
    esp_err_t ret = ESP_OK;

    ret = esp_camera_deinit();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera deinitialization failed");
        return ret;
    }

    return ESP_OK;
}

esp_err_t send_camera_frame(espfsp_fb_t *fb)
{
    camera_fb_t *camera_fb = esp_camera_fb_get();
    if (camera_fb == NULL)
    {
        ESP_LOGE(TAG, "Camera frame buffer capture failed");
        return ESP_FAIL;
    }

    fb->len = (int) camera_fb->len;
    fb->width = (int) camera_fb->width;
    fb->height = (int) camera_fb->height;
    fb->format = transform_to_stream_pixel_format(camera_fb->format);
    fb->timestamp.tv_sec = camera_fb->timestamp.tv_sec;
    fb->timestamp.tv_usec = camera_fb->timestamp.tv_usec;

    memcpy(fb->buf, camera_fb->buf, camera_fb->len);
    esp_camera_fb_return(camera_fb);

    return ESP_OK;
}

esp_err_t reconf_camera(const espfsp_cam_config_t *cam_config)
{
    esp_err_t ret = ESP_OK;

    sensor_t *s = esp_camera_sensor_get();
    if (s == NULL)
    {
        ret = ESP_FAIL;
        ESP_LOGE(TAG, "Camera get sensor failed");
    }
    if (ret == ESP_OK)
    {
        int res = 0;
        int quality = cam_config->cam_jpeg_quality > 63 ?
                        63 :
                        cam_config->cam_jpeg_quality < 0 ?
                            0 :
                            cam_config->cam_jpeg_quality;

        res = s->set_quality(s, quality);
        if (res < 0)
        {
            ret = ESP_FAIL;
            ESP_LOGE(TAG, "Camera set quality failed");
        }
    }

    return ret;
}

esp_err_t reconf_frame(const espfsp_frame_config_t *frame_config)
{
    esp_err_t ret = ESP_OK;

    sensor_t *s = esp_camera_sensor_get();
    if (s == NULL)
    {
        ret = ESP_FAIL;
        ESP_LOGE(TAG, "Camera get sensor failed");
    }
    if (ret == ESP_OK)
    {
        int res = 0;

        pixformat_t pixformat = transform_to_camera_pixel_format(frame_config->pixel_format);
        framesize_t framesize = transform_to_camera_frame_size(frame_config->frame_size);

        res = s->set_pixformat(s, pixformat);
        if (res < 0)
        {
            ret = ESP_FAIL;
            ESP_LOGE(TAG, "Camera set pixel format failed");
        }
        res = s->set_framesize(s, framesize);
        if (res < 0)
        {
            ret = ESP_FAIL;
            ESP_LOGE(TAG, "Camera set frame size failed");
        }
    }

    return ret;
}
