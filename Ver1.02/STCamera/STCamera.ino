/* *******************************************************
 * This code is compiled with the board NodeMCU (ESP-12E)
 * The reference book is at H:\ElectronicsAndIoT
 * Esp32OverTheAirAduino.pdf & Esp32OverTheAirAduino.docx
 * Note ESP8266 & ESP32 use different Webserver. ifdef
 * handles this. WebSocket Services library used is from 
 * Markus Sattler.
 * URL for libraries:
 * For WebSockets:
 *     https://github.com/Links2004/arduinoWebSockets
 *
 * OTA - Over The Air is used for compiling and pushing the bin file to ESP 
 *
 * For OTA ElegantOTA is to be considered if the final application is on ESP 32
 * and would be running webserver. An example of this is in ESPCaptiveSSS as well
 * Details of ElegantOTA use the library by Ayush Sharma URL:
 *     https://github.com/ayushsharma82/ElegantOTA - 
 * ElegantOTA is always at localhost:/update. Needs further testing for ESP8266
 * Avoid OTA for ESP8266. Not reliable. Provided here since this will be converted to ESP32-cam
 *
 * For Telnet ESP Telnet by Lennart Hennigs is used. Details at:
 * https://github.com/LennartHennigs/ESPTelnet 
 *
 * We Configure for AP mode. Two modes possible WIFI_STA  and WIFI_AP 
 * In order to use Captive web server (to configure for local network)
 * We need to use AP mode. This is the default mode
.*
 * In Captive mode, the device will reboot the device after 
 * 10 mins of inactivity. This is in case if local network was  
 * busy and could not be connected.
 *
 * Note if compiling for ESP32, PWMRange will be 0-255; slider needs 
 * to be compensated accordingly See the comment on line 306 in MotionControl.ino
 * Last Update 1st June 2024 - Vasu
 ***********************************************************/

#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include <Arduino.h>
#include "soc/soc.h"             // disable brownout problems
#include "soc/rtc_cntl_reg.h"    // disable brownout problems

#include <WebSocketsServer.h> //Socket Service if required
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>           //WiFi info are stored. Up to 6 networks can be stored
#include "Style.h"
#include "ESPTelnet.h"
#include <SPIFFS.h>
#include <FS.h> 
/*#include "SD_MMC.h"
#include <SPI.h>
                              // gives file access
 
#define SD_CS 15 */
#define WHILE_LOOP_DELAY  500   //Connection Wait tim
#define WIFI_TIMEOUT 8         //Time out in secs for Wifi connection.
#define MAXNETWORK 6  

//Used for testing only. Remove it in other programs 
//const int ledPin = 2;

//Header For EEPROM storage and retrieval
#define version 0x0102   //First cut
#define progId 0x0151 // Sum of STWS char map
#define NetworkOffset 100   //We will leave first 100 bytes for Headers

#define CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define BUILTIN_LED       4

uint8_t cam_num;
bool connected = false;

struct NETWORK
{
    char SSID[33];
    char Password[33];
};

NETWORK Networks[MAXNETWORK];

struct HEADER
{
  uint16_t id;
  uint16_t ver; 
} header;

uint8_t storedNetworks = 0;
int storageIndex;

//Network Variables. Minimum Requirement
const char WIFIPwd[] = "pass1234";  //Host password - None
const char HOSTNAME[] = "sentrytank-ws";   //
IPAddress ESPIP(172, 15, 6, 1);  //Defalt server IP. Safe on class B Network 
IPAddress netMask(255, 255, 255, 0); //by using 0 in 4th octet, (254 - 5) network connectivity possible
char ESPSSID[] = "STWS241106";

WebServer server(80);
WebSocketsServer ws = WebSocketsServer(81); //Socket Service if required
WebSocketsServer wsStream = WebSocketsServer(82); //Socket Service for streaming video
bool bConnect = false;
char logFile[3184] = "\0";
char buf[192] = "\0";
bool bCaptive = false;
char Val[16] = "\0";
char Control[16] = "\0";
char retVal[256];
unsigned long timeDelay = 30000; //30 secs will be used in multiples for next fetch
unsigned long resetDelay = timeDelay * 20;//two timedDelay is 1 minute
unsigned long rd; //Restart Captive Server in 10 mins if idle

//Sentry Tank Variables
//Motor Controls
#define PWMRANGE    255
#define PWMMOTOR    15
#define LMOTOR      13
#define RMOTOR      12
#define BLED        14
#define PWMFLASH    7
#define PWMFREQ     50000
#define PWMRES      9
#define PWMMAX      511
#define TELNETPORT  23

