import {PageController} from "./controllers/page_controller.js"
import {ComboBoxController} from "./controllers/combobox_controller.js"
import { SliderController } from "./controllers/slider_controller.js";
import {MultiStateController} from "./controllers/multi_state_controller.js"
import {AxisController} from "./axis_controller.js"
import {ProgressDialogController} from "./progress_dialog_controller.js"

class MainPageController extends PageController{
    constructor(theContentEl){
        super(theContentEl);
        this._m_AxisCount = 3;
        this._m_X = 0.;
        this._m_Y = 0.;
        this._m_Z = 0.;
        this._m_A = 0.;
        this._m_B = 0.;
        this._m_C = 0.;
        this._m_D = 0.;
        this._m_E = 0.;
    }

    setDataUpdater(theProvider){
        this._m_Updater = theProvider;
        this._m_Updater.registerDataReq(this._onUpdate.bind(this));
    }

    setAxisCount(theAxisCount){
        if( !this._m_IsLoaded )
            return;
        this._m_AxisCtrl.setAxisCount(theAxisCount);
    }

    async _upadteAfterLoad(){
        let anAxisSelEl = this._m_BaseElement.querySelector("#axis_selector");
        this._m_AxisSel = new ComboBoxController(anAxisSelEl);
        let aSliderEl = this._m_BaseElement.querySelector("#power_slider");
        this._m_SliderCtrl = new SliderController(aSliderEl);
        let aPwrBtn = this._m_BaseElement.querySelector("#laser_power");
        this._m_PwrCtrl = new MultiStateController(aPwrBtn);
        this._m_PwrCtrl.addState("images/laser_on.png","laser_on");
        this._m_PwrCtrl.addState("images/laser_off.png","laser_off");
        this._m_PwrCtrl.setState(1);
        this._m_AxisCtrl = new AxisController(this._m_BaseElement);
        this._m_FileInpuField = this._m_BaseElement.querySelector("#file_upload");
        this._m_FileInpuField.addEventListener("change", this._onCompProcess.bind(this)); 
        this._m_ProcessCompBtn = this._m_BaseElement.querySelector("#process_from_comp");
        this._m_ProcessCompBtn.addEventListener("click", this._onBtnProcessCompClick.bind(this));
        let aProgDlgEl = this._m_BaseElement.querySelector("#progress_process");
        this._m_ProgDlgCtrl = new ProgressDialogController(aProgDlgEl);
        this._m_ProgDlgCtrl.hide();
        super._upadteAfterLoad();
        this.setAxisCount(this._m_AxisCount);
    }

    _setCoord(theCoord){
        if( this._m_X != theCoord.X){
            this._m_X = theCoord.X;
            let anEl = this._m_BaseElement.querySelector("#curr_x");
            if( anEl ){
                anEl.innerHTML = theCoord.X;
            }
        }
        if( this._m_Y != theCoord.Y){
            this._m_Y = theCoord.Y;
            let anEl = this._m_BaseElement.querySelector("#curr_y");
            if( anEl ){
                anEl.innerHTML = theCoord.Y;
            }
        }
        if( this._m_Z != theCoord.Z){
            this._m_Z = theCoord.Z;
            let anEl = this._m_BaseElement.querySelector("#curr_z");
            if( anEl ){
                anEl.innerHTML = theCoord.Z;
            }
        }
        if( this._m_A != theCoord.A){
            this._m_A = theCoord.A;
            let anEl = this._m_BaseElement.querySelector("#curr_a");
            if( anEl ){
                anEl.innerHTML = theCoord.A;
            }
        }
        if( this._m_B != theCoord.B){
            this._m_B = theCoord.B;
            let anEl = this._m_BaseElement.querySelector("#curr_b");
            if( anEl ){
                anEl.innerHTML = theCoord.B;
            }
        }
        if( this._m_C != theCoord.C){
            this._m_C = theCoord.C;
            let anEl = this._m_BaseElement.querySelector("#curr_c");
            if( anEl ){
                anEl.innerHTML = theCoord.C;
            }
        }
        if( this._m_D != theCoord.D){
            this._m_D = theCoord.D;
            let anEl = this._m_BaseElement.querySelector("#curr_d");
            if( anEl ){
                anEl.innerHTML = theCoord.D;
            }
        }
        if( this._m_E != theCoord.E){
            this._m_E = theCoord.E;
            let anEl = this._m_BaseElement.querySelector("#curr_e");
            if( anEl ){
                anEl.innerHTML = theCoord.E;
            }
        }
    }

    _onUpdate(theParameters){
        if( !this._m_IsLoaded )
            return;
        let anAxisCount = theParameters.axis_count;
        if( anAxisCount != this._m_AxisCount ){
            this._m_AxisCount = anAxisCount;
            this.setAxisCount(anAxisCount);
        }
        this._setCoord(theParameters);
    }

    async _onBtnProcessCompClick(){
        this._m_FileInpuField.dispatchEvent(new MouseEvent("click")); 
    }

    _onCompProcess(){
        let aFile = this._m_FileInpuField.files[0];
        this._m_FileInpuField.value="";
        this._m_ProgDlgCtrl.show();
    }

}

export {MainPageController};