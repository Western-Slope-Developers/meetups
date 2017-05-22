#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <stdlib.h>


int status = WL_IDLE_STATUS;

#include "auth.h"

/* 'auth' file has these lines
  const char *ssid      = "????";
  const char *password  = "????";

  #define LOCATION        "????"

*/

const int UDP_PACKET_SIZE = 128;
char udpInBuffer[UDP_PACKET_SIZE];
char udpOutBuffer[UDP_PACKET_SIZE];

#define MILLIINTERVAL 10

#define LOCATION "plate"

WiFiUDP Udp;
IPAddress ipUdpMulti(239, 0, 0, 57);
unsigned int ipUdpPort = 12345;

int debug = 0;
int c;
unsigned long now;

float valueSIN;
unsigned valueADC;

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("\n\nConnecting to ");
  Serial.print(ssid);
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tries++;
    if (tries > 30) {
      break;
    }
  }
  Serial.println();
  printNetworkStatus();
  Udp.beginMulticast(WiFi.localIP(),  ipUdpMulti, ipUdpPort);
}

void getValues() {
  valueSIN = sin(now/100.0);
  valueADC = analogRead(A0);  
}

void loopBroadcast()
{
  if ((millis() % MILLIINTERVAL) == (MILLIINTERVAL - 1)) { // are we near next broadcast time?
    while (now = millis()) {
      if ((now % MILLIINTERVAL) == 0) {
        getValues();
        break;
      }
    }
    char v[20];
    dtostrf(valueSIN,0,4,v);
    sprintf(udpOutBuffer,"{\"type\":\"data\",\"name\":\"%s\",\"t\":%d,\"v\":%s,\"adc\":%d}",LOCATION,now,v,valueADC);

    Udp.beginPacketMulticast(ipUdpMulti, ipUdpPort, WiFi.localIP());
    Udp.write(udpOutBuffer, strlen(udpOutBuffer));
    Udp.endPacket();
  }
}

void loopIncomingProcess()
{
  int numBytes = Udp.parsePacket();
  if ( numBytes ) {
    if (debug) {
      Serial.print(millis() / 1000);
      Serial.print(":Packet of ");
      Serial.print(numBytes);
      Serial.print(" received from ");
      Serial.print(Udp.remoteIP());
      Serial.print(":");
      Serial.println(Udp.remotePort());
    }

    Udp.read(udpInBuffer, numBytes);

    udpInBuffer[numBytes] = 0;
    Serial.println(udpInBuffer);
  }
}

void printNetworkStatus() {
  Serial.print("SSID:          ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address:    ");
  Serial.println(ip);

  Serial.print("UDP multicast: ");
  Serial.print(ipUdpMulti);
  Serial.print(":");
  Serial.println(ipUdpPort);
}

void loop() {
  loopBroadcast();
  loopIncomingProcess();
}

