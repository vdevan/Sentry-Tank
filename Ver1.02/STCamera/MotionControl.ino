const char motion[] = R"raw(
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8" />
    <script src='https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js'></script>
    <title>Tank Control</title>
    <script>
    var websock;
    var bFlash = 0;

 function wait(ms)
  {
    var start = new Date().getTime();
    var end = start;
    while(end < start + ms) 
    {
      end = new Date().getTime();
    }
  }
   /*setInterval(pc, 15000);*/

  window.addEventListener('load', onLoad);
  function onLoad(event) 
  {
    start();
    initButton();
    startStream();
  }

  function startStream()
  {
    webstream = new WebSocket('ws://' + window.location.hostname + ':82/');
    webstream.binaryType = 'arraybuffer';
    if (webstream) 
    {
      webstream.onopen = function (evt) { console.log('webstream open'); };
      webstream.onclose = function (evt) { console.log('webstream close'); };
      webstream.onerror = function (evt) { console.log(evt); };
      webstream.onmessage = function (evt) 
      {
        var bytes = new Uint8Array(evt.data);
        var binary = '';
        var len = bytes.byteLength;
        for (var i = 0; i < len; i++) 
        {
          binary += String.fromCharCode(bytes[i])
        }
        var img = document.getElementById('live');
        img.src = 'data:image/jpg;base64,' + window.btoa(binary);
      }
    }
  }

  function initButton() 
  {
    document.getElementById("btnUp").addEventListener("mousedown", mousePress);
    document.getElementById("btnUp").addEventListener("touchstart", mousePress);
    document.getElementById("btnDown").addEventListener("mousedown", mousePress);
    document.getElementById("btnDown").addEventListener("touchstart", mousePress);
    document.getElementById("btnLeft").addEventListener("mousedown", mousePress);
    document.getElementById("btnLeft").addEventListener("touchstart", mousePress);
    document.getElementById("btnRt").addEventListener("mousedown", mousePress);
    document.getElementById("btnRt").addEventListener("touchstart", mousePress);
    document.getElementById("btnUp").addEventListener("mouseup", mouseup);
    document.getElementById("btnUp").addEventListener("touchend", mouseup);
    document.getElementById("btnDown").addEventListener("mouseup", mouseup);
    document.getElementById("btnDown").addEventListener("touchend", mouseup);
    document.getElementById("btnLeft").addEventListener("mouseup", mouseup);
    document.getElementById("btnLeft").addEventListener("touchend", mouseup);
    document.getElementById("btnRt").addEventListener("mouseup", mouseup);
    document.getElementById("btnRt").addEventListener("touchend", mouseup);
    document.getElementById("btnstop").addEventListener("click", startstop);
    document.getElementById("speed").addEventListener("click",speed);
    document.getElementById("speed").addEventListener("touchend", speed);
    document.getElementById("lumen").addEventListener("touchend", lumen);
    document.getElementById("lumen").addEventListener("click", lumen);
    document.getElementById("flash").addEventListener("click", flash);
    document.getElementById("img1").addEventListener("click", capturePhoto);
    document.getElementById("img2").addEventListener("click", capturePhoto);
    document.getElementById("img3").addEventListener("click", capturePhoto);
  }
  function flash() {
    bFlash = bFlash==0 ? 1 : 0;
    $(this).toggleClass("down");
    console.log("Flash Value: " + bFlash);
  }
  function start()
  {
    websock = new WebSocket('ws://' + window.location.hostname + ':81/');
    websock.onopen = function (evt) { console.log('websock open'); };
    websock.onclose = function (evt) { console.log('websock close'); };
    websock.onerror = function (evt) { console.log(evt); };
    websock.onmessage = function (evt) 
    {
      var result = evt.data;
      console.log(evt);
      if (result[0] == '_')
      {
        var status = result.slice(2,9);
        console.log("Initial Message Received: " + status);
        var sl;
        var img;
        switch (result[1])
        {
        case 'C':
          if (status == "Photo_1")
          {
            sl = document.getElementById("p1");
            sl.src = "/saved-photo?pi=1_" + new Date().getTime();
          }
          else
          {
            if (status == "Photo_2") 
            {
              sl = document.getElementById("p2");
              sl.src = "/saved-photo?pi=2_" + new Date().getTime();
            }
            else
            {
              if (status == "Photo_3") 
              {
                sl = document.getElementById("p3");
                sl.src = "/saved-photo?pi=3_"  + new Date().getTime();
              }
              else
                console.log("Unknown Status = "+ status);
            }
          }
          break;
        case 'M':
          if (status == "Running")
            document.getElementById("StartStop").style.background = 'green';
          else 
            document.getElementById("StartStop").style.background = 'red';
          $("#retVal").text("Current Status: "+ result.slice(2)); 
          break;
        case 'L':
          sl = document.getElementById("lumen");
          sl.value = result.slice(2);
          sl = document.getElementById('lmtxt');
          sl.innerHTML = result.slice(2);
          break;
        case 'S':
          sl = document.getElementById("speed");
          sl.value = result.slice(2);
          sl = document.getElementById("sptxt");
          sl.innerHTML = result.slice(2);
          break;
        }
      }
      else
      {
        if ((result == "Running") || result.includes('Start'))
          document.getElementById("StartStop").style.background = 'green';
        else if ((result == "STOP") || result.includes("Stop"))
          document.getElementById("StartStop").style.background = 'red';
        $("#retVal").text("Current Status: "+ result);
      }
    }
  }

  function mouseup() 
  {
    var name = this.name;
    $("#log").text("Button " + name + ": Released");
    websock.send(name[0] + "_released");
  }
  function mousePress() 
  {
    var name = this.name;
    $("#log").text("Button " + name + ": Pressed");
    websock.send(name[0] + "_pressed");
  }
  function startstop()
  {
    var name = this.name;
    var test = document.getElementById("StartStop").style.background;
    var colr = test;
    console.log("Color: " + colr)
    switch (test) 
    {
        case '':
            colr = 'Stop';
            break;
        case 'red':
            colr = 'Start';
            break;
        case 'green':
            colr = 'Stop';
            break;
    }
    document.getElementById("StartStop").style.background =  colr== 'Start' ? 'green' : 'red';
    websock.send(name + "_" +  colr);
  }
  function speed() 
  {
    var sl = this.id;
    var val = $("#" + sl).val();
    websock.send("S_" + val);
    $("#log").text("Speed Value: " + val);
    $("#" + sl).next().text(val);
  }
  function lumen() 
  {
    var sl = this.id;
    var val = $("#" + sl).val();
    websock.send("M_" + val);
    console.log("Sending: Lumen_" + val);
    $("#log").text("Light Brightness Value: " + val);
    $("#" + sl).next().text(val);
  }
  function capturePhoto() {
    websock.send("C_" + this.name + bFlash);
    console.log("Capture_" + this.name + bFlash);
    /* Use if you want to use GET
    var params = new URLSearchParams();
    params.set('ci', this.name);
    params.set('bf', bFlash);
    console.log("Sending: /capture?" + params);
    var xhr = new XMLHttpRequest();
    xhr.open('GET', '/capture?' + params, true);
    xhr.send();*/
  }
  </script>
  <meta charset="utf-8">
  <title>Tank Control</title>
  )raw";

