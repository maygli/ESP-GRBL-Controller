import {PasswordController} from "./controllers/password_controller.js"
import {PageController,ParametersPageController,FormPageController} from "./controllers/page_controller.js"
import {StackController} from "./controllers/stack_controller.js"
import {SettingsWifiPageController} from "./manta_settings_wifi.js"
import {LanguageSelector} from "./language_selector.js"
import {Translator} from "./translator.js"
import {InfoPageController} from "./manta_settings_info.js"
import {MantaMainMenu} from "./manta_main_menu.js"
import {SettingsMenu} from "./manta_settings_menu.js"
import {ConsoleController} from "./manta_console.js"
import {UpgradeController} from "./manta_settings_upgrade.js" 
import {SDController} from "./manta_settings_sd.js"
import {DataUpdater} from "./data_updater.js"
import {HttpProcessor} from "./http_processor.js"

export async function onMainWindowLoaded()
{
    let language = navigator.languages ? navigator.languages[0] : (navigator.language || navigator.userLanguage);
    i18next.init({
        lng: language,
        debug: true,
        resources: translations 
    });
    let aBoardInfo = await HttpProcessor.loadData("board_info",false);
    setMainWindowBoardInfo(aBoardInfo);
    let aDefAct = document.querySelector(".active");
 //   if( aDefAct ){
 //     onMainMenuClickWithUpdate(aDefAct);
 //   }
    let aLang = language.split("-")[0];
    Translator.translate(aLang, document);
    let anEl = document.querySelector("#lang_selector");
    let aLangSel = new LanguageSelector(anEl);
    aLangSel.setCurrentItem(language);
 //   updateLangSelector(aLang);

    let aPagesContent = document.getElementById("pages");
    let aPagesStack = new StackController(aPagesContent);
    let aDataUpdater = new DataUpdater();
    let aReqParams = new Object();
    aReqParams.url = "filesystem";
    aReqParams.interval = 3;
    aDataUpdater.registerDataReq("FileSystem", aReqParams);

    let aConsoleEl = document.querySelector("#manta_console");
    let aConsController = new ConsoleController(aConsoleEl);
    aPagesStack.addPageController("main_menu_console",aConsController);

    let aWiFiEl = document.querySelector("#manta_settings_wifi");
    let aWiFiSett = new SettingsWifiPageController(aWiFiEl);
    aPagesStack.addPageController("settings_menu_wifi", aWiFiSett)

    let aGrblEl = document.querySelector("#manta_settings_grbl");
    let aGrblSett = new FormPageController(aGrblEl);
    aPagesStack.addPageController("settings_menu_grbl", aGrblSett);

    let anUpgradeEl = document.querySelector("#manta_settings_upgrade");
    let anUpgradeSett = new UpgradeController(anUpgradeEl);
    aPagesStack.addPageController("settings_menu_upgrade", anUpgradeSett);

    let aSDEl = document.querySelector("#manta_settings_sd");
    let aSDSett = new SDController(aSDEl);
    aPagesStack.addPageController("settings_menu_sd", aSDSett);

    let anInfoEl = document.querySelector("#manta_info");
    let anInfoSett = new InfoPageController(anInfoEl);
    aPagesStack.addPageController("main_menu_about", anInfoSett);

    let aMainMenu = document.querySelector("#main_menu");
    let aMainMenuController = new MantaMainMenu(aMainMenu);
    aMainMenuController.setStackController(aPagesStack);
    
    let aSettMenuEl = document.querySelector("#settings_menu");
    let aSettingsMenu = new SettingsMenu(aSettMenuEl);
    aSettingsMenu.setStackController(aPagesStack);
    aMainMenuController.setSettingsMenuController(aSettingsMenu);

//    let aMenuController = new MenuController(null);
/*    let aMenuEl = document.getElementById("main_menu");
    let aMainMenuController = new MenuController(aMenuEl);
    
    let aHomeEl = document.getElementById("main_menu_home");
    let aHomeContent = document.getElementById("manta_home");
    let aHomeController = new PageController(aHomeEl,aMainMenuController,aHomeContent,"home.html");

    let aSettEl = document.getElementById("main_menu_settings");
    let aSettingsController = new MenuItemController(aSettEl,aMainMenuController);
*/
//    let aController = new LoadDataPageController("info.html","manta_home","");
//    anEl.controller = aController;
//    aMenuController.addMenuItem(anEl, aController);

//    anEl.onclick = aController.activate.bind(aController);
//    setInterval(onTimer,1000);
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




