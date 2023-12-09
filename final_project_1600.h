#ifndef FINAL_PROJECT_1600_H
#define FINAL_PROJECT_1600_H

#include <WiFi101.h>
#include <WiFiUdp.h>

// Function Declarations
void initializeWifi();
void connectToNTP();
void handleWiFiClient();
void sendResponseToClient(WiFiClient &client);
void printWiFiStatus();

String getCurrentTime();
String urlDecode(String str);

unsigned long sendNTPpacket(IPAddress& address);



// Global Variables
extern String wakeup_time;
extern String sleep_time;
extern WiFiServer server;
extern WiFiUDP Udp;
extern const int NTP_PACKET_SIZE;
extern byte packetBuffer[];

#endif