char TempStub[] PROGMEM = R"raw( 
</head>
<body>
<div class="container">
<h1>Motion Control for Sentry Tank</h1>
<table>
<tr>
<td colspan="2" width="40%">

<div class="btncnt">
<table>
<tr>
<td colspan="3" style="text-align:center">
<button class="btn" name="Up" id="btnUp">
  <div class="btnimg divup"></div>
</button>
</td>
</tr>
<tr>
<td>
<button class="btn" name="Left" id="btnLeft">
  <div class="btnimg divleft"></div>
</button>
</td>
<td>
<button class="btn" name="B" id="btnstop">
  <span class="btnstart" id="StartStop"> </span>
</button>
</td>
<td>
<button class="btn" id="btnRt" name="Right">
  <div class="btnimg divright"></div>
</button>
</td>
</tr>
<tr>
<td colspan="3" style="text-align:center">
<button class="btn" name="Down" id="btnDown">
  <div class="btnimg divdown"></div>
</button>
</td>
</tr>
</table>
</div>
</td>
<td>
<table>
<tr><td><p><span>Feedback:<br /><textarea style="width: 100%; height:100px; word-wrap:normal;" id="retVal"></textarea></span></p></td></tr>
<tr>
<td>
<span>
<button class="btn" onclick="location.href = '/captive'" name="wifi">Wifi Connect</button>
<button id="flash" class="btn">Flash LED</button>
</span>
</td>
</tr>
</table>
</td>
</tr>
<tr>
<td colspan="2"><span>Speed:<br/><input class="sldrclass" id="speed" type="range" min="0" max="255" value="255"  />
<span class="sptxt" id="sptxt">255</span></span></td>
<td colspan="2"><span>Light Brightness:<br/><input class="sldrclass" id="lumen" type="range" min="0" max="255" value="0"  />
<span class="lmtxt" id="lmtxt">0</span></span></td></tr>
</table>

