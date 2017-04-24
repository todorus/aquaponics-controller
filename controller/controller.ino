#include <ESP8266WiFi.h>

const int PIN_BUTTON = 16;

WiFiServer server(80);
const char* SSID_NAME = "ESP8266WiFi";

bool inConfigMode = false;
bool wasInConfigMode = false;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BUTTON, INPUT);
  
  Serial.println("Mode = running");
}

void loop(){
  inConfigMode = inConfigMode || digitalRead(PIN_BUTTON);
  
  if(wasInConfigMode != inConfigMode){
    if(inConfigMode){
      setupAp();
      printWifiStatus();
      Serial.println("looking for client");
    }
    wasInConfigMode = inConfigMode;
  }
  if(inConfigMode){
    configMode();
  }
}

void setupAp()
{
  Serial.println("Mode = config");
  server.begin();
  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID_NAME);

  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
}

void configMode(){
  WiFiClient client = server.available(); 
  Serial.print(".");
  if (client)
  {
    // we have a new client sending some request
    Serial.println("clientfigmode");
    
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          client.print(prepareHtmlPage());
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

String prepareHtmlPage()
{
  String htmlPage = 
     String("HTTP/1.1 200 OK\r\n") +
            "Content-Type: text/html\r\n" +
            "Connection: close\r\n" +  // the connection will be closed after completion of the response
            "Refresh: 5\r\n" +  // refresh the page automatically every 5 sec
            "\r\n" +
            "<!DOCTYPE HTML>" +
            "<html>" +
            "Hello world" +
            "</html>" +
            "\r\n";
  return htmlPage;
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
