import {BaseController} from "./base_controller.js"

class ComboBoxController extends BaseController{
    constructor(theEl){
        super(theEl);
        this._m_BaseElement.onmouseover = this._onMouseOver.bind(this);
        this._m_BaseElement.onmouseout = this._onMouseOut.bind(this);
        this._m_ComboBoxContent = this._m_BaseElement.querySelector('[data-controller_item="ComboBoxContent"]'); 
        this._m_ComboBoxButton = this._m_BaseElement.querySelector('[data-controller_item="ComboBoxButton"]');
        this._m_ComboBoxItems = this._m_BaseElement.querySelectorAll('[data-controller_item="ComboBoxItem"]');
        for( let i = 0 ; i < this._m_ComboBoxItems.length ; i++ ){
            this._m_ComboBoxItems[i].onclick = this._onClickComboBoxItem.bind(this);
        }
    }

    setCurrentItem(theData){
        if( this._m_ComboBoxButton.getAttribute(ComboBoxController._m_ItemDataAttribute) == theData ){
            return;
        }
        let aQueryString = '[' + ComboBoxController._m_ItemDataAttribute + '=' + theData + ']';
        let anItemEl = this._m_ComboBoxContent.querySelector(aQueryString);
        this._selectItem(anItemEl);
    }

    _onMouseOver()
    {
        let aComboBoxContent = this._m_ComboBoxContent;
        if( aComboBoxContent.style.display != "block" ){
            aComboBoxContent.style.display = "block";
        }
    }

    _onMouseOut()
    {
        let aComboBoxContent = this._m_ComboBoxContent;
        if( aComboBoxContent.style.display != "none" ){
            aComboBoxContent.style.display = "none";
        }
    }

    _onClickComboBoxItem(event)
    {
        let aSelItem = event.target||event.srcElement;
        let aParent = aSelItem;
        while(aParent){
            if( aParent.getAttribute("data-controller_item") == "ComboBoxItem"){
                break;
            }
            aParent = aParent.parentElement;
        }
        this._selectItem(aParent);
    }

    _selectItem(theItem){
        this._m_ComboBoxContent.style.display = "none";
        if( theItem ){
            this._m_ComboBoxButton.innerHTML = theItem.innerHTML;
            let aData = theItem.getAttribute(ComboBoxController._m_ItemDataAttribute);
            this._m_ComboBoxButton.setAttribute(ComboBoxController._m_ItemDataAttribute,aData);
            this._comboBoxItemSelected(aData);
        }
    }

    _comboBoxItemSelected(theData){
    }

    static _m_ItemDataAttribute = "data-controller_item_data";
}

export {ComboBoxController};