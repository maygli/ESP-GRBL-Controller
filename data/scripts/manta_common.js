
function findAncestor (theElement, theClass) {
    do{
        theElement = theElement.parentNode;
        var aClasses = theElement.className.split(" ");
    } while( theElement && aClasses && ( aClasses.indexOf(theClass) < 0 ));
    return theElement;
}

function ipFilter(event)
{
    let charCode = (event.which) ? event.which : event.keyCode
    if (charCode != 46 && charCode != 110 && charCode != 190 && charCode > 31 
      && (charCode < 48 || charCode > 57))
       return false;
    return true;
}

function printBytes(theBytes) {
    var aSizes = ['B', 'K', 'M', 'G', 'T', 'P'];
    for (var i = 0; i < aSizes.length; i++) {
      if (theBytes < 1024) {
        return theBytes + ' ' + aSizes[i];
      } else {
        theBytes = parseFloat(theBytes / 1024).toFixed(2)
      }
    }
    return theBytes + ' P';
}


