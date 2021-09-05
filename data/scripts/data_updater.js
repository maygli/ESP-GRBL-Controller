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
        return;
        if( this._m_IsInRequest ){
            return;
        }
        let aReqParams = new Object();
        for (let aReqId of this._m_Requests.keys()) {
            if( this._m_Requests.get(aReqId).enable ){
                aReqParams[aReqId]=true;
            }
        }
        this._m_IsInRequest = true;
        let aRes = await HttpProcessor.loadData("system_info", false, aReqParams);
        for( let aProp in aRes ){
            if( this._m_Requests.has(aProp) ){
                let aReq = this._m_Requests.get(aProp);
                if( "callback" in aReq ){
                    let aParam = aRes[aProp];
                    aReq["callback"](aParam);
                }
            }
        }
        this._m_IsInRequest = false;
        this._m_TimeInterval++;
    }
}

export {DataUpdater};