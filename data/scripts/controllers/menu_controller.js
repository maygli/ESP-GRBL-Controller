import {BaseController} from "./base_controller.js"

class MenuController extends BaseController{
    constructor(theEl){
        super(theEl);
        let aMenuItemName = this._getControllerItemName();
        this._m_Items = this._m_BaseElement.querySelectorAll('[data-controller_item="' + aMenuItemName + '"]');
        for( let i = 0 ; i < this._m_Items.length ; i++ ){
            this._m_Items[i].addEventListener("click",this._onItemClicked.bind(this));
        }
    }

    viewMenu(isView){
        if( isView ){
            super.show();
            let anActiveItem = this._getActiveItem();
            if( anActiveItem )
                this._executeItem(anActiveItem);
        }
        else{
            super.hide();
        }
    }

    _getActiveItem(){
        for( let i = 0 ; i < this._m_Items.length ; i++ ){
            let aClassName = this._m_Items[i].className;
            if( aClassName.indexOf("active") >= 0 ){
                return this._m_Items[i];
            }
        }
        return null;
    }

    _onItemClicked(theEvent){
        let aSelItem = theEvent.currentTarget;
        this._selectItem(aSelItem);        
    }

    _selectItem(theItem){
        let anActiveItem = this._getActiveItem();
        if( anActiveItem != null ){
            this._deactivateItem(anActiveItem);
        }
        this._activateItem(theItem);
    }

    _activateItem(theItem){
        theItem.className = "active manta_menu_item";
        this._executeItem(theItem);
    }

    _deactivateItem(theItem){
        theItem.className = "manta_menu_item";
    }

    _executeItem(theItem){
    }

    _getControllerItemName(){
        return "menu_item";
    }
}

export {MenuController};