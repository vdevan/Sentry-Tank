void startOTA()
{
   ArduinoOTA.setHostname(HOSTNAME);
        // No authentication by default
  if (strlen(WIFIPwd) != 0) 
  {
    ArduinoOTA.setPassword(WIFIPwd);
    Serial.printf("OTA Password: %s\n\r", WIFIPwd);
  } 
  else 
  {
    Serial.printf("\r\nNo OTA password has been set! (insecure)\r\n\r\n");
  }
  ArduinoOTA
  .onStart([]() 
  {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
    else // U_SPIFFS
        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        type = "filesystem";
    Serial.println("Start updating " + type);
    // Stop the camera since OTA will crash the module if it is running.
    // the unit will need rebooting to restart it, either by OTA on success, or manually by the user
    Serial.println("Stopping Camera");
    esp_err_t err = esp_camera_deinit();
    strcat(logFile, "OTA Has been started; Camera has Halted!\r\n");
  })
  .onEnd([]() 
  {
    Serial.println("\r\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) 
  {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) 
  {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
        
  ArduinoOTA.begin();
}