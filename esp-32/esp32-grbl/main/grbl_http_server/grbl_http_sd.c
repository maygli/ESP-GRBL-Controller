#include <esp_err.h>
#include <esp_http_server.h>
#include <esp_log.h>
#include <dirent.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <ff.h>

#include <uri_encode.h>

#include <cJSON.h>

#include <mpp_file_uploader.h>

#include "grbl_http_server.h"
#include "grbl_http_utils.h"
#include "grbl_http_upload_file.h"
#include "../grbl_board_default_config.h"
#include "../grbl_file_system.h"

#define PATH_TAG "path"
#define NEW_PATH_TAG "new_path"
#define NAME_TAG "\"name\":\""

#define IS_FILE_TAG "\",\"is_file\": "
#define FALSE_TAG "false"
#define TRUE_TAG "true"

#define SIZE_TAG ",\"size\": "

//We have to use FatFS instead of VFS due to neccessary to filter hidden and system files
#define SD_VOLUME "1:"

#define FILE_BUFFER_OFFSET (MAX_PATH_SIZE*2)

typedef struct _UploadContextSD{
    GRBLServer*    m_Server;
    FILE*          m_SavedFile;
    char           m_Path[MAX_PATH_SIZE];
} UploadContextSD;

void grbl_InitUploadContextSD(UploadContextSD* theContext, GRBLServer* theServer)
{
    theContext->m_Server = theServer;
    theContext->m_SavedFile = NULL;
    theContext->m_Path[0] = 0;
}

int grbl_OpenFileSD(char* theFileName, int theNameSize, void* theContext)
{
    int  aPathLen;
    UploadContextSD* aContext = (UploadContextSD*)theContext;
    aPathLen = strlen(aContext->m_Path);
    if( (aPathLen + theNameSize) > (MAX_PATH_SIZE-2) ){
        ESP_LOGE(TAG,"Upload file error. Full path too long");
        return -1;
    }
    aContext->m_Path[aPathLen] = '/';
    strncpy(aContext->m_Path+aPathLen+1,theFileName,theNameSize);
    aContext->m_Path[aPathLen+1+theNameSize] = 0;
    ESP_LOGI(TAG,"Open file %s", aContext->m_Path);
    aContext->m_SavedFile = fopen(aContext->m_Path,"w");
    return 0;
}

int grbl_GetParameterSD(char* theName, int theNameSize, char* theData, int theDataSize,  void* theContext)
{
    UploadContextSD* aContext = (UploadContextSD*)theContext;
    if( strncmp(theName, PATH_TAG, theNameSize) == 0 ){
        if( theDataSize > (MAX_PATH_SIZE-1) ){
            ESP_LOGE(TAG,"Upload file error. Path is too long");
            return -1;
        }
        strncpy(aContext->m_Path, theData, theDataSize);
        aContext->m_Path[theDataSize] = 0;
        ESP_LOGI(TAG,"Get path='%s'", aContext->m_Path);
    }
    return 0;
}

int grbl_WriteFileDataSD(char* theDataPtr, int theLen, void* theContext)
{
    UploadContextSD* aContext = (UploadContextSD*)theContext;
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

char* grbl_GetFileName(FILINFO* theFileInfo)
{
    if( theFileInfo->fname){
        return theFileInfo->fname;
    }
    return theFileInfo->altname;
}

char* grbl_GetPath(char* theURI)
{
    char* aPtr=theURI;
    while( (*aPtr) != 0 ){
        if( (*aPtr) == '=' ){
            aPtr++;
            break;
        }
        aPtr++;
    }
    return aPtr;
}

char* grbl_GetJSONStringValue(httpd_req_t *req, cJSON* theJson, char* theItem)
{
    const cJSON*    aPathItem = NULL;
    aPathItem = cJSON_GetObjectItemCaseSensitive(theJson,theItem);
    if( !aPathItem ){
        ESP_LOGE(TAG, "GRBL_HttpRemoveFile:Can't find path item");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "GRBL_HttpRemoveFile:Can't find path item in content");
        cJSON_Delete(theJson);
        return NULL;
    }
    if(!aPathItem->valuestring){
        ESP_LOGE(TAG, "GRBL_HttpRemoveFile:Path is NULL");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "GRBL_HttpRemoveFile:Path is NULL");
        cJSON_Delete(theJson);
        return NULL;
    }
    return aPathItem->valuestring;
}

