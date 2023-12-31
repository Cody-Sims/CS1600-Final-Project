#ifndef FINAL_PROJECT_1600_H
#define FINAL_PROJECT_1600_H

#include <WiFi101.h>
#include <WiFiUdp.h>

enum State {VERIFY_LIGHT_STATUS, LIGHTS_OFF_WAIT,  LIGHTS_ON_WAIT, PRESS_BUTTON};

/*
 A struct to keep all three state inputs in one place. Time is also an input, 
 but we just call milis() from within the FSM
 */
typedef struct {
  int light_amt;
  bool claps;
} state_inputs;


// Function Declarations
void initializeWifi();
void connectToNTP();
void handleWiFiClient();
void sendResponseToClient(WiFiClient &client);
void printWiFiStatus();
State updateFSM(State currentState, state_inputs inputs);
state_inputs updateInputs();
String getCurrentTime();
String urlDecode(String str);
unsigned long sendNTPpacket(IPAddress& address);
unsigned long getSecsSince1900();
void setSecsSince1900(unsigned long time);





// Global Variables
extern String wakeup_time;
extern String sleep_time;
extern WiFiServer server;
extern WiFiUDP Udp;
extern const int NTP_PACKET_SIZE;
extern byte packetBuffer[];
extern int button_pushes;
#endif
