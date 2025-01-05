/*
 * Home monitoring system
 * Author: Maksymilian Komarnicki
 */

#pragma once

#include "espfsp_config.h"

typedef int esp_err_t;

esp_err_t start_camera(const espfsp_cam_config_t *cam_config, const espfsp_frame_config_t *frame_config);
esp_err_t stop_camera();
esp_err_t send_camera_frame(espfsp_fb_t *fb);
esp_err_t reconf_camera(const espfsp_cam_config_t *cam_config);
esp_err_t reconf_frame(const espfsp_frame_config_t *frame_config);
