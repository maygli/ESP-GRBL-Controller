idf_component_register(
    SRCS   "grbl_utils.c" "grbl_config_ini.c" "grbl_http_server/grbl_http_sd.c" "grbl_http_server/grbl_http_settings_grbl.c" "grbl_http_server/grbl_http_settings_wifi.c" "grbl_http_server/grbl_http_board_info.c" "grbl_http_server/grbl_http_upload_file.c" "grbl_http_server/grbl_http_get_file.c" "grbl_http_server/grbl_http_utils.c" "grbl_http_server/grbl_default_page.c" "grbl_updater.c" "grbl_sd_card_manager.c" "grbl_http_server/grbl_http_server.c" "grbl_file_system.c" "grbl_config_manager.c" "grbl_wifi.c" "main.c"
    INCLUDE_DIRS "" "grbl_http_server" "../"
    REQUIRES "inih" "uri_decoder" "mpp_parser" "json" "esp_http_server" "vfs" "spi_flash" "fatfs" "nvs_flash"
)
