var http = require('http');
var url = require('url');
var fs = require('fs');
var mime = require('mime');

function generate_board_info()
{
  var aBoardInfo = {
    "board_name": "MantaRay GRBL",
    "board_hw_version": "v1.0.0",
    "board_sw_version": "v2.1.3",
    "board_desc": "MantaRay GRBL controller based on esp8266",
    "manufacturer": "Maygli <a href=\"mailto:mmaygli@gmail.com\">mmaygli@gmail.com</a>",
    "copyright": "&copy; Maygli 2021"
  };
  return JSON.stringify(aBoardInfo)
}

function generate_settings_wifi(){
  var aWiFiSettings = {
    "type_selector":"st",
    "ap_ssid":"Test",
    "ap_pwd":"11111111111111111",
    "ap_ip":"192.168.1.11",
    "ap_netmask":"255.255.255.0",
    "ap_gateway":"192.168.1.1",
    "st_ssid1":"MaygliHome",
    "st_pwd1":"1",
    "st_ssid2":"",
    "st_pwd2":"",
    "st_ssid3":"",
    "st_pwd3":"",
    "st_ssid4": "",
    "st_pwd4": "",
    "st_ip": "192.168.1.70",
    "st_timeout": "5"
  };
  return JSON.stringify(aWiFiSettings)
}

function generate_settings_grbl(){
  var aGrblSettings = {
    "grbl0": "0000",
    "grbl1": "0001",
    "grbl2": "0002",
    "grbl3": "0003",
    "grbl4": "false",
    "grbl5": "false",
    grbl6: 'false',
    grbl10: '0010',
    grbl11: '0011',
    grbl12: '0012',
    grbl13: 'false',
    grbl20: 'false',
    grbl21: 'false',
    grbl22: 'false',
    grbl23: '0023',
    grbl24: '0024',
    grbl25: '0025',
    grbl26: '0026',
    grbl27: '0027',
    grbl30: '0030',
    grbl31: '0031',
    grbl32: 'false',
    grbl100: '0100',
    grbl101: '0101',
    grbl102: '0102',
    grbl110: '0110',
    grbl111: '0111',
    grbl112: '0112',
    grbl120: '0120',
    grbl121: '0121',
    grbl122: '0122',
    grbl130: '0130',
    grbl131: '0131',
    grbl132: '0132' 
  }
  return JSON.stringify(aGrblSettings)
}

function get_filesystem_data(thePath){
  console.log("Request filesystem data. Path="+thePath);
  let aRawData = fs.readFileSync('fs_config.json');
  let aFsObject = JSON.parse(aRawData);  
  return JSON.stringify(aFsObject);
}

http.createServer(function (req, res) {
  console.log("Get request"); 
  console.log(req.url);
  console.log(req.method);
  var q = url.parse(req.url, true);
  var aPathName = q.pathname;
  console.log(aPathName)
  if( req.method == "POST" ){
    if( aPathName == "/settings_wifi" || aPathName == "/settings_grbl"){
      let body = "";
      req.on("data", (chunk) => {
        body += chunk.toString(); // convert Buffer to string
      });
      req.on("end", () => {
        const result = JSON.parse(body);
        console.log(result);
        return res.end("ok");
      });
    }
    else{
      return res.end("ok");
    }
    return;
  }
  if( aPathName == "/filesystem"){
    aParam = q.query;
    console.log("Query params=",aParam);
    aPath = aParam["path"]
    console.log("Path=",aPath);    
    var aData = get_filesystem_data(aPath);
    res.writeHead(200, {"Content-Type": "application/json"});
    res.write(aData);
    return res.end();
  }
  if( aPathName == "/settings_wifi"){
    res.writeHead(200, {"Content-Type": "application/json"});
    var aData = generate_settings_wifi();
    res.write(aData);
    return res.end();
  }
  if( aPathName == "/settings_grbl"){
    res.writeHead(200, {"Content-Type": "application/json"});
    var aData = generate_settings_grbl();
    res.write(aData);
    return res.end();
  }
  if( aPathName == "/board_info" ){
    res.writeHead(200, {"Content-Type": "application/json"});
    var aData = generate_board_info();
    res.write(aData);
    return res.end();
  }    
  if((aPathName == "/") || (aPathName=="")){
    console.log("Set path to index"); 
    aPathName = "/index.html";
  }
  var filename = "../../data" + aPathName;
  console.log("Request file"); 
  console.log(filename); 
  fs.readFile(filename, function(err, data) {
    if (err) {
      res.writeHead(404, {'Content-Type': 'text/html'});
      return res.end("404 Not Found");
    } 
    var aType = mime.getType(aPathName)
    res.writeHead(200, {'Content-Type': aType});
    res.write(data);
    return res.end();
  });
}).listen(8080);