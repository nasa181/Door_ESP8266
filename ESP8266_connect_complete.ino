/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial
uint8_t state;
uint8_t next;
ESP8266WiFiMulti WiFiMulti;
HTTPClient http;
char _ssid[20] = "sigma-desktop";//len 13
char _password[20] = "AphW73qq";//len 8
char * ssid = _ssid;
char * password = _password;
char _web_address[256] = "http://nodemcu.com/index_en.html";//len 32
char * web_address = _web_address;

void change_WIFI(void);
void set_up_http(){
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
        http.begin(web_address); //HTTP for get function
    }
}
void get_http(){
        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    
  
}
void connect_to_wifi(){
  Serial.println(ssid);
  Serial.println(password);
  WiFiMulti.addAP(ssid, password);
}
void scan_WIFI(){
      Serial.println("scan start");
  
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0)
      Serial.println("no networks found");
    else
    {
      Serial.print(n);
      Serial.println(" networks found");
      for (int i = 0; i < n; ++i)
      {
        // Print SSID and RSSI for each network found
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(")");
        Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
        
        delay(10);
        
      }
    }  
  
}






void setup() {

    USE_SERIAL.begin(115200);
   // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    //WiFiMulti.addAP("sigma-desktop", "AphW73qq");
    WiFiMulti.addAP(ssid, password);

}
uint8_t input_char = 0;
void loop() {

  switch(state){
    case 0 :
      if(Serial.available())next = 3;
      else next = 1;
      set_up_http();
      break;
    case 1 :
      if(Serial.available())next = 3;
      else next = 2;
      get_http();
      break;
    case 2 :
      Serial.println("press any key to change a config");  
      if(Serial.available()){
        next = 3;
      }
      else {
        next = 0;
        delay(5000);
        }
      break;
//=============================== Since case 3 do the config =================================================================      
    case 3 :
      while(Serial.available())Serial.read();
      Serial.println("Press \"1\" to change which web address to get API or type \"2\" to access others wifi");
      Serial.println("or type \"0\" to cancel");
      delay(1000);
      next = 4;
      break;

    case 4 :
      if(Serial.available()){
        uint8_t input = Serial.read();
        if(input == '1')next = 22;
        else if(input == '2')next = 5;
        else if(input == '0')next = 0;
        else next = 3;
      }
      else next = 4;
      break;
//=================================== change wifi ===========================================
    case 5 :
      Serial.println("now will scan the wifi");
      next = 6;
      break;
    case 6 :
      scan_WIFI();
      next = 7;
      break;
    case 7 :
      delay(1000);
      next = 8;
      break;
    case 8 :
       Serial.println("Plase press \"1\" for select a SSID or scan again by pressing \"2\" or cancel by pressing \"0\"");
       while(Serial.available())Serial.read();
       delay(1000);
       next = 9;
       break;
    case 9 :
      if(Serial.available()){
        uint8_t input = Serial.read();
        if(input == '1')next = 10;
        else if(input == '2')next = 5;
        else if(input == '0')next = 0;
        else next = 8;
      }
      else next = 9;
      break;
    case 10 :
      Serial.println("Please enter a new SSID");
      Serial.println("The SSID must end with \".\"");
      while(Serial.available())Serial.read();
      delay(1000);
      input_char = 0;
      next = 11;
      break;
    case 11 :
      if(Serial.available() && input_char < 18){
        uint8_t input1 = Serial.read();
        if(input1 == '.')next = 12;
        else{
          _ssid[input_char] = input1;
          input_char++;
          next = 11;
        }  
      }
      else next = 11;
      break;
    case 12 :
      _ssid[input_char] = '\0';
      Serial.println(input_char);
      Serial.println(ssid);
      next = 13;
      input_char = 0;
      break;
    case 13 :
      Serial.println("Is the SSID has a password?");
      Serial.println("Press \"1\" for enter the password or press \"2\" if no need to enter the password ");
      while(Serial.available())Serial.read();
      delay(2000);
      next = 14;
      break;
    case 14 :
      if(Serial.available()){
        uint8_t hasPassword = Serial.read();
        if(hasPassword == '1')next = 15;//has password
        else if(hasPassword == '2')next = 21;//no password
        else next = 13;
        while(Serial.available())Serial.read();  
      }
      else next = 14;
      break;
    case 15 :
      Serial.println("Please enter the password");
      Serial.println("The password mush end with \".\"");
      input_char = 0;
      next = 16;
      break;
    case 17 :
    if(Serial.available() && input_char < 18){
        uint8_t input1 = Serial.read();
        if(input1 == '.')next = 18;
        else{
          _password[input_char] = input1;
          input_char++;
          next = 17;
        }  
      }
      else next = 17;
      break;
    case 18 :
      _password[input_char] = '\0';
      Serial.println(input_char);
      input_char = 0;
      next = 19;
      Serial.println(password);
      break;
    case 19 :
      delay(100);
      ssid = _ssid;
      password = _password;
      next = 20;
      break;
    case 20 :
      connect_to_wifi();
      delay(100);
      next = 0;
      break;
    case 21 :
      _password[0] = '\0';
      Serial.println("now is case 21");
      next = 19;
      break;
//======================================== change wed address =============================================
    case 22 :
      Serial.println("Please type a new web address(Maximum langth = 254) or cancel by typing \"0\" ");
      Serial.println("(Example) http://www.example.com");
      Serial.println("The end of web address must be \".\"");
      input_char = 0;
      delay(1000);
      next = 23;
      break;
    case 23 :
      if(Serial.available()){
        if(Serial.read() == '0')next = 0;
        else{
         input_char = 0;
         next = 24; 
        }
      }
      else next = 23;
      break;
    case 24 :
      if(Serial.available()){
        uint8_t in_web = Serial.read();
        if(input_char >= 255){
          next = 22;
          Serial.println("The length is more than 254 charectors");
          input_char = 0;
        }
        if(in_web == '.'){
          _web_address[input_char] = '\0';
          next = 25;
        }
        else{
          _web_address[input_char] = in_web;
          input_char++;
          next = 24;
        }
      }
      else next = 24;
      break;
    case 25 :
      Serial.println(web_address);
      Serial.println(input_char);
      input_char = 0;
      next = 26;
      break;
    case 26 :
      delay(1000);
      Serial.println(web_address);
      next = 0;
      break;
    default:
      break;
  }
  if(next > 26)next = 0;
  state = next;
}

