#include <ESP8266WiFi.h>

const int PIN_BUTTON = 16;

WiFiServer server(80);
const char* SSID_NAME = "ESP8266WiFi";

bool inConfigMode = false;
bool wasInConfigMode = false;

struct network {
  String ssid;
  int32_t rssi;
  uint8_t encryptionType;
};
struct scan {
  unsigned int ssidCount;
  network * networks;
};

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
  scan scanResult = fetchNetworks();
  listNetworks(scanResult);

  Serial.println("looking for clients");
  for(unsigned int x=0 ; x < 10000; x++){
    WiFiClient client = server.available(); 
    if(x % 1000 == 0)
    {
      Serial.print(".");
    }
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
            client.print(prepareHtmlPage(scanResult));
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
}

String prepareHtmlPage(scan scanResult)
{
  String list = listNetworks(scanResult);
  list += "</ul>";
  String htmlPage = 
     String("HTTP/1.1 200 OK\r\n") +
            "Content-Type: text/html\r\n" +
            "Connection: close\r\n" +  // the connection will be closed after completion of the response
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


scan fetchNetworks() {
  // scan for nearby networks:
  scan result;
  unsigned int ssidCount = WiFi.scanNetworks();
  result.ssidCount = ssidCount;
  result.networks = new network[result.ssidCount];
  
  for (int ssidNum = 0; ssidNum < ssidCount; ssidNum++) {
    network nw;
    nw.ssid = WiFi.SSID(ssidNum);
    nw.rssi = WiFi.RSSI(ssidNum);
    nw.encryptionType = WiFi.encryptionType(ssidNum);
    result.networks[ssidNum] = nw;
  }

  return result;
}

String listNetworks(scan scanResult) {
  String list = String("<ul>"); 
  Serial.println();
  for( unsigned int x = 0; x < scanResult.ssidCount; x++ ){
    network nw = scanResult.networks[x];
    list += "(";
    list += x;
    list +=") ";
    list += nw.ssid;
    list +="signal: ";
    list += nw.rssi;
    list += "dBm Encryption: ";
    list += encryptionTypeToString(nw.encryptionType);
    Serial.print(x);
    Serial.print(") ");
    Serial.print(nw.ssid);
    Serial.print("\tSignal: ");
    Serial.print(nw.rssi);
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    Serial.println(encryptionTypeToString(nw.encryptionType)); 
  }
  list += "</ul>";
  return list;
}

String encryptionTypeToString(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      return "WEP";
      break;
    case ENC_TYPE_TKIP:
      return "WPA";
      break;
    case ENC_TYPE_CCMP:
      return "WPA2";
      break;
    case ENC_TYPE_NONE:
      return "None";
      break;
    case ENC_TYPE_AUTO:
      return "Auto";
      break;
  }
}
