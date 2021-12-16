#include <stdio.h>
#include <esp_log.h>
#include <sys/param.h>

#include "grbl_http_upload_file.h"
#include "grbl_http_server.h"
#include "grbl_http_utils.h"
#include "mpp_file_uploader.h"

typedef struct _UploadContext{
    GRBLServer*    m_Server;
    FILE*          m_SavedFile;
    bool           m_isClear;
} UploadContext;

void grbl_InitUploadContext(UploadContext* theContext, GRBLServer* theServer)
{
    theContext->m_Server = theServer;
    theContext->m_SavedFile = NULL;
    theContext->m_isClear = false;
}

int grbl_OpenFile(char* theFileName, int theNameSize, void* theContext)
{
    char aFilePath[MAX_PATH_SIZE];
    int  aPathLen;
    UploadContext* aContext = (UploadContext*)theContext;
/*TODO temporary - set correct base path */     
    strcpy(aFilePath,SD_BASE_PATH);
    aPathLen = strlen(aFilePath);
    aFilePath[aPathLen] = '/';
    aPathLen++;
    strncpy(aFilePath + aPathLen, theFileName, theNameSize);
    aFilePath[aPathLen+theNameSize] = 0;
    ESP_LOGI(TAG,"Open file %s", aFilePath);
    aContext->m_SavedFile = fopen(aFilePath,"w");
    return 0;
}

int grbl_GetParameter(char* theName, int theNameSize, char* theData, int theDataSize,  void* theContext)
{
    UploadContext* aContext = (UploadContext*)theContext;
    return 0;
}

int grbl_WriteFileData(char* theDataPtr, int theLen, void* theContext)
{
    UploadContext* aContext = (UploadContext*)theContext;
    if( aContext->m_SavedFile != NULL ){
        int aWriteCnt = fwrite(theDataPtr, 1, theLen, aContext->m_SavedFile);
        if( aWriteCnt != theLen ){
            ESP_LOGE(TAG,"Error write to uplaoded file. Try to write %d bytes, but actually %d\n", theLen, aWriteCnt);
            return -1;
        }
        return 0;
    }
    ESP_LOGE(TAG,"Error write to uplaoded file. File is not open");
    return -1;
}

char* GRBL_GetBound(httpd_req_t *req, char* theBuffer, int* theBoundSize)
{
    char*       aBound = NULL;
    esp_err_t   aRes;
    size_t aLen = httpd_req_get_hdr_value_len(req,"Content-Type");
    if( aLen > (MAX_BOUND_SIZE-1) ){
        ESP_LOGE(TAG,"Content-Type header is too long");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive file");
        return NULL;
    }
    ESP_LOGI(TAG,"POST Content-Type_len=%d", aLen);
    aRes = httpd_req_get_hdr_value_str(req,"Content-Type", theBuffer, MAX_BOUND_SIZE-1);
    if( aRes != ESP_OK ){
        ESP_LOGE(TAG,"Can't get header string");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive file");
        return NULL;
    }
    theBuffer[aLen] = 0;
    ESP_LOGI(TAG,"POST Content-Type_val=%s", theBuffer);
    aBound = MPP_GetBoundary(theBuffer,theBoundSize);
    aBound[*theBoundSize] = 0;
    ESP_LOGI(TAG,"POST Boundary=%s", aBound);
    return aBound;
}

esp_err_t GRBL_ProcessFileUpload(httpd_req_t *req, FileUploader* theFileUploader, char* theBuff, int theBuffSize)
{
    int aRemaining = req->content_len;
    int aReceived = 0;
    int aRes;
    while (aRemaining > 0) {

 //       ESP_LOGI(TAG, "Remaining size : %d", aRemaining);
        /* Receive the file part by part into a buffer */
        if ((aReceived = httpd_req_recv(req, theBuff, MIN(aRemaining, theBuffSize-1))) <= 0) {
            if (aReceived == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry if timeout occurred */
                continue;
            }
            /* In case of unrecoverable error,
             * close and delete the unfinished file*/
//            fclose(fd);
//            unlink(filepath);

            ESP_LOGE(TAG, "File upload failed!");
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to upload file");
            return ESP_FAIL;
        }
        theBuff[aReceived] = 0;
        printf(theBuff);
        printf("\n===================\n");
        aRes = MPP_FileUploaderProcess(theFileUploader,theBuff,aReceived);
        if( aRes != 0 ){
            ESP_LOGE(TAG, "Error process aReceived data");
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to upload file");
            return ESP_FAIL;
        }
        aRemaining -= aReceived;
    }
    return ESP_OK;
}

esp_err_t GRBL_UploadFile(httpd_req_t *req)
{
    ESP_LOGI(TAG,"POST content_len=%d uri='%s'", req->content_len, req->uri);
    int     aRes;
    char    aBoundStr[MAX_BOUND_SIZE];
    int     aBoundSize;
    FileUploader  aFileUploader;
    UploadContext anUploadContext;
    
    GRBLServer* aServer = (GRBLServer*)req->user_ctx;
    char *aBuff = aServer->m_FileBuffer;

    grbl_InitUploadContext(&anUploadContext,aServer);

    char* aBound = GRBL_GetBound(req,aBoundStr,&aBoundSize);

    MPP_FileUploaderInit(&aFileUploader,aBound,aBoundSize,&anUploadContext);
    aFileUploader.cb_OpenFile = &grbl_OpenFile;
    aFileUploader.cb_GetParameter = &grbl_GetParameter;
    aFileUploader.cb_WriteData = &grbl_WriteFileData;

    aRes = GRBL_ProcessFileUpload(req,&aFileUploader,aBuff,HTTP_BUFFER_SIZE);
    if( anUploadContext.m_SavedFile != NULL ){
        fclose(anUploadContext.m_SavedFile);
    }
    if( aRes != ESP_OK ){
        return aRes;
    }
    httpd_resp_sendstr(req, "File successfully uploaded");

    ESP_LOGI(TAG, "File reception complete");    
    return ESP_OK;
}