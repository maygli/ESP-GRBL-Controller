import { BaseController } from "./controllers/base_controller.js";
import {PageController} from "./controllers/page_controller.js"
import {UploadDialogController} from "./controllers/upload_dialog_controller.js"
import {HttpProcessor} from "./http_processor.js"
import {GUIUtils} from "./gui_utils.js"
import {FileViewerController} from "./controllers/file_viewer_controller.js"
import {PathController} from "./controllers/path_controller.js"
import {DialogController} from "./controllers/dialog_controller.js"
import {RenameFileDialog} from "./rename_file_dlg.js"

class SDController extends PageController{
    constructor(theEl){
        super(theEl);
    }

    async _upadteAfterLoad(){
        super._upadteAfterLoad();
        let anUploadEl = this._m_BaseElement.querySelector('#upload_progress');
        this._m_UploadDialogController = new UploadDialogController(anUploadEl);
        this._m_UploadDialogController.hide();
//Should be called after _m_UploadDialogController.hide() to avoid call refresh        
        this._m_UploadDialogController.setCloseDialogCallback(this._onUploadedComplete.bind(this));
        let aSDWarnEl = this._m_BaseElement.querySelector("#no_sd_panel");
        this._m_SDWarnController = new BaseController(aSDWarnEl);
        this._m_SDWarnController.hide();
        this._m_TotalSpaceEl = this._m_BaseElement.querySelector("#total_size");
        this._m_FreeSpaceEl = this._m_BaseElement.querySelector("#free_space");
        this._m_UploadFileBtn = this._m_BaseElement.querySelector("#upload_file_button");
        this._m_UploadFileBtn.addEventListener("click", this._onUploadFileClick.bind(this));
        this._m_AddFolderBtn = this._m_BaseElement.querySelector("#add_folder_button");
        this._m_RemoveBtn = this._m_BaseElement.querySelector("#remove_button");
        this._m_RemoveBtn.addEventListener("click", this._onRemoveClick.bind(this));
        this._m_RenameBtn = this._m_BaseElement.querySelector("#rename_button");
        this._m_RenameBtn.addEventListener("click", this._onRenameClick.bind(this));
        let anAddNewFolderBtn = this._m_BaseElement.querySelector("#add_folder_button");
        anAddNewFolderBtn.addEventListener("click", this._onAddNewFolder.bind(this));
        this._m_FileInpuField = this._m_BaseElement.querySelector("#file_upload");
        this._m_FileInpuField.addEventListener("change", this._onUploadFile.bind(this)); 
        this._m_FileSelector = this._m_BaseElement.querySelector("#file_selector_table");
        this._m_FileSelectorController = new FileViewerController(this._m_FileSelector);
        this._m_FileSelectorController.setRoot("");
        this._m_FileSelectorController.setFolderDblClickCallback(this._buildFS.bind(this));
        this._m_FileSelectorController.setSelectionChangeCallback(this._onSelectionChanged.bind(this));
        let aPathEl = this._m_BaseElement.querySelector("#path_control");
        this._m_PathController = new PathController(aPathEl);
        this._m_PathController.setRoot("/SD");
        this._m_PathController.setClickCallback(this._buildFS.bind(this));
        let aRenameEl = this._m_BaseElement.querySelector("#rename_dialog");
        this._m_RenameDialog = new RenameFileDialog(aRenameEl);
        this._m_RenameDialog.setRenameCallcback(this._onRename.bind(this));
        this._m_RenameDialog.hide();
        let aNewFolderEl = this._m_BaseElement.querySelector("#new_foler_dialog");
        this._m_NewFolderDialog = new RenameFileDialog(aNewFolderEl);
        this._m_NewFolderDialog.setRenameCallcback(this._onNewFolder.bind(this));
        this._m_NewFolderDialog.hide();
        this._buildFS("");
        this._onSelectionChanged();
     }

    setDataUpdater(theProvider){
        this._m_Updater = theProvider;
        let aParam = new Object();
        aParam.callback = this._onSDData.bind(this);
        this._m_Updater.registerDataReq("sd_card", aParam);
    }

    _onSDData(theData){
        if( theData.mounted ){
            if( this._m_SDWarnController ){
                this._m_SDWarnController.hide();
                let aTotalSize = GUIUtils.printBytes(theData.volume_size);
                this._m_TotalSpaceEl.innerHTML = aTotalSize;
                let aFreeSpace = GUIUtils.printBytes(theData.free_space);
                this._m_FreeSpaceEl.innerHTML = aFreeSpace;
            }
        }
        else{
            if( this._m_SDWarnController ){
                this._m_SDWarnController.show();
            }
        }
    }

    async _onRemoveClick(){
        let aSelFileInfo = this._m_FileSelectorController.getSelectedFileInfo();
        if( !aSelFileInfo || aSelFileInfo.is_up )
            return;
        let aPath = this._m_PathController.getAbsolutePath() + "/" + aSelFileInfo.name;
        let aSendObj = new Object();
        aSendObj.path = aPath;
        await HttpProcessor.sendPostJSON("/remove_file",aSendObj);
        await this._refresh();
    }

