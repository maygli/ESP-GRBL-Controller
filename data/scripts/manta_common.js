
async function loadData(theURL, isText)
{
    let response = await fetch(theURL);
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

function sendFormData(theRequestName, theForm, theCallback){
    let request = new XMLHttpRequest();
    request.open("POST", theRequestName);
    let aFormData = new FormData(theForm);
    let aData = {};
    for (let aTuple of aFormData.entries()) aData[aTuple[0]] = aTuple[1]; 
    request.setRequestHeader('Content-Type', 'application/json; charset=UTF-8');
    request.addEventListener('readystatechange', function() {
        if ( request.readyState==4 ){
            let aResText = request.responseText;
            theCallback(aResText, request.status);
        }
    });
   request.send(JSON.stringify(aData));    
}

function onExpandComboBox(theBtn)
{
    let aComboBox = findAncestor(theBtn,"combobox");
    if( aComboBox ){
        let aComboBoxContent = aComboBox.querySelector(".combobox-content");
        if( aComboBoxContent ){
            if( aComboBoxContent.style.display == "inline-block"){
                aComboBoxContent.style.display = "none";
            }
            else{
                aComboBoxContent.style.display = "inline-block";
            }
        } 
    }
}

function onClickComboBoxItem(theElement)
{
    let aComboBox = findAncestor(theElement,"combobox");
    if( aComboBox ){
        let aBtn = aComboBox.querySelector(".combobox_btn");
        if( aBtn ){
            aBtn.innerHTML = theElement.innerHTML;
        }
        let aComboBoxContent = aComboBox.querySelector(".combobox-content");
        if( aComboBoxContent ){
            aComboBoxContent.style.display = "none";
        } 
    }
}

function findAncestor (theElement, theClass) {
    do{
        theElement = theElement.parentNode;
        var aClasses = theElement.className.split(" ");
    } while( theElement && aClasses && ( aClasses.indexOf(theClass) < 0 ));
    return theElement;
}

function onCloseComboBox()
{
    let aComboBoxContents = document.querySelectorAll(".combobox-content");
    for( let i = 0 ; i < aComboBoxContents.length ; i++ ){
        aComboBoxContents[i].style.display = "none";
    }
}

function onMouseOver(theComboBox)
{
    let aComboBoxContent = theComboBox.querySelector(".combobox-content");
    if( aComboBoxContent.style.display != "block" ){
        aComboBoxContent.style.display = "inline-block";
    }
}

function onMouseOut(theComboBox)
{
    let aComboBoxContent = theComboBox.querySelector(".combobox-content");
    if( aComboBoxContent.style.display != "none" ){
        aComboBoxContent.style.display = "none";
    }
}

function ipFilter(event)
{
    let charCode = (event.which) ? event.which : event.keyCode
    if (charCode != 46 && charCode != 110 && charCode != 190 && charCode > 31 
      && (charCode < 48 || charCode > 57))
       return false;
    return true;
}

/********************** Menu **************************/
function onSettingsPageSubmit(theForm, theCallback)
{
    let anAct = theForm.getAttribute("action");
    sendFormData(anAct,theForm, theCallback);
    return false;
}

function setPageInfo(thePageText) {
    aPageContainer = document.getElementById("page_content");
    if( aPageContainer ){
      aPageContainer.innerHTML = thePageText;
    }
}

async function setPageParameters(theURL, theUpdateCallback)
{
    let aJson = await loadData(theURL, false);
    theUpdateCallback(aJson);
}

async function loadMenuPage(theItem)
{
    if( !theItem.hasAttribute("data-page") )
        return;
    let aPageName = theItem.getAttribute("data-page");
    let aPageText = await loadData(aPageName, true);
    setPageInfo(aPageText);
    if( theItem.hasAttribute("data-page_update") && theItem.hasAttribute("data-page_parameters")){
        let aPageParametersURL = theItem.getAttribute("data-page_parameters");
        let aUpdCallbackName = theItem.getAttribute("data-page_update");
        let aUpdCallback = window[aUpdCallbackName];
        await setPageParameters(aPageParametersURL,aUpdCallback);
    }
    setLanguage(i18next.language);    
}

async function onMainMenuClickWithUpdate(sender)
{
    if( sender.hasAttribute("data-submenu") ){
      let aShowSubmenu = sender.getAttribute("data-submenu");
      let isShowDefault = sender.hasAttribute("data-submenu-activate");
      updateSubmenus(aShowSubmenu, isShowDefault);
      updateMenuState(sender);
      return;
    }
    updateSubmenus("", false);
    updateMenuState(sender);
    loadMenuPage(sender);
}

function updateForm(theInfo)
{
    const { elements } = document.querySelector('form')

    for (const [ key, value ] of Object.entries(theInfo) ) {
      const field = elements.namedItem(key)
      field && (field.value = value)
    }
}
