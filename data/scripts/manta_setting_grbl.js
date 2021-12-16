import {FormPageController} from "./controllers/page_controller.js"
import {AxisController} from "./axis_controller.js"

class GRBLController extends FormPageController{
    constructor(theEl){
        super(theEl);
        this._m_AxisController = new AxisController(theEl);
        this._m_AxisCount = 3;
    }

    setAxisCount(theCnt){
        this._m_AxisController.setAxisCount(theCnt);
    }

    setDataUpdater(theProvider){
        this._m_Updater = theProvider;
        this._m_Updater.registerDataReq(this._onAxisCount.bind(this));
    }

    async _upadteAfterLoad(){
        super._upadteAfterLoad();
        this.setAxisCount(8);
    }

    _onAxisCount(theParameters){
        if( !this._m_IsLoaded )
            return;
        let anAxisCount = theParameters.axis_count;
        if( anAxisCount == this._m_AxisCount ){
            return;
        }
        this._m_AxisCount = anAxisCount;
        this.setAxisCount(anAxisCount);
    }
}

export {GRBLController};