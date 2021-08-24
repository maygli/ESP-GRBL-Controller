import {HttpProcessor} from "./http_processor.js"

class DataUpdater{
    constructor(theInterval){
        this._m_TimeInterval = theInterval;
        this._m_Counter = 0;
        setInterval(this._onInterval.bind(this), this._m_TimeInterval);
        this._m_Requests = new Map();
        this._m_IsInRequest = false;
    }

    registerDataReq(theId, theParameters){
        theParameters.enable = true;
        this._m_Requests.set(theId, theParameters);
    }    

    enableDataReq(theId){

    }

    disableDataReq(theId){

    }
    
    async _onInterval(){
        if( this._m_IsInRequest ){
            return;
        }
        this._m_IsInRequest = true;
        let aRes = await HttpProcessor.loadData("filesystem");
        this._m_IsInRequest = false;
        this._m_TimeInterval++;
    }
}

export {DataUpdater};