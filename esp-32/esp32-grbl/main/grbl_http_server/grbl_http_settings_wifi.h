#ifndef GRBL_HTTP_SETTINGS_WIFI_H
#define GRBL_HTTP_SETTINGS_WIFI_H

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t GRBL_SendSettingsWiFi(httpd_req_t *req);
esp_err_t GRBL_SetSettingsWiFi(httpd_req_t *req);

#ifdef __cplusplus
}
#endif

#endif