uint8_t speed = 255;    
bool bRun = false;
bool bForward = true;
int lumen = 0;
bool bBlink = true;
int bTime = 500;
int cTime;

//For Telnet - if needed
ESPTelnet telnet;

//For Photo Capture
#define PHOTO1 "/Photo_1.jpg"
#define PHOTO2 "/Photo_2.jpg"
#define PHOTO3 "/Photo_3.jpg"
bool bFile = true;

void setup() 
{

  //Tank will be OFF initally

  Serial.begin(115200);
  /*
  if (!SD_MMC.begin("/sdroot", true))
  {     
    logFile += "No SD Card detected";
  }
*/
  analogWrite(PWMMOTOR,0);  
  pinMode(LMOTOR,OUTPUT);
  pinMode(RMOTOR,OUTPUT);
  pinMode(BUILTIN_LED,OUTPUT);
  pinMode(BLED,OUTPUT);
  digitalWrite(LMOTOR, LOW);
  digitalWrite(RMOTOR, LOW);
  digitalWrite(BLED,LOW);
  delay(1000);
  Serial.begin(115200);
  sprintf(buf, "Total heap: %dK\r\n",ESP.getHeapSize()/1000);
  strcat(logFile,buf);
  sprintf(buf,"Free heap: %dK\r\n",ESP.getFreeHeap()/1000);
  strcat(logFile,buf); 
  sprintf(buf, "Total PSRAM: %dK\r\n",ESP.getPsramSize()/1000);
  strcat(logFile,buf); 
  sprintf(buf, "Free PSRAM: %dK\r\n",ESP.getFreePsram()/1000);
  strcat(logFile,buf);
  WiFi.softAPConfig(ESPIP, ESPIP, netMask);
  WiFi.softAP(ESPSSID, WIFIPwd);
  delay(500);


  strcat(logFile,"Trying to read from EEPROM.\r\n");
  if (loadCredentials()) // Load WLAN credentials from EPROM if not valid or no network stored then we need to start webserver
    connectWifi();
  else
  {
    Serial.println("Unable to continue");
    delay(30000);
    ESP.restart();
  }

  server.on("/wifisave", handleWifiSave);
  server.onNotFound(handleNotFound);
  server.on("/capture", HTTP_GET,handleCapturePhoto);
  server.on("/saved-photo", handleSendPhoto);
  
  //Following is for moving from LED to Captive Screen and back
  server.on("/captive",handleCaptive);
  server.on("/control",handleControl);
 
  if (bConnect) //Replace this with your Program
  {
    server.on("/", handleControl);   
  }
  else
  {
    server.on("/",handleCaptive);
    bCaptive = true;
  }

  startServer();
  //startCameraServer();
  //ElegantOTA.begin(&server);
  setupTelnet();

  MDNS.addService("ws", "tcp", 81);
  MDNS.addService("wsStream", "tcp", 82);
  ws.begin();
  ws.onEvent(webSocketEvent);  
    
  wsStream.begin();
  wsStream.onEvent(wsStreamEvent); 
  startOTA();
  if (!SPIFFS.begin(true)) 
  {
    telnet.println("Unable to Open SPIFF\r\n");
    bFile = false;
  }
  else 
    delay(500);    

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //Turn-off brownout detector

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  //Use this for storing Picture. Currently not used
  if(psramFound())
  {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } 
  else
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
    // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) 
  {
    telnet.printf("Camera init failed with error 0x%x", err);
    return;
  }
    
  ledcSetup(PWMFLASH,PWMFREQ,PWMRES);
  ledcAttachPin(BUILTIN_LED,PWMFLASH);
  ledcWrite(PWMFLASH,PWMMAX/2);
  delay(100);
  ledcWrite(PWMFLASH,0);
  digitalWrite(BLED,bBlink ? HIGH : LOW );
}


void loop()
{
  server.handleClient();
  ws.loop(); //Socket Service if required
  wsStream.loop();
  telnet.loop();
  ArduinoOTA.handle();
  if (!bConnect)
  {
    if ((millis() - rd) > resetDelay) //10 minutes over? then restart
    {
      ESP.restart();
    }
  }   
  if(connected)
  {
    liveCam(cam_num);
  }
  /*
  if (millis() > cTime + bTime)
  {
    digitalWrite(BLED,bBlink ? HIGH : LOW );
    bBlink = !bBlink;
    cTime = millis();
  }*/
}
