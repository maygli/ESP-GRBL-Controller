
class GUIUtils{
    static printBytes(theBytes) {
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

    static getUniqName(thePrefix, theExisted){
      let aCounter = 1;
      let aNewName = "";
      let aNotUniq = false;
      do {
        aNewName = thePrefix + aCounter;
        aNotUniq = theExisted.includes(aNewName);
        aCounter++;
      } while( aNotUniq );
      return aNewName;
    }
    
}

export {GUIUtils};