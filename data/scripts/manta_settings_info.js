function setBoardInfo(theInfo)
{
    let aBoardName = document.getElementById("settings_info_board");
    aBoardName.innerHTML = theInfo.board_name;
    let aHWVersion = document.getElementById("settings_info_hw_version");
    aHWVersion.innerHTML = theInfo.board_hw_version;
    let aSWVersion = document.getElementById("settings_info_sw_version");
    aSWVersion.innerHTML = theInfo.board_sw_version;
    let aManufacturer = document.getElementById("settings_info_manufacturer");
    aManufacturer.innerHTML = theInfo.manufacturer;
    let aDesc = document.getElementById("settings_info_desc");
    aDesc.innerHTML = theInfo.board_desc;
}