<div id="logtxt"><p>Last Action: <span id="log" class="attn"></span></p></div>
<div style="margin-left:35px;"><img id='live' src=''  width="90%"></div>
<table>
<tr>
<td>
  <div><a href="/saved-photo?pi=4"><img src="/saved-photo?pi=1" id="p1" class="ci"></a></div>
  <p><button class="btn" id="img1" name="1">Capture Photo</button></p>
</td>
<td>
  <div><a href="/saved-photo?pi=5"><img src="/saved-photo?pi=2" id="p2" class="ci"></a></div>
  <p><button id="img2" class="btn" name="2">Capture Photo</button></p>
</td>
<td>
  <div><a href="/saved-photo?pi=6"><img src="/saved-photo?pi=3" id="p3" class="ci"></a></div>
  <p><button id="img3" class="btn" name="3">Capture Photo</button></p>
</td>
</tr>
</table>
</div>
</body>
</html>
)raw";

void handleControl()
{
  server.send(200, "text/html",  String(motion) + String(Style) + String(TempStub));
  strcat(logFile, "Motion Control service Started\r\n"); 
  Serial.println("Index Page provided");
  bCaptive = false;
  return;
}

void wsStreamEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) 
{
  switch (type) 
  {
    case WStype_CONNECTED:
      cam_num = num;
      connected = true;
      break;

    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\r\n", num);
      break;

    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\r\n", num, payload);
      break;

    case WStype_BIN:
      Serial.printf("[%u] get binary length: %u\r\n", num, length);
      break;

    default:
      Serial.printf("Invalid WStype [%d]\r\n", type);
      break;
  }
}

