import {BaseController} from "./base_controller.js"

class MultiStateController extends BaseController{
    constructor(theEl){
        super(theEl);
        this._m_State = 0;
        this._m_Images = new Array();
        this._m_Descriptions = new Array();
        this._m_ImageEl = theEl.querySelector(".state_image");
        this._m_ImageEl.addEventListener("click",this._onImageClick.bind(this));
        this._m_DescEl = theEl.querySelector(".state_description");
    }

    addState(theImage, theDescription){
        this._m_Images.push(theImage);
        this._m_Descriptions.push(theDescription);
    }

    setState(theState){
        if( theState < 0 || theState > this._m_Images.size ){
            return;
        }
        this._m_State = theState;
        this._m_ImageEl.setAttribute("src", this._m_Images[theState]);
        let aText = i18next.t(this._m_Descriptions[theState]);
        this._m_DescEl.innerHTML = aText;
        this._m_DescEl.setAttribute("data-i18n",this._m_Descriptions[theState]);
    }

    _onImageClick(){
        this._m_State++;
        if( this._m_State >= this._m_Images.length ){
            this._m_State = 0;
        }
        this.setState(this._m_State);
    }
}

export {MultiStateController};