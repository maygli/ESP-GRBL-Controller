#ifndef GRBL_HTTP_UPLOAD_FILE_H
#define GRBL_HTTP_UPLOAD_FILE_H

#include <esp_err.h>
#include <esp_http_server.h>

#include <mpp_file_uploader.h>

#define MAX_BOUND_SIZE 256 

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t GRBL_ProcessFileUpload(httpd_req_t *req, FileUploader* theFileUploader, char* theBuff, int theBuffSize);
char* GRBL_GetBound(httpd_req_t *req, char* theBuffer, int* theBoundSize);

esp_err_t GRBL_UploadFile(httpd_req_t *req);

#ifdef __cplusplus
}
#endif

#endif