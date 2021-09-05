import {ListController} from "./list_controller.js"
import {GUIUtils} from "../gui_utils.js"

class FileViewerController extends ListController{
    constructor(theEl){
        super(theEl);
        this._m_Path = "";
    }

    setRoot(theRoot){
        this._m_Root = theRoot;
    }

    setFolderDblClickCallback(theCallback){
        this._m_FolderDblClickCallback = theCallback;
    }

    setFiles(theFilesList, thePath, isRestoreSelection=true){
        this._m_Path = thePath;
        theFilesList.sort(this._sortFSEntry);
        let aSelEl = this.getSelected();
        let aSelInfo = null;
        if( aSelEl && isRestoreSelection ){
            aSelInfo = aSelEl.file_info;
        }
        this.clear();
        if( thePath != "" && thePath != "/" && thePath != this._m_Root){
            let anUpInfo = new Object();
            anUpInfo.name = "...";
            anUpInfo.is_file = false;
            anUpInfo.size = 0;
            anUpInfo.is_up = true;
            let aRow = this._createFileElement(anUpInfo.name, anUpInfo.is_file, anUpInfo.size, true);
            aRow.file_info = anUpInfo;
            if( isRestoreSelection && aSelInfo && aSelInfo.is_up ){
                this.setSelected(aRow,true);            
            }
        }
        theFilesList.forEach(function(aFileInfo, i, anArray){
            let aRow = this._createFileElement(aFileInfo.name, aFileInfo.is_file, aFileInfo.size);
            aFileInfo.is_up = false;
            aRow.file_info = aFileInfo;
            if( aSelInfo && ((aFileInfo.name == aSelInfo.name) && (aFileInfo.is_file == aSelInfo.is_file))){
                this.setSelected(aRow,true);            
            }
        }, this);
        this.reconnect();        
    }

    selectFile(theFileName, isFile){
        let anItem = this._findFile(theFileName, isFile);
        if( anItem ){
            this.setSelected(anItem, true, true);
        }
    }

    getSelectedFileInfo(){
        let aSelItem = this.getSelected();
        if( aSelItem )
            return aSelItem.file_info;
        return null;
    }

    getFilesOrFolersNames(isFiles){
        let aRes = [];
        let anEl = this._m_BaseElement.firstElementChild;
        while(anEl){
            let aFileInfo = anEl.file_info;
            if( aFileInfo && !aFileInfo.is_up ){
                if( aFileInfo.is_file == isFiles){
                    aRes.push(aFileInfo.name);
                }
            }
            anEl = anEl.nextElementSibling;
        }
        return aRes;
    }

    _findFile(theFileName, isFile){
        let anEl = this._m_BaseElement.firstElementChild;
        while(anEl){
            let aFileInfo = anEl.file_info;
            if( aFileInfo && ((aFileInfo.name == theFileName) && (aFileInfo.is_file == isFile))){
                return anEl;
            }
            anEl = anEl.nextElementSibling;
        }
        return null;
    }

    _sortFSEntry(theA, theB){
        if( theA.is_file != theB.is_file ){
            if( theA.is_file ){
                return 1;
            }
            return -1;
        }
        let anAFileName = theA.name;
        let aBFileName = theB.name;
        if( anAFileName < aBFileName )
            return -1;
        if( anAFileName > aBFileName )
            return 1;
        return 0;
    }

    _createFileElement(theFileName, isFile, theFileSize, isUp=false){
        let aTr = document.createElement("tr");
        this._m_BaseElement.appendChild(aTr);
        let anIconTd = document.createElement("td");
        aTr.appendChild(anIconTd);
        let anIconImg = document.createElement("img");
        if( isUp ){
            anIconImg.setAttribute("src","images/up.png")
        }
        else if(isFile){
            anIconImg.setAttribute("src","images/file.png");
        }
        else{
            anIconImg.setAttribute("src", "images/folder.png");
        }
        anIconTd.append(anIconImg);
        let aNameTd = document.createElement("td");
        aNameTd.textContent = theFileName;
        aTr.appendChild(aNameTd);
        let aSizeTd = document.createElement("td"); 
        if( !isUp && isFile ){
            let aPrettySize = GUIUtils.printBytes(theFileSize);
            aSizeTd.innerHTML = "&nbsp;&nbsp;" + aPrettySize;
        }
        else{
            aSizeTd.innerHTML = "&nbsp;&nbsp;"
        }
        aTr.appendChild(aSizeTd);
        return aTr;
    }

    _processDblClick(theItem){
        let aFileInfo = theItem.file_info;
        if( !aFileInfo.is_file ){
            if( this._m_FolderDblClickCallback ){
                if( aFileInfo.is_up){
                    let aSplitPath = this._m_Path.split("/");
                    if( aSplitPath.length > 0 ){
                        aSplitPath = aSplitPath.slice(0,-1);
                        let aNewPath = aSplitPath.join("/");
                        this._m_FolderDblClickCallback(aNewPath);
                    }
                }
                else{
                    let aNewPath = aFileInfo.name;
                    if( this._m_Path != "" )
                        aNewPath = this._m_Path + "/" + aFileInfo.name;
                    this._m_FolderDblClickCallback(aNewPath);
                }
            }
        }
    }
}

export {FileViewerController};