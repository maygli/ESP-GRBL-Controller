class HttpProcessor{
    static async loadData(theURL, isText, theParameters=null)
    {
        let anURL = theURL;
        if( theParameters ){
            anURL = anURL + "?" + new URLSearchParams(theParameters).toString();
        }
        let response = await fetch(anURL);
        if( !response.ok ){
            throw new Error('Server response error', {"response_code":response.status,"response_message":response.text()});
        }
        if( isText ){
            let aText = await response.text();
            return aText;
        }
        let aJson = await response.json();
        return aJson;
    }

    static async sendFormData(theURL, theForm){
        let aFormData = new FormData(theForm);
        let aData = {};
        for (let aTuple of aFormData.entries()) aData[aTuple[0]] = aTuple[1]; 
        let aSendText = JSON.stringify(aData); 
        let aResponse = await fetch(theURL, {
            method: "POST",
            headers: {
              'Content-Type': 'application/json; charset=UTF-8'
            },
            body: aSendText,
        });
        if( !aResponse.ok ){
            throw new Error('Failed to upload form data');
        }
        return aResponse.statusText;
    }

    static uploadFile( theFileName, theParams, theLoadProgressCB, theLoadCompleteCB, theLoadErrCB, theLoadAbortCB ){
        let aParReq = new XMLHttpRequest();
        aParReq.timeout = 5000;
        aParReq.open("POST", "upload_parameters"); 
        aParReq.send(theParams);
        aParReq.onreadystatechange = function(){
            if( aParReq.readyState != 4 ){
                return;
            }
            let aFormData = new FormData();
            aFormData.append("file", theFileName);
            let aReq = new XMLHttpRequest();
            aReq.upload.addEventListener("progress", theLoadProgressCB, false);
            aReq.addEventListener("load", theLoadCompleteCB, false);
            aReq.addEventListener("error", theLoadErrCB, false);
            aReq.addEventListener("abort", theLoadAbortCB, false);
            aReq.open("POST", "upload"); 
            aReq.send(aFormData);
        }
    }
}

export {HttpProcessor};