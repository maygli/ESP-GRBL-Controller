import {ComboBoxController} from "./combobox_controller.js"

class UnitController extends ComboBoxController{
    constructor(theCBEl, theDataRoot){
        super(theCBEl);
        this._m_Root = theDataRoot;
        this._m_CurrScale = 1.;
        if( UnitController._s_ConversionMap == null ){
          UnitController._s_ConversionMap = new Map();
          UnitController._s_ConversionMap.set("mm",1);
          UnitController._s_ConversionMap.set("cm",10.);
          UnitController._s_ConversionMap.set("m", 100.);
          UnitController._s_ConversionMap.set("in", 25.4);
          UnitController._s_ConversionMap.set("feet", 304.8);
        }
    }

    _comboBoxItemSelected(theData){
      if( !UnitController._s_ConversionMap.has(theData) ){
        return;
      }
      let anUnitTransId = "unit_" + theData;
      let anUnitName = i18next.t(anUnitTransId);
      let anUnitEls = this._m_Root.querySelectorAll(".unit_name");
      for( let i = 0 ; i < anUnitEls.length ; i++ ){
        let anUnitEl = anUnitEls[i];
        anUnitEl.innerHTML = anUnitName;
        anUnitEl.setAttribute(ComboBoxController._m_TransAttribute, anUnitTransId);
      }
      let aCurrScale = UnitController._s_ConversionMap.get(theData);
      let anUnitValEls = this._m_Root.querySelectorAll(".unit_value");
      for( let i = 0 ; i < anUnitValEls.length ; i++ ){
        let anUnitEl = anUnitValEls[i];
        if( anUnitEl.tagName == "INPUT"){
          let aCurrVal = anUnitEl.value;
          if( aCurrVal != null ){
            aCurrVal = parseFloat(aCurrVal);
            aCurrVal = aCurrVal * this._m_CurrScale;
            aCurrVal = aCurrVal/aCurrScale;
            anUnitEl.value = aCurrVal;
          }
        }
        else{
          let aCurrVal = anUnitEl.innerHTML;
          if( aCurrVal != null ){
            aCurrVal = parseFloat(aCurrVal);
            aCurrVal = aCurrVal * this._m_CurrScale;
            aCurrVal = aCurrVal/aCurrScale;
            anUnitEl.innerHTML = aCurrVal;
          }
        }
      }
      this._m_CurrScale = aCurrScale;
    }

    static _s_ConversionMap = null;
}

export {UnitController};