function onMainWindowLoaded()
{

    var language = navigator.languages ? navigator.languages[0] : (navigator.language || navigator.userLanguage);
    i18next.init({
        lng: language,
        debug: true,
        resources: translations 
    });
    loadJSON("board_info",loadBoardInfoCB);
    var aDefAct = document.querySelector(".active");
    if( aDefAct ){
      onMainMenuClick(aDefAct);
    }
    var aLang = language.split("-")[0];
    setLanguage(aLang);
    updateLangSelector(aLang);
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

function setLanguage(theLang){
  i18next.changeLanguage(theLang).then((t) => {
    var aNeedTranslation = document.querySelectorAll("[data-i18n]");
    for( var i = 0 ; i < aNeedTranslation.length ; i++ ){
      var aTransEl = aNeedTranslation[i];
      var aKey = aTransEl.getAttribute("data-i18n");
      var aVal = t(aKey);
      aTransEl.innerHTML = aVal;
    }
  });
}

function onLangSelClicked(sender)
{
  onClickComboBoxItem(sender);
  var aLang = sender.getAttribute("data-lang");
  setLanguage(aLang);
}

function clearActiveMainMenu(theClassName){
  anItems = document.getElementsByClassName(theClassName);
  for( var i = 0 ; i < anItems.length ; i++ ){
      anItems[i].className = theClassName;
  }
}

  function updateSubmenus(theShowMenu, isShowDefault)
  {
    var aSubMenus = document.querySelectorAll(".manta_submenu");
    for( var i = 0 ; i < aSubMenus.length ; i++ ){
      var aSubMenu = aSubMenus[i]; 
      if( aSubMenu.hasAttribute("id") ){
        var anId = aSubMenu.getAttribute("id");
        if( anId == theShowMenu ){
          aSubMenu.style.display = "inline-block";
          if(isShowDefault){
            anActItem = aSubMenu.querySelector(".active");
            if( anActItem ){
              loadMenuPage(anActItem); 
            }
          }
        }
        else{
          aSubMenu.style.display = "none";
        }
      }
    }
  }

  function updateMenuState( theMenuElement )
  {
    var aClassName = theMenuElement.className.split(" ")[0];
    clearActiveMainMenu(aClassName);
    theMenuElement.className = aClassName + " active";
  }

  function setPage(thePageText) {
    aPageContainer = document.getElementById("page_content");
    if( aPageContainer ){
      aPageContainer.innerHTML = thePageText;
    }
  }

  function loadMenuPage(theMenuItem)
  {
    if( theMenuItem.hasAttribute("data-page") ){
      aPageName = theMenuItem.getAttribute("data-page");
      loadText(aPageName, setPage);
    }
  }

  function onMainMenuClick(sender) {
    var aShowSubmenu = ""
    if( sender.hasAttribute("data-submenu") ){
      aShowSubmenu = sender.getAttribute("data-submenu");
    }
    var isShowDefault = sender.hasAttribute("data-submenu-activate");
    updateSubmenus(aShowSubmenu, isShowDefault);
    updateMenuState(sender);
    loadMenuPage(sender);
  }

  function onSettingMenuItemClick(sender){
    updateMenuState(sender);
    loadMenuPage(sender);
  }
 
 function updateLangSelector(theLang)
 {
    var aLangSel = document.getElementById("lang_selector");
    if( aLangSel ){
      var anItem = aLangSel.querySelector("[data-lang=" + theLang + "]");
      if( anItem ){
        onLangSelClicked(anItem);
      }
    }
 }