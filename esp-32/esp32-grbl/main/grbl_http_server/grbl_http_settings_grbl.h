#ifndef GRBL_HTTP_SETTINGS_GRBL_H
#define GRBL_HTTP_SETTINGS_GRBL_H

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t GRBL_SendSettingsGrbl(httpd_req_t *req);
esp_err_t GRBL_SetSettingsGrbl(httpd_req_t *req);

#ifdef __cplusplus
}
#endif

#endif