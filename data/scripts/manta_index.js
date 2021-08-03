function onMainWindowLoaded()
{

    var language = navigator.languages ? navigator.languages[0] : (navigator.language || navigator.userLanguage);
    i18next.init({
        lng: language,
        debug: true,
        resources: translations 
    });
    var aTest = i18next.t('key')
    let aTestLabel = document.getElementById('test_label');
    aTestLabel.innerHTML = aTest;

    loadJSON("board_info",loadBoardInfoCB);
}

function loadBoardInfoCB(theBoardInfo)
{
    const lang = navigator.language;
    console.log("======" + lang + "=======");
    let boardname = document.getElementById('boardname_id');
    boardname.innerHTML = theBoardInfo.board_name;
    let hwversion = document.getElementById("hardware_version_id")
    hwversion.innerHTML = theBoardInfo.board_hw_version;
    let aCopyRight = document.getElementById("main_footer_id")
    aCopyRight.innerHTML = theBoardInfo.copyright;
}

/* Toggle between adding and removing the "responsive" class to topnav when the user clicks on the icon */
function myFunction() {
    var x = document.getElementById("myTopnav");
    if (x.className === "topnav") {
      x.className += " responsive";
    } else {
      x.className = "topnav";
    }
  }

  function clearActiveMainMenu(){
    anItems = document.getElementsByClassName("main_menu_item");
    for( var i = 0 ; i < anItems.length ; i++ ){
        anItems[i].className = "main_menu_item";
    }
  }

  function onMainMenuClick(sender) {
    clearActiveMainMenu();
    sender.className += " active";
  }

 
  