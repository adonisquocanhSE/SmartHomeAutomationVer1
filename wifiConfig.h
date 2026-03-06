#include <EEPROM.h> //Tên wifi và mật khẩu lưu vào ô nhớ 0->96
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h> //Thêm thư viện web server
WebServer webServer(80); //Khởi tạo đối tượng webServer port 80
#include <Ticker.h>
Ticker blinker;

String ssid;
String password;
#define ledPin 2
#define btnPin 0
unsigned long lastTimePress=millis();
#define PUSHTIME 5000
int wifiMode; //0:Chế độ cấu hình, 1:Chế độ kết nối, 2: Mất wifi
unsigned long blinkTime=millis();
//Tạo biến chứa mã nguồn trang web HTML để hiển thị trên trình duyệt
const char html[] PROGMEM = R"html( 
  <!DOCTYPE html>
    <html>
    <head>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <title>SETTING WIFI INFORMATION</title>
        <style type="text/css">
          body{display: flex;justify-content: center;align-items: center;}
          button{width: 135px;height: 40px;margin-top: 10px;border-radius: 5px}
          label, span{font-size: 25px;}
          input{margin-bottom: 10px;width:275px;height: 30px;font-size: 17px;}
          select{margin-bottom: 10px;width: 280px;height: 30px;font-size: 17px;}
        </style>
    </head>
    <body>
      <div>
        <h3 style="text-align: center;">SETTING WIFI INFORMATION</h3>
        <p id="info" style="text-align: center;">Scanning wifi network...!</p>
        <label>Wifi name:</label><br>
        <select id="ssid">
          <option>Choise wifi name!</option>
        </select><br>
        <label>Password:</label><br>
        <input id="password" type="text"><br>

        <button onclick="saveWifi()" style="background-color: cyan;margin-right: 10px">SAVE</button>
        <button onclick="reStart()" style="background-color: pink;">RESTART</button>
      </div>
        <script type="text/javascript">
          window.onload=function(){
            scanWifi();
          }
          var xhttp = new XMLHttpRequest();
          function scanWifi(){
            xhttp.onreadystatechange = function(){
              if(xhttp.readyState==4&&xhttp.status==200){
                data = xhttp.responseText;
                // alert(data);
                document.getElementById("info").innerHTML = "WiFi scan completed!"
                var obj = JSON.parse(data);
                    var select = document.getElementById("ssid");
                    for(var i=0; i<obj.length;++i){
                      select[select.length] = new Option(obj[i],obj[i]);
                    }
              }
            }
            xhttp.open("GET","/scanWifi",true);
            xhttp.send();
          }
          function saveWifi(){
            ssid = document.getElementById("ssid").value;
            pass = document.getElementById("password").value;
            xhttp.onreadystatechange = function(){
              if(xhttp.readyState==4&&xhttp.status==200){
                data = xhttp.responseText;
                alert(data);
              }
            }
            xhttp.open("GET","/saveWifi?ssid="+ssid+"&pass="+pass,true);
            xhttp.send();
          }
          function reStart(){
            xhttp.onreadystatechange = function(){
              if(xhttp.readyState==4&&xhttp.status==200){
                data = xhttp.responseText;
                alert(data);
              }
            }
            xhttp.open("GET","/reStart",true);
            xhttp.send();
          }
        </script>
    </body>
  </html>
)html";

void blinkLed(uint32_t t){
  if(millis()-blinkTime>t){
    digitalWrite(ledPin,!digitalRead(ledPin));
    blinkTime=millis();
  }
}

void ledControl(){
  if(digitalRead(btnPin)==LOW){
    if(millis()-lastTimePress<PUSHTIME){
      blinkLed(1000);
    }else{
      blinkLed(50);
    }
  }else{
    if(wifiMode==0){
      blinkLed(50);
    }else if(wifiMode==1){
      blinkLed(3000);
    }else if(wifiMode==2){
      blinkLed(300);
    }
  }
}