//handle all websock send events here connect Except for WS_EVT_DATA rest are house keeping.
//num is the IPAddress, type Typeof Event, Payload and length of payload. Will need a terminate '0' to payload
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) 
{
  
  sprintf(buf, "webSocketEvent(%d, %d, ...)\r\n", num, type);
  strcat(logFile,buf);
  if (strlen(logFile) >=3000)
  {
    logFile[0] = '\0';
    strcpy(logFile, "Message overflow. Discarding old messages\r\n");
  }
  IPAddress ip = ws.remoteIP(num);
  String msg = bRun ? "_MRunning" : "_MStopped";
  msg += ", Speed set at: " + String(speed);
  msg += ", Tank Motion: ";
  msg += bForward ? "Forward": "Backword";
  msg += ", Light Brightness at: " + String (lumen) + "\n";

  switch (type) 
  {
    case WStype_CONNECTED:
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      sprintf(buf,"[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      //On Connect send the message back to client
      ws.sendTXT(num, msg.c_str(), msg.length()); //num = IpAddress of the client, Text to send, Length of Text
      msg = "_L" + String(lumen);
      ws.sendTXT(num, msg.c_str(), msg.length());
      msg = "_S" + String(speed);
      ws.sendTXT(num, msg.c_str(), msg.length());
      break;

    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\r\n", num);
      sprintf(buf,"[%u] Disconnected!\r\n", num);
      break;

    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\r\n", num, payload);
      sprintf(buf,"[%u] get Text: %s\r\n", num, payload);
      handleWebSocketMessage(num, payload,length); //call the event data to handle message
      break;

    case WStype_BIN:
      Serial.printf("[%u] get binary length: %u\r\n", num, length);
      ws.sendBIN(num, payload, length);//send the message back to client
      break;

    default:
      Serial.printf("Invalid WStype [%d]\r\n", type);
      break;
  }
  strcat(logFile,buf); 
  if (strlen(logFile) >=3000)
  {
    logFile[0] = '\0';
    strcpy(logFile, "Message overflow. Discarding old messages\r\n");
  }
}

void notifyClients() 
{
  if (bForward)
    strcat(retVal, "\r\nVehicle motion set for: Forward");
  else
    strcat(retVal, "\r\nVehicle motion set for: Backward");

  ws.broadcastTXT(retVal,strlen(retVal));
  sprintf(buf,"Notification sent to cient: %s\r\n",retVal);
  strcat(logFile,buf);
  if (strlen(logFile) >= 3000)
  {
    logFile[0]='\0';
    strcpy(logFile, "Message Overload. Old Messages deleted\r\n");
  }
}

void handleWebSocketMessage(uint8_t num, uint8_t *data, size_t len)
{
  buf[0]='\0';
  retVal[0]='\0';
  IPAddress ip = ws.remoteIP(num); 
  Serial.println("Address of the client sending the message: ");
  Serial.println(ip);
  sprintf(buf,"Message Receeived from: %s\r\n",ip.toString().c_str());
  strcat(logFile,buf);
  digitalWrite(BLED,bBlink ? HIGH : LOW );
  bBlink = !bBlink;
  //check for end of stream. This is required as it is a Websocket message and not POST
  data[len] = 0; //terminate the string
  telnet.printf("Data Received: %s\r\n",data);
  strcat(logFile,(char*)data);
  strcat (logFile, " Message Received\r\n");
  
  if (strlen(logFile) >=3000)
  {
    logFile[0] = '\0';
    strcpy(logFile,"Message overflow. Discarding old messages\r\n");
  }
  int offset = -1;
  for (int i =0; i < len +1; i++)
  {
    if (data[i] == 95) // '_' ascii value
    {
      offset = i;
      break;
    }
  }
  char cnt;
  if (offset >= 0)
  {
    cnt = (char)data[0];
    //strncpy(Control,(char*)data,offset);
    //Control[offset]='\0';
    strcpy(Val, (char*)&data[offset+1]);
    sprintf(buf,"Offset: %d; Control: %c & Value: %s\r\n",offset,cnt,Val);
    strcat(logFile,buf);

    if (cnt == 'C')
    {
      bool btemp = bRun;
      bRun = false;
      analogWrite(PWMMOTOR,0); 
      sprintf(buf,"Photo Captured on Slot: %c, Flash: %c",Val[0],Val[1]);
      strcpy(retVal, buf);
      telnet.println(buf);
      notifyClients();
      strcpy(retVal,"_C");
      bool bFlash = Val[1]=='1' ? true : false;
      if (Val[0] == '1') 
      {
        capturePhoto(PHOTO1,bFlash);
        strncat(retVal,&PHOTO1[1],7);
      }
      else
      {
        capturePhoto((char*)(Val[0] == '2' ? PHOTO2 : PHOTO3),bFlash);  
        strncat(retVal, Val[0] == '2' ? &PHOTO2[1] : &PHOTO3[1],7);
      }
      retVal[strlen(retVal)] = '\0';
      notifyClients();
      bRun=btemp;
      bRun == true ? analogWrite(PWMMOTOR,speed) : analogWrite(PWMMOTOR,0);
      return;
    }
    
    if (cnt == 'M')
    {
      sprintf(buf,"Light Brightness adjusted to %s",Val);
      strcpy(retVal, buf); 
      lumen = atoi(Val);
      ledcWrite(PWMFLASH,lumen);
      notifyClients();
      return;
    }
    
    if (cnt == 'S')
    {
      sprintf(buf,"Speed adjusted to %s", Val);
      strcpy(retVal, buf); 
      speed = atoi(Val);
      Serial.printf("Speed: %s\n",Val);
      if (bRun)
      {
        analogWrite(PWMMOTOR,speed);
        strcat(retVal,"-Running");
      }
      else
      {
        analogWrite(PWMMOTOR,0); 
        strcat(retVal, "-Not Running");
      }
      notifyClients();
      return;
    }

    if (cnt == 'U')
    {
      analogWrite(PWMMOTOR,0); //Let us stop first
      strcpy(retVal,"Forward button ");
      strcat(retVal, Val);
      if (strcmp(Val,"pressed") == 0)
      {
        digitalWrite(LMOTOR,HIGH);
        digitalWrite(RMOTOR,HIGH);
        bForward = true;
        analogWrite(PWMMOTOR,speed);
        strcat(retVal, "-Running");
      }
      else
      {
        bRun == true ? analogWrite(PWMMOTOR,speed) : analogWrite(PWMMOTOR,0); 
        strcat(retVal, bRun ? "-Running" : "-Not Running");
      }
      notifyClients();
      return;
    }

    if (cnt =='D')
    {
      analogWrite(PWMMOTOR,0); //Let us stop first
      strcpy(retVal, "Backward button " );
      strcat(retVal,Val);
      if (strcmp(Val, "pressed")==0)
      {
        digitalWrite(LMOTOR,LOW);
        digitalWrite(RMOTOR,LOW);
        bForward = false;
        analogWrite(PWMMOTOR,speed);
        strcat(retVal,"-Running");
      }
      else
      {
        bRun == true ? analogWrite(PWMMOTOR,speed) : analogWrite(PWMMOTOR,0); 
        strcat(retVal, bRun ? "-Running" : "-Not Running");
      }
      notifyClients();
      return;
    }

    if (cnt =='R')
    {
      analogWrite(PWMMOTOR,0); //Let us stop first
      strcpy(retVal,"Right button ");
      strcat(retVal,Val);
      if (strcmp(Val, "pressed") == 0)
      {
        digitalWrite(RMOTOR,HIGH);
        digitalWrite(LMOTOR,LOW);
        analogWrite(PWMMOTOR,speed);
        strcat(retVal,"-Running");      
      }
      else
      {
        if (bForward)
        {
          digitalWrite(LMOTOR,HIGH);
          digitalWrite(RMOTOR,HIGH);
        }
        else
        {
          digitalWrite(LMOTOR,LOW);
          digitalWrite(RMOTOR,LOW);
        }      
        bRun == true ? analogWrite(PWMMOTOR,speed) : analogWrite(PWMMOTOR,0); 
        strcat(retVal, bRun ? "-Running" : "-Not Running");
      }
      notifyClients();
      return;
    }

    if (cnt == 'L')
    {
      analogWrite(PWMMOTOR,0); //Let us stop first
      strcpy(retVal,"Left button ");
      strcat(retVal,Val);
      if (strcmp(Val, "pressed") ==0)
      {
        digitalWrite(LMOTOR,HIGH);
        digitalWrite(RMOTOR,LOW);
        analogWrite(PWMMOTOR,speed);
        strcat(retVal,"-Running");
      }
      else
      {
        if (bForward)
        {
          digitalWrite(LMOTOR,HIGH);
          digitalWrite(RMOTOR,HIGH);
        }
        else
        {
          digitalWrite(LMOTOR,LOW);
          digitalWrite(RMOTOR,LOW);
        }
        bRun == true ? analogWrite(PWMMOTOR,speed) : analogWrite(PWMMOTOR,0); 
        strcat(retVal, bRun ? "-Running" : "-Not Running");
      }    
      notifyClients();
      return;
    }

    telnet.printf("Value: %s",Val);
    if (strcmp(Val,"Stop")==0)
    {
      bRun = false;    
      Serial.printf("From STOP: Control: %s, Value: %s, Speed: %d Run: %d\n",Control,Val,speed,bRun);
      analogWrite(PWMMOTOR, 0);
      strcpy(retVal,"Stop Requested and Stopped");
      notifyClients();
      return;
    }

    if (strcmp(Val,"Start")==0)
    {
      bRun = true;
      Serial.printf("From START: Control: %s, Value: %s, Speed: %d Run: %d\n",Control,Val,speed,bRun);
      if (bForward)
      {
        digitalWrite(LMOTOR,HIGH);
        digitalWrite(RMOTOR,HIGH);
        strcat(retVal,"Start Requested and is in Forward motion");
      }
      else
      {
        digitalWrite(LMOTOR,LOW);
        digitalWrite(RMOTOR,LOW);
        strcat(retVal, "Start Requested and is in Backward motion");
      }
      analogWrite(PWMMOTOR, speed);
      notifyClients();
      return;
    }

  }
  strcat(logFile,"Invalid Message Received. Discarding\r\n");
  strcpy(retVal,"Invalid Message Received. Discarding");
  notifyClients();
  return;
}

void liveCam(uint8_t num)
{
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) 
  {
      telnet.println("Frame buffer could not be acquired");
      return;
  }
  wsStream.sendBIN(num, fb->buf, fb->len);
  esp_camera_fb_return(fb);
}



