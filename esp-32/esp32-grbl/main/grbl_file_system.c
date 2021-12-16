#include <esp_log.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>

#include "grbl_file_system.h"
#include "grbl_board_default_config.h"

#define COPY_BUFFER_SIZE 4096

esp_err_t grbl_CopyFile(char* theSrcPath, char* theDstPath)
{
    FILE* aSrcFile = NULL;
    FILE* aDstFile = NULL;
    char aBuffer[COPY_BUFFER_SIZE];
    int aReadSize;
    int aWriteSize;
    esp_err_t aRes = ESP_OK;

    aSrcFile = fopen(theSrcPath,"r");
    if( !aSrcFile ){
        ESP_LOGE(TAG,"grbl_CopyFile::Can't open source file %s", theSrcPath);
        return ESP_FAIL;
    }
    aDstFile = fopen(theDstPath,"w");
    if( !aDstFile ){
        ESP_LOGE(TAG, "grbl_CopyFile::Can't open destination file %s", theDstPath);
        fclose(aSrcFile);
        return ESP_FAIL;
    }
    do{
        aReadSize = fread(aBuffer,1,COPY_BUFFER_SIZE, aSrcFile);
        if( aReadSize > 0 ){
            aWriteSize = fwrite(aBuffer,1,aReadSize,aDstFile);
            if( aWriteSize != aReadSize ){
                ESP_LOGE(TAG, "grbl_CopyFile::Read %d bytes, write %d bytes. Source file %s, Destination file %s", aReadSize, aWriteSize, theSrcPath, theDstPath);
                aRes = ESP_FAIL;
                break;
            }
        }
    } while(aReadSize > 0 );
    fclose(aSrcFile);
    fclose(aDstFile);
    return aRes;
}

void grbl_PathJoin(char* thePath, char* theName, char* theResult)
{
    if( thePath == NULL ){
        theResult[0] = 0;
    }
    else{
        strcpy(theResult, thePath);
    }
    int aSize = strlen(theResult);
    theResult[aSize] = '/';
    if( theName == NULL )
        return;
    strcpy(theResult + aSize+1,theName);
}

esp_err_t grbl_CopyFolder(char* theSrcPath, char* theDstPath)
{
    esp_err_t aRes;
    char aDstFullPath[MAX_PATH_SIZE];
    char aSrcFullPath[MAX_PATH_SIZE];

    aRes = mkdir(theDstPath,  0777);
    if( aRes != ESP_OK ){
        ESP_LOGE(TAG, "grbl_CopyFolder: Error %d create dir %s", errno, theDstPath);
        return aRes;
    }
    DIR* aDir = opendir(theSrcPath);
    if( !aDir ){
        ESP_LOGE(TAG,"GRBL_RmTree: Can't open dir %s", theSrcPath);
        return ESP_FAIL;
    }
    while(true){
        struct dirent* de = readdir(aDir);
        if( !de ){
            break;
        }
        ESP_LOGI(TAG," Found entity %s type=%d", de->d_name, de->d_type);
        grbl_PathJoin(theSrcPath, de->d_name, aSrcFullPath);
        grbl_PathJoin(theDstPath, de->d_name, aDstFullPath);
        if( de->d_type == DT_DIR ){
            aRes = grbl_CopyFolder(aSrcFullPath, aDstFullPath);
            if( aRes != ESP_OK ){
                ESP_LOGE(TAG,"Can't copy folder from %s to %s", aSrcFullPath, aDstFullPath);
                closedir(aDir);
                return aRes;
            } 
        }
        else{
            aRes = grbl_CopyFile(aSrcFullPath,aDstFullPath);
            if( aRes != ESP_OK ){
                ESP_LOGE(TAG,"Can't copy file from %s to %s", aSrcFullPath, aDstFullPath);
                closedir(aDir);
                return aRes;
            } 
        }
    }
    closedir(aDir);    
    return ESP_OK;
}

esp_err_t GRBL_FCopy(char* theSrcPath, char* theDstPath)
{
    esp_err_t aRes;
    struct stat aStat;
    aRes = stat(theSrcPath,&aStat);
    if( aRes != ESP_OK ){
        ESP_LOGE(TAG,"GRBL_FCopy: Can't get statistic for %s", theSrcPath);
        return ESP_FAIL;
    }
    if( S_ISREG(aStat.st_mode) ){
        return grbl_CopyFile(theSrcPath, theDstPath);
    }
    if( S_ISDIR(aStat.st_mode) ){
        aRes = stat(theDstPath, &aStat);
        if( aRes == ESP_OK ){
            aRes = GRBL_RmTree(theDstPath);
            if( aRes != ESP_OK ){
                ESP_LOGE(TAG,"GRBL_FCopy: Destination folder already exists. Can't remove. Dst folder %s", theDstPath);
                return ESP_FAIL;
            }
        }
        return grbl_CopyFolder(theSrcPath,theDstPath);
    }
    ESP_LOGE(TAG,"GRBL_FCopy: Unsopported file type %d",aStat.st_mode);
    return ESP_FAIL;
}

esp_err_t GRBL_RmTree(char* thePath)
{
    char aFullPath[MAX_PATH_SIZE];
    esp_err_t aRes;
    DIR* aDir = opendir(thePath);
    if( !aDir ){
        ESP_LOGE(TAG,"GRBL_RmTree: Can't open dir %s", thePath);
        return ESP_FAIL;
    }
    while(true){
        struct dirent* de = readdir(aDir);
        if( !de ){
            break;
        }
        grbl_PathJoin(thePath, de->d_name, aFullPath);
        if( de->d_type == DT_DIR ){
            aRes = GRBL_RmTree(aFullPath);
            if( aRes != ESP_OK ){
                ESP_LOGE(TAG,"GRBL_RmTree: Can't remove folder %s", aFullPath );
                closedir(aDir);
                return aRes;
            }
        }
        else{
            aRes = unlink(aFullPath);
            if( aRes != ESP_OK ){
                ESP_LOGE(TAG,"GRBL_RmTree: Can't remove file %s", aFullPath );
                closedir(aDir);
                return aRes;
            }
        }
        ESP_LOGI(TAG," Found entity %s type=%d", de->d_name, de->d_type);
    }
    closedir(aDir);
    return rmdir(thePath);
}
