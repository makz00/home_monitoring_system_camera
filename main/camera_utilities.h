/*
 * Home monitoring system
 * Author: Maksymilian Komarnicki
 */

#pragma once

#include "espfsp_config.h"

typedef int esp_err_t;

esp_err_t init_config(const espfsp_cam_config_t *cam_config, const espfsp_frame_config_t *frame_config);

esp_err_t start_camera(const espfsp_cam_config_t *cam_config, const espfsp_frame_config_t *frame_config);
esp_err_t stop_camera();
esp_err_t send_camera_frame(espfsp_fb_t *fb, espfsp_send_frame_cb_state_t *state, uint32_t max_allowed_size);
esp_err_t reconf_camera(const espfsp_cam_config_t *cam_config);