esp_err_t GRBL_UploadSDFile(httpd_req_t *req)
{
    ESP_LOGI(TAG,"POST content_len=%d uri='%s'", req->content_len, req->uri);
    int             aRes;
    char            aBoundStr[MAX_BOUND_SIZE];
    int             aBoundSize;
    FileUploader    aFileUploader;
    UploadContextSD anUploadContext;
    
    GRBLServer* aServer = (GRBLServer*)req->user_ctx;
    char *aBuff = aServer->m_FileBuffer;

    grbl_InitUploadContextSD(&anUploadContext,aServer);

    char* aBound = GRBL_GetBound(req,aBoundStr,&aBoundSize);

    MPP_FileUploaderInit(&aFileUploader,aBound,aBoundSize,&anUploadContext);
    aFileUploader.cb_OpenFile = &grbl_OpenFileSD;
    aFileUploader.cb_GetParameter = &grbl_GetParameterSD;
    aFileUploader.cb_WriteData = &grbl_WriteFileDataSD;

    aRes = GRBL_ProcessFileUpload(req,&aFileUploader,aBuff,HTTP_BUFFER_SIZE);
    if( anUploadContext.m_SavedFile != NULL ){
        fclose(anUploadContext.m_SavedFile);
    }
    if( aRes != ESP_OK ){
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error while uploading file");
        return aRes;
    }
    httpd_resp_sendstr(req, "File successfully uploaded");

    ESP_LOGI(TAG, "File reception complete");    
    return ESP_OK;
}

esp_err_t GRBL_HttpRename(httpd_req_t *req)
{
    cJSON*          aJson = NULL;
    char*           aPath = NULL;
    char*           aNewPath = NULL;
    esp_err_t       aRes;
    aJson = GRBL_ReceiveJSON(req);
    if( aJson == NULL ){
        return ESP_FAIL;
    }
    aPath = grbl_GetJSONStringValue(req,aJson,PATH_TAG);
    if( aPath == NULL ){
        cJSON_Delete(aJson);
        return ESP_FAIL;
    }
    aNewPath = grbl_GetJSONStringValue(req,aJson,NEW_PATH_TAG);
    if( aNewPath == NULL ){
        cJSON_Delete(aJson);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG,"Path=%s, NewPath=%s",aPath,aNewPath);
    aRes = rename(aPath, aNewPath);
    if( aRes != ESP_OK ){
        ESP_LOGE(TAG, "Can't rename a file or folder");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Can't rename file");
        cJSON_Delete(aJson);
    }    
    cJSON_Delete(aJson);
    httpd_resp_sendstr(req, "File renamed");
    return ESP_OK;

} 

esp_err_t GRBL_HttpCreateNewFolder(httpd_req_t *req)
{
    cJSON*          aJson = NULL;
    char*           aPath = NULL;
    esp_err_t       aRes;
    aJson = GRBL_ReceiveJSON(req);
    if( aJson == NULL ){
        return ESP_FAIL;
    }
    aPath = grbl_GetJSONStringValue(req,aJson,PATH_TAG);
    if( aPath == NULL ){
        cJSON_Delete(aJson);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG,"Path=%s",aPath);
    aRes = mkdir(aPath,0777);
    if( aRes != ESP_OK ){
        ESP_LOGE(TAG, "Can't create folder");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Can't create folder");
        cJSON_Delete(aJson);
    }    
    cJSON_Delete(aJson);
    httpd_resp_sendstr(req, "Folder created");
    return ESP_OK;
}

