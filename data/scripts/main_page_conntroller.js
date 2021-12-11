import {PageController} from "./controllers/page_controller.js"
import {ComboBoxController} from "./controllers/combobox_controller.js"
import { SliderController } from "./controllers/slider_controller.js";
import {MultiStateController} from "./controllers/multi_state_controller.js"

class MainPageController extends PageController{
    constructor(theContentEl){
        super(theContentEl);
    }

    async _upadteAfterLoad(){
        super._upadteAfterLoad();
        let anAxisSelEl = this._m_BaseElement.querySelector("#axis_selector");
        this._m_AxisSel = new ComboBoxController(anAxisSelEl);
        let aSliderEl = this._m_BaseElement.querySelector("#power_slider");
        this._m_SliderCtrl = new SliderController(aSliderEl);
        let aPwrBtn = this._m_BaseElement.querySelector("#laser_power");
        this._m_PwrCtrl = new MultiStateController(aPwrBtn);
        this._m_PwrCtrl.addState("images/laser_on.png","laser_on");
        this._m_PwrCtrl.addState("images/laser_off.png","laser_off");
        this._m_PwrCtrl.setState(1);
     }

}

export {MainPageController};