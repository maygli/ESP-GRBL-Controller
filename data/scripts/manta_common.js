
function loadJSON(theRequestName, theCallback){
    let request = new XMLHttpRequest();
    request.open('GET', theRequestName, true);
    request.addEventListener('readystatechange', function() {
        if ((request.readyState==4) && (request.status==200)){
            let aResJSON = JSON.parse(request.responseText);
            theCallback(aResJSON);
        }
    });
    request.send();
}

function loadText(theRequestName, theCallback){
    let request = new XMLHttpRequest();
    request.open('GET', theRequestName, true);
    request.addEventListener('readystatechange', function() {
        if ((request.readyState==4) && (request.status==200)){
            let aResText = request.responseText;
            theCallback(aResText);
        }
    });
    request.send();
}

function onExpandComboBox(theBtn)
{
    var aComboBox = findAncestor(theBtn,"combobox");
    if( aComboBox ){
        var aComboBoxContent = aComboBox.querySelector(".combobox-content");
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
    var aComboBox = findAncestor(theElement,"combobox");
    if( aComboBox ){
        var aBtn = aComboBox.querySelector(".combobox_btn");
        if( aBtn ){
            aBtn.innerHTML = theElement.innerHTML;
        }
        var aComboBoxContent = aComboBox.querySelector(".combobox-content");
        if( aComboBoxContent ){
            aComboBoxContent.style.display = "none";
        } 
    }
}

function findAncestor (theElement, theClass) {
    do{
        theElement = theElement.parentNode;
        var aClasses = theElement.className.split(" ");
        var anIndx = aClasses.indexOf(theClass);
    } while( theElement && aClasses && ( aClasses.indexOf(theClass) < 0 ));
    return theElement;
}

function onCloseComboBox()
{
    var aComboBoxContents = document.querySelectorAll(".combobox-content");
    for( var i = 0 ; i < aComboBoxContents.length ; i++ ){
        aComboBoxContents[i].style.display = "none";
    }
}

function onMouseOver(theComboBox)
{
    var aComboBoxContent = theComboBox.querySelector(".combobox-content");
    if( aComboBoxContent.style.display != "block" ){
        aComboBoxContent.style.display = "inline-block";
    }
}

function onMouseOut(theComboBox)
{
    var aComboBoxContent = theComboBox.querySelector(".combobox-content");
    if( aComboBoxContent.style.display != "none" ){
        aComboBoxContent.style.display = "none";
    }
}