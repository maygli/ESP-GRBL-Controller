function updateInfo(){
    loadJSON("board_info",setBoardInfo);
}

function setBoardInfo(theInfo)
{
    var aBoardName = document.getElementById("settings_info_board");
    aBoardName.innerHTML = theInfo.board_name;
    var aHWVersion = document.getElementById("settings_info_hw_version");
    aHWVersion.innerHTML = theInfo.board_hw_version;
    var aSWVersion = document.getElementById("settings_info_sw_version");
    aSWVersion.innerHTML = theInfo.board_sw_version;
    var aManufacturer = document.getElementById("settings_info_manufacturer");
    aManufacturer.innerHTML = theInfo.manufacturer;
    var aDesc = document.getElementById("settings_info_desc");
    aDesc.innerHTML = theInfo.board_desc;
}