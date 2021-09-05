import {DialogController} from "./controllers/dialog_controller.js"
import {GUIUtils} from "./gui_utils.js"

class RenameFileDialog extends DialogController{
    constructor(theEl){
        super(theEl);
        this._m_FilePath = this._m_BaseElement.querySelector("#filename_info");
        this._m_NewName = this._m_BaseElement.querySelector("#new_name");
        this.setButtonCallback("button_ok", this._onOk.bind(this));
    }

    setExisted(theExisted){
        this._m_Existed = theExisted;
    }

    setFile(theFile){
        if( this._m_FilePath )
            this._m_FilePath.innerHTML = "&nbsp;" + theFile;
        let aSplitted = theFile.split("/");
        let aName = aSplitted.slice(-1)[0];
        this._m_NewName.value = GUIUtils.getUniqName(aName, this._m_Existed);
        this._m_NewName.select();
        this._m_NewName.focus();
    }

    setRenameCallcback(theCallback){
        this._m_RenameCallback = theCallback;
    }

    _onOk(){
        let anErr = "";
        let aValue = this._m_NewName.value;
        if( aValue == "" ){
            anErr = i18next.t("empty_filename_error");
        }
        else{
            let aMatch = aValue.match(/^(?!\.)(?!com[0-9]$)(?!con$)(?!lpt[0-9]$)(?!nul$)(?!prn$)[^\|\*\?\\:<>/$"]*[^\.\|\*\?\\:<>/$"]+$/); 
            if( !aMatch ){
                anErr = i18next.t("incorrect_file_name");
            }
        }
        if( this._m_Existed && this._m_Existed.includes(aValue) ){
            anErr = i18next.t("file_already_exists");
        } 
        if( anErr != "" ){
            this._m_NewName.setCustomValidity(anErr);
            this._m_NewName.reportValidity();
            this._m_NewName.select();
            this._m_NewName.focus();
            return false;
        }
        if( this._m_RenameCallback ){
            this._m_RenameCallback(aValue);
        }
        return true;
    }    
}

export {RenameFileDialog};