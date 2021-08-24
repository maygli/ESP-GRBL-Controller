import {MenuController} from "./controllers/menu_controller.js"

class SettingsMenu extends MenuController{
    constructor(theElement){
        super(theElement);
    }

    setStackController(thePagesController){
        this._m_StackController = thePagesController;
    }

    _activateItem(theItem){
        super._activateItem(theItem);
        this.viewMenu(false);
    }

    _executeItem(theItem){
        let anId = theItem.getAttribute("id");
        this._m_StackController.activatePage(anId);
    }

    _getControllerItemName(){
        return "settings_menu_item";
    }
} 

export {SettingsMenu};