esp_err_t GRBL_HttpRemoveFile(httpd_req_t *req)
{
    cJSON*          aJson = NULL;
    char*           aPath = NULL;
    esp_err_t       aRes;
    struct stat     aStat;
    aJson = GRBL_ReceiveJSON(req);
    if( aJson == NULL ){
        return ESP_FAIL;
    }
    aPath = grbl_GetJSONStringValue(req,aJson,PATH_TAG);
    if( aPath == NULL ){
        cJSON_Delete(aJson);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG,"Path=%s",aPath);
    aRes = stat(aPath, &aStat);
    if( aRes != ESP_OK ){
        ESP_LOGE(TAG,"GRBL_HttpRemoveFile:Can't get statistic");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "GRBL_HttpRemoveFile:Can't get statistic");
        cJSON_Delete(aJson);
        return ESP_FAIL;
    }
    if( S_ISREG(aStat.st_mode) ){
        aRes = unlink(aPath);
        if( aRes != ESP_OK ){
            ESP_LOGE(TAG,"GRBL_HttpRemoveFile:Can't remove file");
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "GRBL_HttpRemoveFile:Can't remove file");
            cJSON_Delete(aJson);
            return ESP_FAIL;
        }
    }
    else if( S_ISDIR(aStat.st_mode) ){
        aRes = GRBL_RmTree(aPath); 
        if( aRes != ESP_OK ){
            ESP_LOGE(TAG,"GRBL_HttpRemoveFile:Can't remove folder");
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "GRBL_HttpRemoveFile:Can't remove folder");
            cJSON_Delete(aJson);
            return ESP_FAIL;
        }
    }
    cJSON_Delete(aJson);
    httpd_resp_sendstr(req, "File removed");
    ESP_LOGI(TAG,"File removed");
    return ESP_OK;
}

esp_err_t GRBL_HttpSendSDData(httpd_req_t *req)
{
    bool isFirst = true;
    FRESULT aRes;
    FF_DIR  aDir;
    FILINFO aFileInfo;
    char*   aName;
    GRBLServer* aServer = (GRBLServer*)req->user_ctx;
    char* aRealPath = aServer->m_FileBuffer;
    char* aBuffer = aServer->m_FileBuffer + FILE_BUFFER_OFFSET;
    strcpy(aRealPath,SD_VOLUME);
    ESP_LOGI(TAG,"GRBL_HttpSendSDData URI=%s",req->uri);
    uri_decode(req->uri,strlen(req->uri),aBuffer);
    ESP_LOGI(TAG,"Decoded URI='%s'",aBuffer);
    char* aPath = grbl_GetPath(aBuffer);
    ESP_LOGI(TAG,"Requested path '%s'", aPath);
    strcpy(aRealPath+strlen(SD_VOLUME),aPath);
    ESP_LOGI(TAG,"Real path=%s",aRealPath);
    httpd_resp_set_type(req, "application/json");

    aRes = f_opendir(&aDir, aRealPath);
    if(aRes != FR_OK) {
        ESP_LOGE(TAG,"GRBL_HttpSendSDData: Can't open dir '%s'", aRealPath);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Can't open folder");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG,"f_opendir() success");

    aBuffer[0] = '[';
    aBuffer[1] = 0;
    httpd_resp_send_chunk(req, aBuffer, 1);    

    for(;;) {
        aRes = f_readdir(&aDir, &aFileInfo);
        if((aRes != FR_OK) || (aFileInfo.fname[0] == '\0')) {
            break;
        }
        aName = grbl_GetFileName(&aFileInfo);       
        if( ( aFileInfo.fattrib & AM_HID) || (aFileInfo.fattrib & AM_SYS) ){
            ESP_LOGI(TAG,"Found hidden or sys file=%s",aName);
            continue;            
        }
        if( !isFirst ){
            aBuffer[0] = ',';
            httpd_resp_send_chunk(req, aBuffer, 1);
        }
        isFirst = false;
        aBuffer[0] = '{';
        httpd_resp_send_chunk(req, aBuffer, 1);
        httpd_resp_send_chunk(req,NAME_TAG,strlen(NAME_TAG));
        httpd_resp_send_chunk(req,aName,strlen(aName));    
        httpd_resp_send_chunk(req,IS_FILE_TAG,strlen(IS_FILE_TAG));    
        if(aFileInfo.fattrib & AM_DIR) {
            httpd_resp_send_chunk(req,FALSE_TAG,strlen(FALSE_TAG));    
        } else {
            httpd_resp_send_chunk(req,TRUE_TAG,strlen(TRUE_TAG));    
        }
        httpd_resp_send_chunk(req,SIZE_TAG,strlen(SIZE_TAG));
        sprintf(aBuffer,"%d",aFileInfo.fsize);    
        httpd_resp_send_chunk(req,aBuffer,strlen(aBuffer));
        aBuffer[0] = '}';
        httpd_resp_send_chunk(req, aBuffer, 1);
    }
    f_closedir(&aDir);
    aBuffer[0] = ']';    
    httpd_resp_send_chunk(req, aBuffer, 1);    
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
