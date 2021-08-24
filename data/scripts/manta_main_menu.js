import {MenuController} from "./controllers/menu_controller.js"

class MantaMainMenu extends MenuController{
    constructor(theElement){
        super(theElement);
    }

    setStackController(thePagesController){
        this._m_StackController = thePagesController;
    }

    setSettingsMenuController(theSettingsController){
        this._m_SettingMenuController = theSettingsController;
        this._m_SettingMenuController.hide();
    }

    _executeItem(theItem){
        super._executeItem(theItem);
        let anId = theItem.getAttribute("id");
        if( ( anId == "main_menu_home" ) || ( anId == "main_menu_console" ) || ( anId == "main_menu_about" ) ){
            this._m_StackController.activatePage(anId);
        }
        else if( anId == "main_menu_settings" ) {
            this._m_SettingMenuController.viewMenu(true);
        }
    }

    _deactivateItem(theItem){
        super._deactivateItem(theItem);
        let anId = theItem.getAttribute("id");
        if( anId == "main_menu_settings" ) {
            this._m_SettingMenuController.viewMenu(false);
        }
    }

} 

export {MantaMainMenu};