//Chương trình xử lý sự kiện wifi
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {

    case WIFI_EVENT_STA_CONNECTED:
      Serial.println("WiFi connected (STA)");
      break;

    case IP_EVENT_STA_GOT_IP:
      Serial.println("Connected to WiFi");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      wifiMode = 1;
      break;

    case WIFI_EVENT_STA_DISCONNECTED:
      Serial.println("Disconnected from WiFi");
      wifiMode = 2;
      WiFi.begin(ssid.c_str(), password.c_str());
      break;

    default:
      break;
  }
}


void setupWifi(){
  if(ssid!=""){
    Serial.println("Connecting to wifi...!");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    WiFi.onEvent(WiFiEvent); //Đăng ký chương trình bắt sự kiện wifi
  }else{
    Serial.println("ESP32 wifi network created!");
    WiFi.mode(WIFI_AP);
    uint8_t macAddr[6];
    WiFi.softAPmacAddress(macAddr);
    String ssid_ap="ESP32-"+String(macAddr[4],HEX)+String(macAddr[5],HEX);
    ssid_ap.toUpperCase();
    WiFi.softAP(ssid_ap.c_str());
    Serial.println("Access point name:"+ssid_ap);
    Serial.println("Web server access address:"+WiFi.softAPIP().toString());
    wifiMode=0;
  }
}

void setupWebServer(){
  //Thiết lập xử lý các yêu cầu từ client(trình duyệt web)
  webServer.on("/",[]{
    webServer.send(200, "text/html", html); //Gửi nội dung HTML
  });
  webServer.on("/scanWifi",[]{
    Serial.println("Scanning wifi network...!");
    int wifi_nets = WiFi.scanNetworks(true, true);
    const unsigned long t = millis();
    while(wifi_nets<0 && millis()-t<10000){
      delay(20);
      wifi_nets = WiFi.scanComplete();
    }
    DynamicJsonDocument doc(200);
    for(int i=0; i<wifi_nets; ++i){
      Serial.println(WiFi.SSID(i));
      doc.add(WiFi.SSID(i));
    }
    //["tên wifi1","tên wifi2","tên wifi3",.....]
    String wifiList = "";
    serializeJson(doc, wifiList);
    Serial.println("Wifi list: "+wifiList);
    webServer.send(200,"application/json",wifiList);
  });
  webServer.on("/saveWifi",[]{
    String ssid_temp = webServer.arg("ssid");
    String password_temp = webServer.arg("pass");
    Serial.println("SSID:"+ssid_temp);
    Serial.println("PASS:"+password_temp);
    EEPROM.writeString(0,ssid_temp);
    EEPROM.writeString(32,password_temp);
    EEPROM.commit();
    webServer.send(200,"text/plain","Wifi has been saved!");
  });
  webServer.on("/reStart",[]{
    webServer.send(200,"text/plain","Esp32 is restarting!");
    delay(3000);
    ESP.restart();
  });
  webServer.begin(); //Khởi chạy dịch vụ web server trên ESP32
}

void checkButton(){
  if(digitalRead(btnPin)==LOW){
    Serial.println("Press and hold for 5 seconds to reset to default!");
    if(millis()-lastTimePress>PUSHTIME){
      for(int i=0; i<100;i++){
        EEPROM.write(i,0);
      }
      EEPROM.commit();
      Serial.println("EEPROM memory erased!");
      delay(2000);
      ESP.restart();
    }
    delay(1000);
  }else{
    lastTimePress=millis();
  }
}

class Config{
public:
  void begin(){
    pinMode(ledPin,OUTPUT);
    pinMode(btnPin,INPUT_PULLUP);
    blinker.attach_ms(50, ledControl);
    EEPROM.begin(100);
    char ssid_temp[32], password_temp[64];
    EEPROM.readString(0,ssid_temp, sizeof(ssid_temp));
    EEPROM.readString(32,password_temp,sizeof(password_temp));
    ssid = String(ssid_temp);
    password = String(password_temp);
    if(ssid!=""){
      Serial.println("Wifi name:"+ssid);
      Serial.println("Password:"+password);
    }
    setupWifi(); //Thiết lập wifi
    if(wifiMode==0) setupWebServer(); //Thiết lập webserver
  }
  void run(){
    checkButton();
    if(wifiMode==0)webServer.handleClient(); //Lắng nghe yêu cầu từ trình client
  }
} wifiConfig;