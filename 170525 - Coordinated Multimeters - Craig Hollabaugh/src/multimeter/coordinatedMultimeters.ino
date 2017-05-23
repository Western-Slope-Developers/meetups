#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <stdlib.h>
#include <ArduinoJson.h>

// expressif sdk include, also link
extern "C" {
#include "user_interface.h"
}

// extern uint32_t micros_at_last_overflow_tick;
// extern uint32_t micros_overflow_count;

static uint32_t micros_at_last_overflow_tick;
static uint32_t micros_overflow_count;

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

#define BROADCASTINTERVAL 10
#define SYNCHOLDOFF 5000

#define LOCATION "plate"

WiFiUDP Udp;
IPAddress ipUdpMulti(239, 0, 0, 57);
unsigned int ipUdpPort = 12345;

int debug = 0;
int c;
unsigned long now;

float valueSIN;
unsigned valueADC;

uint32_t offsetMillis, syncMilli, delayMilli;

StaticJsonBuffer<200> jsonBuffer;

// from core_esp8266_wiring.c
unsigned long ICACHE_RAM_ATTR millissync() {
  uint32_t m = system_get_time();
  uint32_t c = micros_overflow_count + ((m < micros_at_last_overflow_tick) ? 1 : 0);
  return c * 4294967 + m / 1000 + offsetMillis;
}

// see https://github.com/esp8266/Arduino/issues/3078
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
  valueSIN = sin(now / 100.0);
  valueADC = analogRead(A0);
}

void loopBroadcast()
{
  if ((millissync() % BROADCASTINTERVAL) == (BROADCASTINTERVAL - 1)) { // are we near next broadcast time?
    while (now = millissync()) {
      if ((now % BROADCASTINTERVAL) == 0) {
        // Serial.println(now); // this kills timing performance
        getValues();
        break;
      }
    }
    char v[20];
    dtostrf(valueSIN, 0, 4, v);
    sprintf(udpOutBuffer, "{\"type\":\"data\",\"src\":\"%s\",\"t\":%d,\"v\":%s,\"adc\":%d}", LOCATION, now, v, valueADC);

    Udp.beginPacketMulticast(ipUdpMulti, ipUdpPort, WiFi.localIP());
    Udp.write(udpOutBuffer, strlen(udpOutBuffer));
    Udp.endPacket();
  }
}

void processUdp() {
  JsonObject& message = jsonBuffer.parseObject(udpInBuffer);
  const char* dst = message["dst"];
  const char* src = message["src"];
  const char* type = message["type"];
  uint32_t timestamp = message["timestamp"];
  if (debug) {
    Serial.println(udpInBuffer);
    Serial.print(src);
    Serial.print(" ");
    Serial.print(type);
    Serial.print(" ");
    Serial.println(timestamp);
  }
  if (strcmp(type, "sync") == 0) {
    syncMilli = millis();
    return;
  }

  if (strcmp(type, "followup") == 0) {
    offsetMillis = (timestamp - syncMilli);
    delay(random(500));
    sprintf(udpOutBuffer, "{\"type\":\"delayrequest\",\"src\":\"%s\"}", LOCATION, now);
    Udp.beginPacketMulticast(ipUdpMulti, ipUdpPort, WiFi.localIP());
    Udp.write(udpOutBuffer, strlen(udpOutBuffer));
    Udp.endPacket();
    delayMilli = millissync();
    return;
  }

  if ((strcmp(type, "delayreply") == 0) && (strcmp(dst, LOCATION) == 0)) {
    offsetMillis -= (timestamp - delayMilli) / 2.0;
    return;
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
    processUdp();
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

