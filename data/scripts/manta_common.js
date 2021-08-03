
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

