import {BaseController} from "./base_controller.js"

class PathController extends BaseController{
    constructor(theEl){
        super(theEl);
        this._m_Root = "/";
    }

    setRoot(theRoot){
        this._m_Root = theRoot;
    }

    setPath(thePath){
        this._clear();
        let aSplittedPath = thePath.split("/");
        this._createButton(this._m_Root);
        aSplittedPath.forEach(function(theItem){
            if( theItem !="" && theItem != this._m_Root)
                this._createButton("/" + theItem);
        }, this);
    }

    getPath( theItem=null){
        let aPath = "";
        let anEl = theItem;
        if( !anEl ){
            anEl = this._m_BaseElement.lastChild;
        }
        while( anEl ){
            let aText = anEl.textContent;
            if( aText == this._m_Root )
                break;
            aPath = aText + aPath;
            anEl = anEl.previousElementSibling;
        }
        if( aPath.startsWith("/") ){
            aPath = aPath.substring(1);
        }
        return aPath;
    }

    getAbsolutePath(){
        if( this._m_Root ){
            let aPath = this.getPath();
            if( aPath == "" ){
                return this._m_Root;
            }
            return this._m_Root + "/" + aPath;
        }
        return "/" + this.getPath();
    }

    setClickCallback(theCallback){
        this._m_ClickCallback = theCallback;
    }

    _clear(){
        while( this._m_BaseElement.firstChild ){
            this._m_BaseElement.removeChild(this._m_BaseElement.firstChild);
        }
    }

    _createButton(theText){
        let aBtnEl = document.createElement("span");
        aBtnEl.textContent = theText;
        aBtnEl.setAttribute("class","button");
        aBtnEl.addEventListener("click", this._onBtnClick.bind(this));
        this._m_BaseElement.appendChild(aBtnEl);
    }

    _onBtnClick(theEvent){
        let anEl = theEvent.currentTarget;
        let aPath = this.getPath(anEl);
        if( this._m_ClickCallback )
            this._m_ClickCallback(aPath); 
        
    }

    
}

export {PathController};