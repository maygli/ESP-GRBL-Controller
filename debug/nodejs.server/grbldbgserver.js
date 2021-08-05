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

http.createServer(function (req, res) {
  console.log("Get request"); 
  console.log(req.url);
  console.log(req.method);
  var q = url.parse(req.url, true);
  var aPathName = q.pathname;
  if( req.method == "POST" ){
    console.log(req.body)
    res.writeHead(200, {'Content-Type': "text/html"});
    res.write("OK");
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