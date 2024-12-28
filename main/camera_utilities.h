/*
 * Home monitoring system
 * Author: Maksymilian Komarnicki
 */

#pragma once

#include "streamer_camera.h"

typedef int esp_err_t;

esp_err_t start_camera(const streamer_hal_config_t *stream_config);
esp_err_t stop_camera();
esp_err_t send_camera_frame(stream_fb_t *stream_fb);
