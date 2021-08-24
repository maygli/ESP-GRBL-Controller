import { BaseController } from "./controllers/base_controller.js";
import {PageController} from "./controllers/page_controller.js"
import {UploadDialogController} from "./controllers/upload_dialog_controller.js"

class SDController extends PageController{
    constructor(theEl){
        super(theEl);
    }

    async _upadteAfterLoad(){
        super._upadteAfterLoad();
        let anUploadEl = this._m_BaseElement.querySelector('#upload_progress');
        this._m_UploadDialogController = new UploadDialogController(anUploadEl);
        this._m_UploadDialogController.hide();
        let aSDWarnEl = this._m_BaseElement.querySelector("#no_sd_panel");
        this._m_SDWarnController = new BaseController(aSDWarnEl);
        this._m_SDWarnController.hide();
    }

}


export {SDController};

/*function displayElement(theId, isDisplay)
{
    anEl = document.getElementById(theId);
    if( isDisplay ){
        anEl.style.display = "block";
    }
    else{
        anEl.style.display = "none"
    }
}


function displayNoSDWarning(isDisplay)
{
    displayElement("no_sd_warning",isDisplay);
    displayElement("upload_progress",!isDisplay);
    displayElement("modal",isDisplay);
}

function displayUploadProgress(isDisplay)
{
    displayElement("no_sd_warning",!isDisplay);
    displayElement("upload_progress",isDisplay);
    displayElement("modal",isDisplay);
}

function buildPath(theSender)
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

async function onTimer()
{
    aFolderData = await HttpProcessor.loadData("filesystem",false,{"path":"/"});
    if( aFolderData["mounted"]){
        displayNoSDWarning(false);
    }
    else{
        displayNoSDWarning(true);       
    }
    aResEl = document.getElementById("sd_free_space");
    aResEl.innerHTML = "Total size: " + printBytes(aFolderData["volume_size"]) + "/Free space: " + printBytes(aFolderData["free_space"]);
} 

*/