idf_component_register(SRCS "main.c" "udps_handler.c" "camera_handler.c" "wifi_handler.c" "udp_streamer_handler.c" "udp_streamer_handler_sender.c" "udp_streamer_handler_controler.c"
                    PRIV_REQUIRES spi_flash nvs_flash esp_wifi esp_timer
                    INCLUDE_DIRS "")