    _onAddNewFolder(){
         let anEntities = this._m_FileSelectorController.getFilesOrFolersNames(false);
        this._m_NewFolderDialog.setExisted(anEntities);
        this._m_NewFolderDialog.show();
//Should call after show to select text        
        this._m_NewFolderDialog.setFile("Folder");
    }

    _onRenameClick(){
        let aSelFileInfo = this._m_FileSelectorController.getSelectedFileInfo();
        if( !aSelFileInfo || aSelFileInfo.is_up )
            return;
        let anEntities = this._m_FileSelectorController.getFilesOrFolersNames(aSelFileInfo.is_file);
        this._m_RenameDialog.setExisted(anEntities);
        let aPath = this._m_PathController.getAbsolutePath() + "/" + aSelFileInfo.name;
        this._m_RenameDialog.show();
//Should call after show to select text        
        this._m_RenameDialog.setFile(aPath);
    }

    async _onRename(theNewName){
        let aSelFileInfo = this._m_FileSelectorController.getSelectedFileInfo();
        let aPath = this._m_PathController.getAbsolutePath() + "/" + aSelFileInfo.name;
        let aNewPath = this._m_PathController.getAbsolutePath() + "/" + theNewName;
        let aSendObj = new Object();
        aSendObj.path = aPath;
        aSendObj.new_path = aNewPath;
        await HttpProcessor.sendPostJSON("/rename_file",aSendObj);
        await this._refresh();
        this._m_FileSelectorController.selectFile(theNewName, aSelFileInfo.is_file);
    }

    async _onNewFolder(theNewName){
        let aPath = this._m_PathController.getAbsolutePath();
        let aSendObj = new Object();
        aSendObj.path = aPath + "/" + theNewName;
        await HttpProcessor.sendPostJSON("/create_new_folder",aSendObj);
        await this._refresh();
        this._m_FileSelectorController.selectFile(theNewName, false);
    }

    async _onUploadFileClick(){
        this._m_FileInpuField.dispatchEvent(new MouseEvent("click")); 
    }

    _onUploadFile(){
        let aFile = this._m_FileInpuField.files[0];
        this._m_FileInpuField.value="";
        this._m_UploadDialogController.startUpload(aFile.name);
        if( aFile.name.length > 31 ){
            this._m_UploadDialogController.setError(i18next.t("settings_upgrade_name_exceed"));
            return false;
        }

        this._m_UploadedFile = aFile.name;
        let aParFormData = new FormData();
        let aPath = this._m_PathController.getAbsolutePath();
        aParFormData.append("path",aPath);
        HttpProcessor.uploadFile(aFile, "sd_upload", aParFormData, 
                                this._onLoadProgress.bind(this),
                                this._onLoadComplete.bind(this),
                                this._onLoadError.bind(this),
                                this._onLoadAbort.bind(this)
                                );
    }
 
    async _buildFS(thePath){
        let aParam = new Object();
        aParam.path=thePath;
        let aFSData;
        try{
             aFSData = await HttpProcessor.loadData("sd_filesystem_data", false, aParam);
        }
        catch{
            return;
        }
        this._m_FileSelectorController.setFiles(aFSData, thePath);
        this._m_PathController.setPath(thePath);
    }

    _onLoadComplete(event) {
        if(  event.target.status >= 200 && event.target.status < 300 ){
            this._m_UploadDialogController.setSuccess(event.target.responseText);
        }
        else{
            this._m_UploadDialogController.setError(event.target.responseText);
        }
    }
    
    _onSelectionChanged(){
        let aFileInfo = this._m_FileSelectorController.getSelectedFileInfo();
        if( !aFileInfo || aFileInfo.is_up ){
            this._m_RemoveBtn.style.display = "none";
            this._m_RenameBtn.style.display = "none";
        }
        else{
            this._m_RemoveBtn.style.display = "inline-block";
            this._m_RenameBtn.style.display = "inline-block";
        }
    }

    _onLoadProgress(event) {
        this._m_UploadDialogController.setProgress(event.loaded, event.total);
    }

    _onLoadError(event) {
        this._m_UploadDialogController.setError(i18next.t("settings_upgrade_failed"));
    }
    
    _onLoadAbort(event) {
        this._m_UploadDialogController.setError(i18next.t("settings_upgrade_aborted"));
    }

    async _onUploadedComplete(){
        await this._refresh();
        if( this._m_UploadedFile ){
            this._m_FileSelectorController.selectFile(this._m_UploadedFile, true);
        }
    }

    async _refresh(){
        let aPath = this._m_PathController.getPath();
        await this._buildFS(aPath);
    }
}


export {SDController};

/*function buildPath(theSender)
{
    aPath = null;
    anEl = theSender;
    while(anEl){
        if( aPath ){
            aPath = anEl.innerHTML + "/" + aPath;
        }
        else{
            aPath = anEl.innerHTML;
        }
        anEl = anEl.previousElementSibling;
    }
    loadFolder(aPath);
}

async function loadFolder(thePath)
{
    aFolderData = await HttpProcessor.loadData("filesystem",false,{"path":thePath});
}

*/