// Check if photo capture was successful
bool checkPhoto( fs::FS &fs,String fileName ) 
{
  File f_pic = fs.open(fileName);
  unsigned int pic_sz = f_pic.size();
  return ( pic_sz > 100 );
}

void handleCapturePhoto()
{
  if (server.hasArg("ci"))
  {  
    analogWrite(PWMMOTOR,0); //Let us stop first
    telnet.printf("Capture Photo Slot: %s; Flash Value: %s\r\n",server.arg("ci").c_str(),server.arg("bf").c_str());
    char st = server.arg("ci").charAt(0);
    bool bFlash = server.arg("bf")=="1" ? true : false;
    if (st=='1')
      capturePhoto(PHOTO1,bFlash);
    else if (st=='2')  
      capturePhoto(PHOTO2,bFlash);
    else if (st=='3')  
      capturePhoto(PHOTO3,bFlash);
    else
    {
      telnet.println("Nothing to process");
      return;
    }
    sendResponse();
    bRun == true ? analogWrite(PWMMOTOR,speed) : analogWrite(PWMMOTOR,0); 
  }
  else
    telnet.printf("Server Argument Received: %d\r\n",server.args());
  
}

// Capture Photo and Save it to SPIFFS
void capturePhoto(char* fileName,bool bFlash ) 
{
  telnet.printf("File Name Passed: %s and Flash Value: %d\r\n",fileName,bFlash);
  if (!bFile)
    return;
  camera_fb_t * fb = NULL; // pointer
  bool ok = 0; // Boolean indicating if the picture has been taken correctly
  int size;
  if (bFlash)
    ledcWrite(PWMFLASH,PWMMAX/2);
  do 
  {
    fb = esp_camera_fb_get();
    //ledcWrite(PWMFLASH,lumen);
    if (!fb) 
    {
      telnet.println("Camera capture failed");
      return;
    }

    // Photo file name
    File file = SPIFFS.open(fileName, FILE_WRITE);

    // Insert the data in the photo file
    if (!file) 
    {
      telnet.printf("Failed to open file: %s in writing mode\r\n",fileName);
      break;
    }
    else 
    {
      file.write(fb->buf, fb->len); // payload (image), payload length
      
    }
    // Close the file
    size = file.size()/1000;
    file.close();
    esp_camera_fb_return(fb);

    // check if file has been correctly saved in SPIFFS
    ok = checkPhoto(SPIFFS,fileName);
  } 
  while ( !ok );
  ledcWrite(PWMFLASH,lumen);
  telnet.printf("The picture has been saved in %s; Size: %d Kbytes\r\n",fileName,size);
  ws.broadcastTXT("_C",2);
}

