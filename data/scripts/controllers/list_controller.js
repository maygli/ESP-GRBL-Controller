import {BaseController} from "./base_controller.js"

class ListController extends BaseController{
    constructor(theEl){
        super(theEl);
    }

    clear(){
        while(this._m_BaseElement.firstChild){
            this._m_BaseElement.removeChild(this._m_BaseElement.firstChild);
        }
        if( this._m_SelectionChangeCB ){
            this._m_SelectionChangeCB();
        }
    }

    reconnect(){
        let anEl = this._m_BaseElement.firstElementChild;
        while(anEl){
            anEl.addEventListener("click", this._onClick.bind(this));
            anEl = anEl.nextElementSibling;
        }
    }

    setSelected(theItem, isSelected, isClearSelection=false){
        if( isClearSelection ){
            let anEl = this.getSelected();
            if( anEl )
                this.setSelected(anEl, false);
        }
        let aClassList = theItem.classList;
        if( isSelected ){
            aClassList.add("active");
        }
        else{
            aClassList.remove("active");
        }
        if( this._m_SelectionChangeCB ){
            this._m_SelectionChangeCB();
        }
    }

    getSelected(){
        return this._m_BaseElement.querySelector(".active");
    }

    setSelectionChangeCallback(theCallback){
        this._m_SelectionChangeCB = theCallback;
    }

    _processDblClick(theItem){

    }

    _processClick(theItem){

    }

    _onClick(theEvent){
        let anActive = this.getSelected();
        let aClicked = theEvent.currentTarget;
        if( anActive == aClicked ){
            this._processDblClick(anActive);
            return;
        }
        this.setSelected(aClicked,true, true);
        this._processClick(aClicked);
    }
}

export {ListController};