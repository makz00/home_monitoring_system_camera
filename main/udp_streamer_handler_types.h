/*
 * Home monitoring system
 * Author: Maksymilian Komarnicki
 */

#pragma once

#include <sys/time.h>
#include <stdint.h>
#include "lwip/sockets.h"
#include <freertos/task.h>

#define MESSAGE_BUFFER_SIZE 1400

typedef struct
{
    size_t len;               /*!< Length of the buffer in bytes */
    size_t width;             /*!< Width of the buffer in pixels */
    size_t height;            /*!< Height of the buffer in pixels */
    struct timeval timestamp; /*!< Timestamp since boot of the first DMA buffer of the frame */
    int msg_total;
    int msg_number;
    int msg_len;
    uint8_t buf[MESSAGE_BUFFER_SIZE];
} udps_message_t;

typedef struct
{
    TaskHandle_t sender_task_handle;
    TaskHandle_t control_task_handle;
} udps_camera_state_t;

typedef struct
{
    struct in_addr stream_receiver_ip;
    in_port_t stream_receiver_port;
} stream_receiver_addr_t;
