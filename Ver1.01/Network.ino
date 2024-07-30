//Check the stored networks and scanned networks. try to connect all available network
//if connected storageIndex is properly set.
void connectWifi()
{
  //Serial.println("Connecting as wifi client...");
  int n = WiFi.scanNetworks();
  storageIndex = -1;
  int j;
  //Serial.printf("Total Scanned network: %d\n", n);
  //Check if scannednetworks is available in our storage
  for (int i = 0; i < storedNetworks; i++)
  {
    for (j = 0; j < n; j++)
    {
      Serial.printf("Now checking the SSID %s at storage index %d with Scanned Network: %s\n",
            Networks[i].SSID, i, WiFi.SSID(j).c_str());

      if (WiFi.SSID(j).compareTo(String(Networks[i].SSID)) == 0)
      {
          storageIndex = i;
          break;
      }
    }
    if (j < n)  //storageIndex >= 0
    {
      if (tryConnect())
          break;
    }
  }

  if (bConnect)
  {          
    Serial.printf("Connected to Network and Storage Index: %d\n", storageIndex);
    sprintf(buf,"Connected to Network and Storage Index: %d\r\nNetwork Name: %s\r\n", storageIndex,Networks[storageIndex].SSID);
    strcat(logFile, buf);
  }
  else
  {
    Serial.println("Not Connected to Network");
    strcat(logFile, "Unable to connect to Local Area Network\r\n");
  }

  return;
}

//First 
bool tryConnect()
{
  bConnect = false;
  Serial.printf("Trying to connect with SSID: %s, Password: %s\n", Networks[storageIndex].SSID, Networks[storageIndex].Password);

  WiFi.begin(Networks[storageIndex].SSID, Networks[storageIndex].Password);

  int status = WiFi.status();
  int startTime = millis();
  while (status != WL_CONNECTED && status != WL_NO_SSID_AVAIL && status != WL_CONNECT_FAILED && (millis() - startTime) <= WIFI_TIMEOUT * 1000)
  {
      delay(WHILE_LOOP_DELAY);
      status = WiFi.status();
  }

  if (WiFi.status() == WL_CONNECTED)
      bConnect = true;

  Serial.printf("StoredIndex: %d; Connection status of SSID: %s is %d\n", storageIndex, Networks[storageIndex].SSID, WiFi.status());
  return bConnect;
}

//Server start when connection fails
void startServer()
{
  server.begin(); // Web server start
  Serial.println("HTTP server started");

  // Setup MDNS responder
  if (!MDNS.begin(HOSTNAME))
      Serial.println("Error setting up MDNS responder!");
  else
  {
      Serial.println("mDNS responder started");
      MDNS.addService("http", "tcp", 80);
  }
  if (!bConnect)
    rd = millis();
}

