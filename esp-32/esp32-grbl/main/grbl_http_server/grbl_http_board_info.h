#ifndef GRBL_HTTP_BOARD_INFO_H
#define GRBL_HTTP_BOARD_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t GRBL_SendBoardInfo(httpd_req_t *req);

#ifdef __cplusplus
}
#endif

#endif