import {HttpProcessor} from "./http_processor.js"

class DataUpdater{
    constructor(theInterval){
        this._m_TimeInterval = theInterval;
        this._m_Counter = 0;
        setInterval(this._onInterval.bind(this), this._m_TimeInterval);
        this._m_Requests = [];
        this._m_IsInRequest = false;
    }

    registerDataReq(theCallback){
        let aPar = new Object();
        aPar.enable = true;
        aPar.callback = theCallback;
        this._m_Requests.push(aPar);
    }    

    enableDataReq(theId){

    }

    disableDataReq(theId){

    }
    
    async _onInterval(){
        return;
        if( this._m_IsInRequest ){
            return;
        }
        let aReqParams = new Object();
        this._m_IsInRequest = true;
        let aRes = await HttpProcessor.loadData("system_info", false);
        for (let aReq of this._m_Requests) {
            if( aReq.enable ){
                aReq.callback(aRes);
            }
        }
        this._m_IsInRequest = false;
        this._m_TimeInterval++;
    }
}

export {DataUpdater};