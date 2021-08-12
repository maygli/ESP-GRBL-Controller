function onConnectionType(theElement){
    let aAPCheckBox = document.getElementById("access_point_type");
    let aAPGroup = document.getElementById("ap_group");
    let aStGroup = document.getElementById("st_group");
    if( aAPCheckBox.checked){
        aAPGroup.className="active_properties_group";
        aStGroup.className="properties_group";
    }
    else{
        aAPGroup.className="properties_group";
        aStGroup.className="active_properties_group";
    }
}

function onViewPwdClick(theElement)
{
    let anInput = theElement.previousSibling;
    if( anInput ){
        let aType = anInput.getAttribute("type");
        if( aType == "text" ){
            anInput.setAttribute("type", "password");
            theElement.setAttribute("src","images/eye.png")
        }
        if( aType == "password" ){
            anInput.setAttribute("type", "text");
            theElement.setAttribute("src","images/close_eye.png")
        }
    }
}

function onSubmitResponseSettingsWiFi(theText, theResponseCode)
{
    anEl = document.getElementById("submit_response");
    if( anEl ){
        anEl.style.display = "inline-block";
        if( theResponseCode == 200 ){
            anEl.className = "server_message_text send_success";
            anEl.innerHTML = i18next.t("submit_suceess");
        }
        else{
            anEl.className = "server_message_text send_fail";
            anEl.innerHTML = i18next.t("submit_fail");
        }
        setTimeout(onHideServerMsg, 2000);
    }
}

function onHideServerMsg()
{
    anEl = document.getElementById("submit_response");
    if( anEl ){
        anEl.style.display = "none";
    }
}

function updateWiFiSettings(theInfo)
{
    updateForm(theInfo); 
    onConnectionType();
}