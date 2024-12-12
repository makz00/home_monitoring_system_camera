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

#include "esp_err.h"
#include "esp_log.h"

#include <arpa/inet.h>
#include "esp_netif.h"
#include <netdb.h>
#include <sys/socket.h>

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_camera.h"

#include "udp_streamer_handler_sender.h"
#include "udp_streamer_handler_types.h"

static const char *TAG = "UDP_STREAMER_CAMERA_COMPONENT_SENDER";

extern QueueHandle_t xQueue;

static int send_all(int sock, u_int8_t *buffer, size_t n, struct sockaddr_in *dest_addr)
{
    size_t n_left = n;
    while (n_left > 0)
    {
        ssize_t bytes_sent = sendto(sock, buffer, n_left, 0, (struct sockaddr *)dest_addr, sizeof(*dest_addr));
        if (bytes_sent < 0)
        {
            if (errno == ENOMEM)
            {
                continue;
            }

            return -1;
        }
        n_left -= bytes_sent;
        buffer += bytes_sent;
    }

    return 1;
}

static int send_whole_message(int sock, camera_fb_t *fb, struct sockaddr_in *dest_addr)
{
    udps_message_t message = {
        .len = fb->len,
        .width = fb->width,
        .height = fb->height,
        .timestamp.tv_sec = fb->timestamp.tv_sec,
        .timestamp.tv_usec = fb->timestamp.tv_usec,
        .msg_total = (fb->len / MESSAGE_BUFFER_SIZE) + (fb->len % MESSAGE_BUFFER_SIZE > 0 ? 1 : 0)};

    for (size_t i = 0; i < fb->len; i += MESSAGE_BUFFER_SIZE)
    {
        int bytes_to_send = i + MESSAGE_BUFFER_SIZE <= fb->len ? MESSAGE_BUFFER_SIZE : fb->len - i;

        message.msg_number = i / MESSAGE_BUFFER_SIZE;
        message.msg_len = bytes_to_send;
        memcpy(message.buf, fb->buf + i, bytes_to_send);

        int err = send_all(sock, (u_int8_t *)&message, sizeof(udps_message_t), dest_addr);
        if (err < 0)
        {
            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
            return -1;
        }

        // Forced delay as receiver side cannot handle a lot of Frame Buffers in short time
        const TickType_t xDelayMs = pdMS_TO_TICKS(10UL);
        vTaskDelay(xDelayMs);
    }

    return 1;
}

static void process_sender_connection(int sock, struct sockaddr_in *dest_addr)
{
    while (1)
    {
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb)
        {
            ESP_LOGE(TAG, "Camera Capture Failed");
            continue;
        }

        int ret = send_whole_message(sock, fb, dest_addr);

        esp_camera_fb_return(fb);

        if (ret < 0)
        {
            ESP_LOGE(TAG, "Cannot send message. End sender process");
            break;
        }
    }
}

void udp_streamer_camera_sender_task(void *pvParameters)
{
    stream_receiver_addr_t receiver_addr;
    BaseType_t xStatus;

    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;

    char addr_str[128];

    while (1)
    {
        ESP_LOGI(TAG, "Waiting for host to send data...");

        xStatus = xQueueReceive(xQueue, &receiver_addr, portMAX_DELAY);
        if (xStatus != pdPASS)
        {
            ESP_LOGE(TAG, "Could not receive address from the queue");
            continue;
        }

        struct sockaddr_in dest_addr;
        dest_addr.sin_addr = receiver_addr.stream_receiver_ip;
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = receiver_addr.stream_receiver_port;

        while (1)
        {
            int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
            if (sock < 0)
            {
                ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
                continue;
            }

            inet_ntoa_r(receiver_addr.stream_receiver_ip, addr_str, sizeof(addr_str) - 1);
            int host_port = ntohs(receiver_addr.stream_receiver_port);
            ESP_LOGI(TAG, "Socket created, sending to %s:%d", addr_str, host_port);

            process_sender_connection(sock, &dest_addr);

            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }

    vTaskDelete(NULL);
}
