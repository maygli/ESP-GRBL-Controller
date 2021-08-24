import {FormPageController} from "./controllers/page_controller.js"
import {PasswordController} from "./controllers/password_controller.js"

class SettingsWifiPageController extends FormPageController{
    constructor(theContentEl){
        super(theContentEl, "settings_wifi.html", "settings_wifi");
    }

    async setPageParameters(theInfo){
        await super.setPageParameters(theInfo);
        let anAPRadio = this._m_BaseElement.querySelector("#access_point_type");
        anAPRadio.onchange = this._onConnectionType.bind(this); 
        let aStRadio = this._m_BaseElement.querySelector("#station_type");
        aStRadio.onchange = this._onConnectionType.bind(this); 
        this._onConnectionType();
        let aPwds = this._m_BaseElement.querySelectorAll('[data-controller_class="PasswordController"]');
        for( let i=0; i< aPwds.length ; i++ ){
            let anEl = aPwds[i];
            let aController = new PasswordController(anEl); 
        }
    }

    _onConnectionType(){
        let aAPCheckBox = this._m_BaseElement.querySelector("#access_point_type");
        let aAPGroup = this._m_BaseElement.querySelector("#ap_group");
        let aStGroup = this._m_BaseElement.querySelector("#st_group");
        if( aAPCheckBox.checked){
            aAPGroup.className="active_properties_group";
            aStGroup.className="properties_group";
        }
        else{
            aAPGroup.className="properties_group";
            aStGroup.className="active_properties_group";
        }
    }
}

export {SettingsWifiPageController};
