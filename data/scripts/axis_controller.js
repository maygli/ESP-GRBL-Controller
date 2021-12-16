import {BaseController} from "./controllers/base_controller.js"

class AxisController extends BaseController{
    constructor(theEl){
        super(theEl);
    }

    setAxisCount(theAxisCount){
        if( theAxisCount < 0 || theAxisCount > AxisController._s_MaxAxisCount ){
            return;
        }
        for( let i = 0 ; i < AxisController._s_MaxAxisCount; i++ ){
           let aClass = ".axis_" + (i+1);
           let anAxisEls = this._m_BaseElement.querySelectorAll(aClass);
           for (let anEl of anAxisEls) {
               if( i < theAxisCount ){
                   anEl.style.display = "";
               }
               else{
                   anEl.style.display = "none";
               }
           }            
        }
    }

    static _s_MaxAxisCount = 8;

}

export {AxisController};