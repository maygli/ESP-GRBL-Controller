var http = require('http');
var url = require('url');
var fs = require('fs');
var mime = require('mime');
var path = require('path');
var qs = require('querystring');
var formidable = require('formidable');

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

function get_sd_filesystem_data(thePath){
  let aRes = [];
  console.log("aRealPath=",thePath);    
  fs.readdirSync(thePath).forEach( function(aName){
    console.log("  Name=", aName);
    let aFSEntryObj = new Object();
    aFSEntryObj.name = aName;
    let aPath = path.join(thePath, aName);
    let aStat = fs.statSync(aPath);
    aFSEntryObj.is_file = aStat.isFile();
    aFSEntryObj.size = aStat.size;
    aRes.push(aFSEntryObj);
  });
  console.log("aRes=", aRes);
  return JSON.stringify(aRes);  
}

function generate_settings_wifi(){
  var aWiFiSettings = {
    "type_selector":"st",
    "ap_ssid":"Test",
    "ap_pwd":"11111111111111111",
    "ap_ip":"192.168.1.11",
    "ap_netmask":"255.255.255.0",
    "ap_gateway":"192.168.1.1",
    "st_ssid":"MaygliHome",
    "st_pwd":"1",
    "st_ip": "192.168.1.70",
    "st_netmask":"255.255.0.0",
    "st_gateway":"192.168.2.2",
    "st_attempts": "5"
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
    grbl13: 'Off',
    grbl20: 'Off',
    grbl21: 'Off',
    grbl22: 'Off',
    grbl23: '0023',
    grbl24: '0024',
    grbl25: '0025',
    grbl26: '0026',
    grbl27: '0027',
    grbl30: '0030',
    grbl31: '0031',
    grbl32: 'Off',
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

function get_system_info(theParam){
  console.log("Request system info");
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
    if( aPathName == "/sd_upload" ){
    	var form = new formidable.IncomingForm();
        form.maxFileSize = 2*1024*1024*1024;
    	form.parse(req, function (err, fields, files) {
          if( err ) throw err;
          console.log("Receive SD file=" + files.file.name);
      	  var oldpath = files.file.path;
      	  var newpath = 'SD/' + files.file.name;
      	  fs.copyFile(oldpath, newpath, function (err) {
        	if (err) throw err;
        	res.end("ok");
                fs.unlink(oldpath, function(err){
                  if (err) throw err;
                });
          });
       });
       return true;
    }
    if( aPathName == "/settings_wifi" || aPathName == "/settings_grbl" || aPathName=="/remove_file" || aPathName=="/rename_file" ||
        aPathName == "/create_new_folder"){
      let body = "";
      req.on("data", (chunk) => {
        body += chunk.toString(); // convert Buffer to string
      });
      req.on("end", () => {
        const result = JSON.parse(body);
        console.log(result);
        if( aPathName == "/upload_parameters" ){
        }
        if( aPathName == "/remove_file" ){
          let aRealPath = result.path;
	  if( result.path.startsWith("/SD") ){
            aRealPath = result.path.substring(1);
            console.log("Remove file=" + aRealPath );
            let aStat = fs.statSync(aRealPath);
            if( aStat.isFile() ){
              fs.unlink(aRealPath, function(err){
                if(err) throw err;
	      });
            }
            else{
              fs.rmdirSync(aRealPath,{ recursive: true });
            }
          }
        }
        if( aPathName == "/rename_file" ){
          let aPath = result.path;
          let aNewPath = result.new_path;
	  if( aPath.startsWith("/SD") ){
            let aRealPath = aPath.substring(1);
            aNewPath = aNewPath.substring(1);
            console.log("Rename file=" + aRealPath + " aNewPath=", aNewPath );
	    fs.renameSync(aRealPath, aNewPath);
          }
        }
        if( aPathName == "/create_new_folder" ){
          let aPath = result.path;
	  if( aPath.startsWith("/SD") ){
            let aRealPath = aPath.substring(1);
            console.log("Create new folder in=" + aRealPath);
            fs.mkdirSync(aRealPath);
          }
        }
        return res.end("ok");
      });
    }
    else{
      return res.end("ok");
    }
    return;
  }
  if( aPathName == "/sd_filesystem_data" ){
    let aParam = q.query;
    console.log("Query params=",aParam);
    let aPath = aParam["path"];
    let aRealPath="./SD/" + aPath;
    let aData = get_sd_filesystem_data(aRealPath);
    console.log("aData=",aData); 
    res.writeHead(200, {"Content-Type": "application/json"});
    res.write(aData);
    return res.end();
  }
  if( aPathName == "/system_info"){
    aParam = q.query;
    console.log("Query params=",aParam);
//    aPath = aParam["path"]
///    console.log("Path=",aPath);    
    var aData = get_system_info(aParam);
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