idf_component_register(
    SRCS
    "mdns_handler.c"
    "main.c"
    "udps_handler.c"
    "camera_handler.c"
    "wifi_handler.c"
    "udp_streamer_handler.c"
    "udp_streamer_handler_sender.c"
    "udp_streamer_handler_controler.c"
    "mdns_handler.c"
    PRIV_REQUIRES spi_flash nvs_flash esp_wifi esp_timer
    INCLUDE_DIRS "")
