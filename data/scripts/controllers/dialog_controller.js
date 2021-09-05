import {BaseController} from "./base_controller.js"

class DialogController extends BaseController{
    constructor(theEl){
        super(theEl);
        this._m_ButtonMap = new Map();
        let aButtons = this._m_BaseElement.querySelectorAll('[data-controller_item="DialogButton"]');
        aButtons.forEach(aButton => {
            aButton.addEventListener("click",this._onClick.bind(this));
        }, this);
    }

    setButtonCallback(theId, theCallback){
        this._m_ButtonMap.set(theId, theCallback);
    }

    _onClick(theEvent){
        let aButton = theEvent.currentTarget;
        let anId = aButton.getAttribute("id");
        if( this._m_ButtonMap.has(anId) ){
            let aRes = this._m_ButtonMap.get(anId)();
            if( !aRes ){
                return;
            }
        } 
        this.hide();
    }

}

export {DialogController};
