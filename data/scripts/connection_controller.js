import {BaseController} from "./controllers/base_controller.js"

class ConnectionController extends BaseController{
    constructor(theEl){
        super(theEl);
        this._m_isConnected = false;
    }

    setDataUpdater(theProvider){
        this._m_Updater = theProvider;
        let aParam = new Object();
        aParam.callback = this._onConnection.bind(this);
        this._m_Updater.registerDataReq("grbl", aParam);
    }

    _onConnection(theParameters){
        let isConnected = theParameters.connected;
        if( isConnected == this._m_isConnected ){
            return;
        }
        this._m_isConnected = isConnected;
        if( isConnected ){
            this._m_BaseElement.setAttribute("src","images/connected.png");
        }
        else{
            this._m_BaseElement.setAttribute("src","images/disconnected.png");
        }
    }
}

export {ConnectionController};