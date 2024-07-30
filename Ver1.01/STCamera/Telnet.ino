void setupTelnet() 
{  
  telnet.setLineMode(false);
  // passing on functions for various telnet events
  telnet.onConnect(onTelnetConnect);
  telnet.onConnectionAttempt(onTelnetConnectionAttempt);
  telnet.onReconnect(onTelnetReconnect);
  telnet.onDisconnect(onTelnetDisconnect);
  telnet.onInputReceived(onTelnetInput);

  Serial.print("- Telnet: ");

  if (telnet.begin(TELNETPORT)) 
  {
    Serial.println("running");
  }
  else 
  {
    Serial.println("error.");
    Serial.println("Telnet not running. reboot may be required...");
  }
}

void onTelnetDisconnect(String ip)
{
  Serial.print(ip);
  Serial.println(" disconnected.");
}

void onTelnetReconnect(String ip) 
{
  Serial.print(ip);
  Serial.println(" connected.");
}

void onTelnetConnectionAttempt(String ip) 
{
  Serial.print("- Telnet: ");
  Serial.print(ip);
  Serial.println(" tried to connect");
}

void onTelnetInput(String str) 
{
  char r = str.charAt(0);
  telnet.println("");
  switch (r) 
  {
    case '?':
      printMenu();
      break;

    case 'R':
      telnet.println("Rebooting in 5 seconds");
      telnet.disconnectClient();
      delay(5000);
      ESP.restart();
      break;

    case 'l':
      if (logFile == "\0")
        telnet.println("Nothing to Report\r\n");
      else
        telnet.println(logFile);
      break;

    case 's':
      telnet.print("For Sentry Tank Control point your browser to http://");
      telnet.print(WiFi.localIP());
      telnet.println("/control to connect");
      break;

    case 'w':

      telnet.print("For WiFi mode point your browser to http://");
      telnet.print(WiFi.localIP());
      telnet.println("/captive to connect");
      break;

    case 'x':
      telnet.println("");
      telnet.println("bye");
      telnet.disconnectClient();
      break;
  }
  return;
  
}

void onTelnetConnect(String ip) 
{
  telnet.print("Welcome - Client: ");
  telnet.print(ip);
  telnet.println(" serving now");
  printMenu();
}

void printMenu()
{
  if (bCaptive)
  {
    telnet.println("Currently In WiFi Mode.");
    telnet.print("Point your browser to http://");
    telnet.print(WiFi.localIP());
    telnet.println("/captive to connect");
  }
  else
  {
    telnet.println("Sentry Tank Ready");
    telnet.println("=================");
    telnet.print("Point your browser to http://");
    telnet.print(WiFi.localIP());
    telnet.println("/control to control the Tank");
  }  
  telnet.println("l.  Print Log File");
  telnet.println("s.  Instruction for Sentry Tank Control mode");
  telnet.println("w.  Instruction for WiFi (Captive) mode");
  telnet.println("x.  Exit session");
  telnet.println("?.  Print This Menu");
  telnet.println("R.  Restart ESP device");
  telnet.println("===========================");
}
