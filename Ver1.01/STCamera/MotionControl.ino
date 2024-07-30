const char motion[] = R"raw(
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8" />
    <script src='https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js'></script>
    <title>Tank Control</title>
    <script>
    var websock;

  function wait(ms)
  {
    var start = new Date().getTime();
    var end = start;
    while(end < start + ms) 
    {
      end = new Date().getTime();
    }
  }

  //setInterval(pc, 10000);

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
        var status = result.slice(2,7);
        console.log("Initial Message Received: " + result.slice(2));
        var sl;
        switch (result[1])
        {
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
      websock.send(name + "_released");
  }
  function mousePress() 
  {
      var name = this.name;
      $("#log").text("Button " + name + ": Pressed");
      websock.send(name + "_pressed");
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
      websock.send("Speed_" + val);
      $("#log").text("Speed Value: " + val);
      $("#" + sl).next().text(val);
  }
  function lumen() 
  {
      var sl = this.id;
      var val = $("#" + sl).val();
      websock.send("Lumen_" + val);
      $("#log").text("Light Brightness Value: " + val);
      $("#" + sl).next().text(val);
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
<button class="btn" name="Stop" id="btnstop">
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
  //String sdat = (char*)data;
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
  //int offset = sdat.indexOf('_'); 
  if (offset >= 0)
  {
    strncpy(Control,(char*)data,offset);
    Control[offset]='\0';
    strcpy(Val, (char*)&data[offset+1]);
    sprintf(buf,"Offset: %d; Control: %s & Value: %s\r\n",offset,Control,Val);
    strcat(logFile,buf);
    
    if (strcmp(Control,"Lumen") == 0)
    {
      sprintf(buf,"Light Brightness adjusted to %s",Val);
      strcpy(retVal, buf); 
      lumen = atoi(Val);
      ledcWrite(PWMFLASH,lumen);
      notifyClients();
      return;
    }
    
    if (strcmp(Control, "Speed") == 0)
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

    if (strcmp(Control,"Up") == 0)
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

    if (strcmp(Control, "Down") == 0)
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

    if (strcmp(Control, "Right") ==0)
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

    if (strcmp(Control,"Left") == 0)
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
      Serial.println("Frame buffer could not be acquired");
      return;
  }
  wsStream.sendBIN(num, fb->buf, fb->len);
  esp_camera_fb_return(fb);
}
