import {BaseController} from "./controllers/base_controller.js"

class ConnectionController extends BaseController{
    constructor(theEl){
        super(theEl);
        this._m_isConnected = true;
    }

    setDataUpdater(theProvider){
        this._m_Updater = theProvider;
        this._m_Updater.registerDataReq(this._onConnection.bind(this));
    }

    _onConnection(theParameters){
        let isConnected = theParameters.grbl_connected;
        if( isConnected == this._m_isConnected ){
            return;
        }
        this._m_isConnected = isConnected;
        if( isConnected==true ){
            this._m_BaseElement.setAttribute("src","images/connected.png");
        }
        else{
            this._m_BaseElement.setAttribute("src","images/disconnected.png");
        }
    }
}

export {ConnectionController};