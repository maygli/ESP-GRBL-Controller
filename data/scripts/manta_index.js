async function onMainWindowLoaded()
{
    let language = navigator.languages ? navigator.languages[0] : (navigator.language || navigator.userLanguage);
    i18next.init({
        lng: language,
        debug: true,
        resources: translations 
    });
    let aBoardInfo = await loadData("board_info",false);
    setMainWindowBoardInfo(aBoardInfo);
    let aDefAct = document.querySelector(".active");
    if( aDefAct ){
      onMainMenuClickWithUpdate(aDefAct);
    }
    let aLang = language.split("-")[0];
    setLanguage(aLang);
    updateLangSelector(aLang);
}

function setMainWindowBoardInfo(theBoardInfo)
{
    let boardname = document.getElementById('boardname_id');
    boardname.innerHTML = theBoardInfo.board_name;
    let hwversion = document.getElementById("hardware_version_id")
    hwversion.innerHTML = theBoardInfo.board_hw_version;
    let aCopyRight = document.getElementById("main_footer_id")
    aCopyRight.innerHTML = theBoardInfo.copyright;
}

function setLanguage(theLang){
  i18next.changeLanguage(theLang).then((t) => {
    let aNeedTranslation = document.querySelectorAll("[data-i18n]");
    for( let i = 0 ; i < aNeedTranslation.length ; i++ ){
      let aTransEl = aNeedTranslation[i];
      let aKey = aTransEl.getAttribute("data-i18n");
      let aVal = t(aKey);
      aTransEl.innerHTML = aVal;
    }
  });
}

function onLangSelClicked(sender)
{
  onClickComboBoxItem(sender);
  let aLang = sender.getAttribute("data-lang");
  setLanguage(aLang);
}

function clearActiveMainMenu(theClassName, theGroup){
  anItems = document.getElementsByClassName(theClassName);
  for( let i = 0 ; i < anItems.length ; i++ ){
      let aGroup = anItems[i].getAttribute("data-menu_group");
      if( aGroup == theGroup )
        anItems[i].className = theClassName;
  }
}

  function updateSubmenus(theShowMenu, isShowDefault)
  {
    let aSubMenus = document.querySelectorAll(".manta_submenu");
    for( let i = 0 ; i < aSubMenus.length ; i++ ){
      let aSubMenu = aSubMenus[i]; 
      if( aSubMenu.hasAttribute("id") ){
        let anId = aSubMenu.getAttribute("id");
        if( anId == theShowMenu ){
          aSubMenu.style.display = "inline-block";
          if(isShowDefault){
            anActItem = aSubMenu.querySelector(".active");
            if( anActItem ){
              loadMenuPage(anActItem);
 //             loadMenuPage(anActItem); 
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
    let aClassName = theMenuElement.className.split(" ")[0];
    let aGroup = theMenuElement.getAttribute("data-menu_group");
    clearActiveMainMenu(aClassName, aGroup);
    theMenuElement.className = aClassName + " active";
  }

  function onSettingMenuItemClick(sender){
    updateMenuState(sender);
    loadMenuPage(sender);
  }
 
 function updateLangSelector(theLang)
 {
    let aLangSel = document.getElementById("lang_selector");
    if( aLangSel ){
      let anItem = aLangSel.querySelector("[data-lang=" + theLang + "]");
      if( anItem ){
        onLangSelClicked(anItem);
      }
    }
 }




