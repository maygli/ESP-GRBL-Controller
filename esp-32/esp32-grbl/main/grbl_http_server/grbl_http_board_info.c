#include <esp_err.h>
#include <esp_http_server.h>

esp_err_t GRBL_SendBoardInfo(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    char* aData = "{\ 
\"board_name\": \"MantaRay GRBL\",\
\"board_hw_version\": \"v1.0.0\",\
\"board_sw_version\": \"v2.1.3\",\
\"board_desc\": \"MantaRay GRBL controller based on esp32\",\
\"manufacturer\": \"Maygli <a href=\\\"mailto:mmaygli@gmail.com\\\">mmaygli@gmail.com</a>\",\
\"copyright\": \"&copy; Maygli 2021\"\
}";
  return  httpd_resp_sendstr(req,aData);
}