function onExpandDesc(){
    var aColSign = document.getElementById("collapse_sign");
    var aContent = document.getElementById("content");
    if(aContent.hidden === true ){
       aContent.hidden = false;
       aColSign.innerHTML = "-";
    }
    else{
       aContent.hidden = true;
       aColSign.innerHTML = "+";
    }
}

function onUploadFile(){
    aModal = document.getElementById("modal");
    aBtnClose = document.getElementById("button_close");
    aBtnClose.style.display = "none"
    aModal.style.display="block"
    var aStatus = document.getElementById("status");
    aStatus.className = "normal_msg";
    var aFile = document.getElementById("file").files[0];
    aFilenameInfo = document.getElementById("filename_info");
    aFilenameInfo.innerHTML = aFile.name;
    if( aFile.name.length > 31 ){
        aLoadBlock = document.getElementById("uploaded_cnt");
        aLoadBlock.style.display="none";
        aStatus.innerHTML = i18next.t("settings_upgrade_name_exceed");
        aStatus.className = "error_msg";
        aBtnClose = document.getElementById("button_close");
        aBtnClose.style.display = "block"
        return false;
    }
    var aParFormData = new FormData();
    var aClear = document.getElementById("clear").checked;
    aParFormData.append("clear", aClear);
    var aParReq = new XMLHttpRequest();
    aParReq.timeout = 5000;
    aParReq.open("POST", "upload_parameters"); 
    aParReq.send(aParFormData);
    aParReq.onreadystatechange = function(){
        if( aParReq.readyState != 4 ){
            return;
        }
        var aFormData = new FormData();
        aFormData.append("file", aFile);
        var aReq = new XMLHttpRequest();
        aReq.upload.addEventListener("progress", onLoadProgress, false);
        aReq.addEventListener("load", onLoadComplete, false);
        aReq.addEventListener("error", onLoadError, false);
        aReq.addEventListener("abort", onLoadAbort, false);
        aReq.open("POST", "upload"); 
        aReq.send(aFormData);
    }

// alert(file.name+" | "+file.size+" | "+file.type);
    return false;
}

function onLoadProgress(event) {
    aProgressBar = document.getElementById("progress_bar")
    aProgressBar.style.display="block";
    aLoadBlock = document.getElementById("uploaded_cnt");
    aLoadBlock.style.display="block";
    aLoadCurrent = document.getElementById("loaded_current");
    aLoadCurrent.innerHTML = event.loaded;
    aLoadTotal = document.getElementById("loaded_n_total");
    aLoadTotal.innerHTML = event.total;
    var aPercent = (event.loaded / event.total) * 100;
    aProgressBar.value = Math.round(aPercent);
    aStatus = document.getElementById("status");
    aStatus.innerHTML = Math.round(aPercent) + i18next.t("settings_upgrade_percents");
//            aStatus.style.backgroundColor = "transparent";
}

function onLoadComplete(event) {
    document.getElementById("progress_bar").style.display="none";
    aStatus = document.getElementById("status");
    aStatus.innerHTML = event.target.responseText;
    if(  event.target.status == 200 ){
        aStatus.className = "success_msg";
    }
    else{
        aStatus.className = "error_msg";
    }
    aBtnClose = document.getElementById("button_close");
    aBtnClose.style.display = "block"
}

function onLoadError(event) {
    document.getElementById("progress_bar").style.display="none";
    aStatus = document.getElementById("file").value=null;
    aStatus = document.getElementById("status");
    aStatus.innerHTML = i18next.t("settings_upgrade_failed");
    aStatus.className = "error_msg";
    aBtnClose = document.getElementById("button_close");
    aBtnClose.style.display = "block"
}

function onLoadAbort(event) {
    document.getElementById("progress_bar").style.display="none";
    aStatus = document.getElementById("file").value=null;
    aStatus.innerHTML = i18next.t("settings_upgrade_aborted");
    aStatus.className = "error_msg";
    aBtnClose = document.getElementById("button_close");
    aBtnClose.style.display = "block"
}

function onCloseClick(){
    aModal = document.getElementById("modal");
    aModal.style.display = "none";
}