void handleSendPhoto()
{
  buf[0] = '\0'; //contains Header
  Control[0] = '\0'; //contains fileName
  size_t len = 0;
  telnet.printf("Server Argument Photo 1: %s\r\n",server.arg("pi").c_str());
 
  if (server.hasArg("pi"))
  {  
    telnet.printf("Server Argument @ Photo 1: %s\r\n",server.arg("pi").c_str());
    char st = server.arg("pi").charAt(0);
    switch (st)
    {
      case '1':
        strcpy (buf,"inline;");
        strcat(buf, PHOTO1);
        strcpy (Control, PHOTO1);
        break;
      case '2':
        strcpy (buf,"inline;");
        strcat(buf, PHOTO2);
        strcpy (Control, PHOTO2);
        break;
      case '3':
        strcpy (buf,"inline;");
        strcat(buf, PHOTO3);
        strcpy (Control, PHOTO3);
        break;
      case '4':
        strcpy (buf,"attachment; filename=");
        strcat(buf, &PHOTO1[1]);
        strcpy (Control, PHOTO1);
        break;
      case '5':
        strcpy (buf,"attachment; filename=");
        strcat(buf, &PHOTO2[1]);
        strcpy (Control, PHOTO2);    
        break;
      case '6':
        strcpy (buf,"attachment; filename=");
        strcat(buf, &PHOTO3[1]);
        strcpy (Control, PHOTO3);        
        break;        
      default:
        return;  
    }
    File rfile = SPIFFS.open(Control, FILE_READ);
    server.sendHeader("Content-Length",String(rfile.size()));
    server.sendHeader("Content-Disposition",buf);
    server.streamFile(rfile,  "application/octet-stream");
    rfile.close();
    buf[0] = '\0'; 
    Control[0] = '\0'; 
  }
}


